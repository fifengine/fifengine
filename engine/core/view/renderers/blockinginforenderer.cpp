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
		// Only render if nothing has changed.
		if (renderCache && !cam->isUpdated() && cam->isLayerCacheUpdated(layer)) {
			bool found = false;
			// Only test code
			for (RenderList::iterator it = instances.begin(); it != instances.end(); ++it) {
				if ((*it)->instance->getId() == "NPC:girl") {
					std::map<std::size_t, uint32_t>::iterator it2 = m_test.find((*it)->instance->getFifeId());
					if (it2 == m_test.end()) continue;
					uint32_t pos = it2->second;
					uint32_t cellSideCount = cg->getCellSideCount();
					const uint32_t halfCell = cellSideCount / 2;
					std::vector<Point> data(cellSideCount + 2);
					Color color(m_color.r, m_color.g, m_color.b, 255);

					std::vector<ExactModelCoordinate> vertices;
					cg->getVertices(vertices, (*it)->instance->getLocationRef().getLayerCoordinates());
					for (uint32_t i = 0; i < cellSideCount; ++i) {
						ScreenPoint pts = cam->toScreenCoordinates(cg->toMapCoordinates(vertices[i]));
						data[i].x = pts.x;
						data[i].y = pts.y;
					}
					data[cellSideCount] = data[0];
					data[cellSideCount + 1] = data[halfCell];
					renderCache->updateLines(pos*6, data, color);
					found = true;
					break;
				}
			}
			if (found) {
				renderCache->render();
				return;
			}

		}
		//if (!cam->isUpdated() && !cam->isLayerCacheUpdated(layer) && !m_updated) {
		if (!cam->isUpdated() && !m_updated) {
			if (renderCache) {
				renderCache->render();
				return;
			}
		}

		if (!renderCache) {
			renderCache = createRenderCache(layer);
		}
		renderCache->clear();
		m_test.clear();
		uint32_t pos = 0;
		uint32_t cellSideCount = cg->getCellSideCount();
		const uint32_t halfCell = cellSideCount / 2;
		std::vector<Point> data(cellSideCount + 2);
		Color color(m_color.r, m_color.g, m_color.b, 255);
		CellCache* cache = layer->getCellCache();
		if (cache) {
			// fill render cache with cell data
			Rect layerView = cam->getLayerViewPort(layer);
			std::vector<Cell*> cells = cache->getCellsInRect(layerView);
			std::vector<Cell*>::iterator cit = cells.begin();
			for (; cit != cells.end(); ++cit) {
				if ((*cit)->getCellType() == CTYPE_NO_BLOCKER) {
					continue;
				}
				const std::set<Instance*>& instances2 = (*cit)->getInstances();
				for (std::set<Instance*>::const_iterator it = instances2.begin(); it != instances2.end(); ++it) {
					m_test[(*it)->getFifeId()] = pos;
				}
				std::vector<ExactModelCoordinate> vertices;
				cg->getVertices(vertices, (*cit)->getLayerCoordinates());
				for (uint32_t i = 0; i < cellSideCount; ++i) {
					ScreenPoint pts = cam->toScreenCoordinates(cg->toMapCoordinates(vertices[i]));
					data[i].x = pts.x;
					data[i].y = pts.y;
				}
				data[cellSideCount] = data[0];
				data[cellSideCount+1] = data[halfCell];
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
				/*if (instance->getId() == "NPC:girl") {
					std::cout << "found " << instance->getFifeId() << "\n";
				}*/
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
			}
		}
		m_updated = false;
		// render
		renderCache->render();
	}

	void BlockingInfoRenderer::setColor(uint8_t r, uint8_t g, uint8_t b) {
		m_color.r = r;
		m_color.g = g;
		m_color.b = b;
		m_updated = true;
	}
}
