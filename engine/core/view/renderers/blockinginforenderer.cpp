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
		RendererBase(renderbackend, position) {
		setEnabled(false);
		m_color.r = 0;
		m_color.g = 255;
		m_color.b = 0;
	}

	BlockingInfoRenderer::BlockingInfoRenderer(const BlockingInfoRenderer& old):
		RendererBase(old),
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

	void BlockingInfoRenderer::setEnabled(bool enabled) {
		RendererBase::setEnabled(enabled);
		if (!enabled) {
			m_bufferMap.clear();
		}
	}

	void BlockingInfoRenderer::removeActiveLayer(Layer* layer) {
		RendererBase::removeActiveLayer(layer);
		std::map<Layer*, std::vector<Point> >::iterator it = m_bufferMap.find(layer);
		if (it != m_bufferMap.end()) {
			m_bufferMap.erase(it);
		}
	}

	void BlockingInfoRenderer::clearActiveLayers() {
		RendererBase::clearActiveLayers();
		m_bufferMap.clear();
	}

	void BlockingInfoRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		CellGrid* cg = layer->getCellGrid();
		if (!cg) {
			FL_WARN(_log, "No cellgrid assigned to layer, cannot draw grid");
			return;
		}

		// Only render if nothing has changed.
		if (!cam->isUpdated() && !cam->isLayerCacheUpdated(layer)) {
			if (m_bufferMap.find(layer) != m_bufferMap.end()) {
				renderBuffer(layer);
				return;
			}
		}

		std::vector<Point>& data = m_bufferMap[layer];
		// clear buffer
		data.clear();
		CellCache* cache = layer->getCellCache();
		if (cache) {
			// fill buffer with cell data
			Rect layerView = cam->getLayerViewPort(layer);
			std::vector<Cell*> cells = cache->getCellsInRect(layerView);
			std::vector<Cell*>::iterator cit = cells.begin();
			for (; cit != cells.end(); ++cit) {
				if ((*cit)->getCellType() == CTYPE_NO_BLOCKER) {
					continue;
				}
				std::vector<ExactModelCoordinate> vertices;
				cg->getVertices(vertices, (*cit)->getLayerCoordinates());
				std::vector<ExactModelCoordinate>::const_iterator it = vertices.begin();
				ScreenPoint firstpt = cam->toScreenCoordinates(cg->toMapCoordinates(*it));
				Point pt(firstpt.x, firstpt.y);
				data.push_back(pt);
				++it;
				for (; it != vertices.end(); ++it) {
					ScreenPoint pts = cam->toScreenCoordinates(cg->toMapCoordinates(*it));
					pt.x = pts.x;
					pt.y = pts.y;
					data.push_back(pt);
				}
			}
		} else {
			// fill buffer with instance data
			RenderList::const_iterator instance_it = instances.begin();
			for (; instance_it != instances.end(); ++instance_it) {
				Instance* instance = (*instance_it)->instance;
				if (!instance->getObject()->isBlocking() || !instance->isBlocking()) {
					continue;
				}
				std::vector<ExactModelCoordinate> vertices;
				cg->getVertices(vertices, instance->getLocationRef().getLayerCoordinates());
				std::vector<ExactModelCoordinate>::const_iterator it = vertices.begin();
				ScreenPoint firstpt = cam->toScreenCoordinates(cg->toMapCoordinates(*it));
				Point pt(firstpt.x, firstpt.y);
				data.push_back(pt);
				++it;
				for (; it != vertices.end(); ++it) {
					ScreenPoint pts = cam->toScreenCoordinates(cg->toMapCoordinates(*it));
					pt.x = pts.x;
					pt.y = pts.y;
					data.push_back(pt);
				}
			}
		}
		// render
		renderBuffer(layer);
	}

	void BlockingInfoRenderer::setColor(uint8_t r, uint8_t g, uint8_t b) {
		m_color.r = r;
		m_color.g = g;
		m_color.b = b;
	}

	void BlockingInfoRenderer::renderBuffer(Layer* layer) {
		std::vector<Point>& data = m_bufferMap[layer];
		uint32_t index = 0;
		uint32_t indexEnd = data.size();
		if (layer->getCellGrid()->getType() == "square") {
			while (index != indexEnd) {
				m_renderbackend->drawLine(data[index], data[index + 1], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 1], data[index + 2], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 2], data[index + 3], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 3], data[index], m_color.r, m_color.g, m_color.b);

				m_renderbackend->drawLine(data[index], data[index + 2], m_color.r, m_color.g, m_color.b);
				index += 4;
			}
		}
		else {
			while (index != indexEnd) {
				m_renderbackend->drawLine(data[index], data[index + 1], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 1], data[index + 2], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 2], data[index + 3], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 3], data[index + 4], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 4], data[index + 5], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 5], data[index], m_color.r, m_color.g, m_color.b);

				m_renderbackend->drawLine(data[index], data[index + 3], m_color.r, m_color.g, m_color.b);
				index += 6;
			}
		}
	}
}
