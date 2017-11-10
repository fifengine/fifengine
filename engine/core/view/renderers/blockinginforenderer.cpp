/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"
#include "video/rendercache.h"
#include "util/math/fife_math.h"
#include "util/log/logger.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"
#include "model/structures/cell.h"
#include "model/structures/cellcache.h"

#include "view/camera.h"
#include "blockinginforenderer.h"


namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_VIEWVIEW);

	BlockingInfoRenderer::BlockingInfoRenderer(RenderBackend* renderbackend, int32_t position):
		RendererBase(renderbackend, position),
		m_updated(true) {
		setEnabled(false);
		m_color.r = 0;
		m_color.g = 255;
		m_color.b = 0;
	}

	BlockingInfoRenderer::BlockingInfoRenderer(const BlockingInfoRenderer& old):
		RendererBase(old),
		m_updated(true),
		m_color(old.m_color) {
		setEnabled(false);
	}

	RendererBase* BlockingInfoRenderer::clone() {
		return new BlockingInfoRenderer(*this);
	}

	BlockingInfoRenderer::~BlockingInfoRenderer() {
	}

	BlockingInfoRenderer* BlockingInfoRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<BlockingInfoRenderer*>(cnt->getRenderer("BlockingInfoRenderer"));
	}

	void BlockingInfoRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		CellGrid* cg = layer->getCellGrid();
		if (!cg) {
			FL_WARN(_log, "No cellgrid assigned to layer, can not draw blocking info");
			return;
		}

		RenderCache* renderCache = getRenderCache(layer);
		// full update on start, camera change or color change
		if (!renderCache || cam->isUpdated() || m_updated) {
			if (!renderCache) {
				renderCache = createRenderCache(layer);
			}
			fullUpdate(cam, layer, instances, renderCache);
		} else if (cam->isLayerCacheUpdated(layer)) {
			// update data on Instance changes
			update(cam, layer, renderCache);
		}

		// render
		renderCache->render();
	}

	void BlockingInfoRenderer::fullUpdate(Camera* cam, Layer* layer, RenderList& instances, RenderCache* renderCache) {
		CellGrid* cg = layer->getCellGrid();

		renderCache->clear();
		m_fifeIdToBufferId.clear();
		uint32_t pos = 0;
		const uint32_t cellSideCount = cg->getCellSideCount();
		const uint32_t halfCell = cellSideCount / 2;
		std::vector<Point> data(cellSideCount + 2);
		Color color(m_color.r, m_color.g, m_color.b, 255);
		CellCache* cache = layer->getCellCache();
		if (cache) {
			// fill render cache with cell data
			Rect layerView = cam->getLayerViewPort(layer);
			std::vector<Cell*> cells = cache->getBlockingCellsInRect(layerView);
			std::vector<Cell*>::iterator cit = cells.begin();
			for (; cit != cells.end(); ++cit) {
				// get Instances on Cell
				const std::set<Instance*>& instances2 = (*cit)->getInstances();
				for (std::set<Instance*>::const_iterator it = instances2.begin(); it != instances2.end(); ++it) {
					m_fifeIdToBufferId[(*it)->getFifeId()] = pos;
				}
				std::vector<ExactModelCoordinate> vertices;
				cg->getVertices(vertices, (*cit)->getLayerCoordinates());
				for (uint32_t i = 0; i < cellSideCount; ++i) {
					ScreenPoint pts = cam->toScreenCoordinates(cg->toMapCoordinates(vertices[i]));
					data[i].x = pts.x;
					data[i].y = pts.y;
				}
				data[cellSideCount] = data[0];
				data[cellSideCount + 1] = data[halfCell];
				renderCache->addLines(data, color);
				++pos;
			}
		} else {
			// fill render cache with instance data
			RenderList::const_iterator instance_it = instances.begin();
			for (; instance_it != instances.end(); ++instance_it) {
				Instance* instance = (*instance_it)->instance;
				if (!instance->getObject()->isBlocking() || !instance->isBlocking()) {
					continue;
				}
				m_fifeIdToBufferId[instance->getFifeId()] = pos;
				std::vector<ExactModelCoordinate> vertices;
				cg->getVertices(vertices, instance->getLocationRef().getLayerCoordinates());
				for (uint32_t i = 0; i < cellSideCount; ++i) {
					ScreenPoint pts = cam->toScreenCoordinates(cg->toMapCoordinates(vertices[i]));
					data[i].x = pts.x;
					data[i].y = pts.y;
				}
				data[cellSideCount] = data[0];
				data[cellSideCount + 1] = data[halfCell];
				renderCache->addLines(data, color);
				++pos;
			}
		}
		m_updated = false;
	}

	void BlockingInfoRenderer::update(Camera* cam, Layer* layer, RenderCache* renderCache) {
		CellGrid* cg = layer->getCellGrid();
		Color color(m_color.r, m_color.g, m_color.b, 255);
		const uint32_t cellSideCount = cg->getCellSideCount();
		const uint32_t halfCell = cellSideCount / 2;
		const uint32_t elements = cellSideCount + 2;
		std::vector<Point> data(elements);
		// TODO: Add code for cam->getRemovedInstances(layer)
		// update Instance data
		const std::set<Instance*>& updatedInstances = cam->getUpdatedInstances(layer);
		std::set<Instance*>::const_iterator it = updatedInstances.begin();
		for (; it != updatedInstances.end(); ++it) {
			std::map<std::size_t, uint32_t>::iterator it2 = m_fifeIdToBufferId.find((*it)->getFifeId());
			if (it2 == m_fifeIdToBufferId.end()) continue;
			uint32_t pos = it2->second;

			std::vector<ExactModelCoordinate> vertices;
			cg->getVertices(vertices, (*it)->getLocationRef().getLayerCoordinates());
			for (uint32_t i = 0; i < cellSideCount; ++i) {
				ScreenPoint pts = cam->toScreenCoordinates(cg->toMapCoordinates(vertices[i]));
				data[i].x = pts.x;
				data[i].y = pts.y;
			}
			data[cellSideCount] = data[0];
			data[cellSideCount + 1] = data[halfCell];
			renderCache->updateLines(pos * elements, data, color);
		}
	}

	void BlockingInfoRenderer::setColor(uint8_t r, uint8_t g, uint8_t b) {
		m_color.r = r;
		m_color.g = g;
		m_color.b = b;
		m_updated = true;
	}
}
