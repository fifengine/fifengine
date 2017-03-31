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

#include "view/camera.h"
#include "cellselectionrenderer.h"


namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_VIEWVIEW);

	CellSelectionRenderer::CellSelectionRenderer(RenderBackend* renderbackend, int32_t position):
		RendererBase(renderbackend, position) {
		setEnabled(false);
		m_color.r = 255;
		m_color.g = 0;
		m_color.b = 0;
	}

 	CellSelectionRenderer::CellSelectionRenderer(const CellSelectionRenderer& old):
		RendererBase(old),
		m_color(old.m_color) {
		setEnabled(false);
	}

	RendererBase* CellSelectionRenderer::clone() {
		return new CellSelectionRenderer(*this);
	}

	CellSelectionRenderer::~CellSelectionRenderer() {
	}

	CellSelectionRenderer* CellSelectionRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<CellSelectionRenderer*>(cnt->getRenderer("CellSelectionRenderer"));
	}

	void CellSelectionRenderer::setEnabled(bool enabled) {
		RendererBase::setEnabled(enabled);
		if (!enabled) {
			m_bufferMap.clear();
			m_updates.clear();
		}
	}

	void CellSelectionRenderer::removeActiveLayer(Layer* layer) {
		RendererBase::removeActiveLayer(layer);
		std::map<Layer*, std::vector<Point> >::iterator it = m_bufferMap.find(layer);
		if (it != m_bufferMap.end()) {
			m_bufferMap.erase(it);
		}

		std::map<Layer*, bool>::iterator uit = m_updates.find(layer);
		if (uit != m_updates.end()) {
			m_updates.erase(uit);
		}
	}

	void CellSelectionRenderer::clearActiveLayers() {
		RendererBase::clearActiveLayers();
		m_bufferMap.clear();
		m_updates.clear();
	}

	void CellSelectionRenderer::reset() {
		m_locations.clear();
		m_bufferMap.clear();
		m_updates.clear();
	}

	void CellSelectionRenderer::selectLocation(const Location* loc) {
		if (loc) {
			std::vector<Location>::const_iterator it = m_locations.begin();
			for (; it != m_locations.end(); it++) {
				if ((*it).getLayerCoordinates() == loc->getLayerCoordinates() && (*it).getLayer() == loc->getLayer()) {
					return;
				}
			}

			m_locations.push_back(Location(*loc));
			m_updates[loc->getLayer()] = true;
		}
	}

	void CellSelectionRenderer::deselectLocation(const Location* loc) {
		if (loc) {
			std::vector<Location>::iterator it = m_locations.begin();
			for (; it != m_locations.end(); it++) {
				if ((*it).getLayerCoordinates() == loc->getLayerCoordinates() && (*it).getLayer() == loc->getLayer()) {
					m_locations.erase(it);
					m_updates[loc->getLayer()] = true;
					break;
				}
			}
		}
	}

	void CellSelectionRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		if (m_locations.empty()) {
			return;
		}

		CellGrid* cg = layer->getCellGrid();
		if (!cg) {
			FL_WARN(_log, "No cellgrid assigned to layer, cannot draw selection");
			return;
		}

		// Only render if nothing has changed.
		if (!cam->isUpdated() && !cam->isLayerCacheUpdated(layer) && !m_updates[layer]) {
			if (m_bufferMap.find(layer) != m_bufferMap.end()) {
				renderBuffer(layer);
				return;
			}
		}

		std::vector<Point>& data = m_bufferMap[layer];
		data.clear();
		// fill buffer
		std::vector<Location>::const_iterator locit = m_locations.begin();
		for (; locit != m_locations.end(); locit++) {
			const Location loc = *locit;
			if (layer != loc.getLayer()) {
				continue;
			}

			std::vector<ExactModelCoordinate> vertices;
			cg->getVertices(vertices, loc.getLayerCoordinates());
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
		m_updates[layer] = false;
		// render
		renderBuffer(layer);
	}

	void CellSelectionRenderer::setColor(uint8_t r, uint8_t g, uint8_t b) {
		m_color.r = r;
		m_color.g = g;
		m_color.b = b;
	}

	void CellSelectionRenderer::renderBuffer(Layer* layer) {
		std::vector<Point>& data = m_bufferMap[layer];
		uint32_t index = 0;
		uint32_t indexEnd = data.size();
		if (layer->getCellGrid()->getType() == "square") {
			while (index != indexEnd) {
				m_renderbackend->drawLine(data[index], data[index + 1], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 1], data[index + 2], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 2], data[index + 3], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 3], data[index], m_color.r, m_color.g, m_color.b);
				index += 4;
			}
		} else {
			while (index != indexEnd) {
				m_renderbackend->drawLine(data[index], data[index + 1], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 1], data[index + 2], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 2], data[index + 3], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 3], data[index + 4], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 4], data[index + 5], m_color.r, m_color.g, m_color.b);
				m_renderbackend->drawLine(data[index + 5], data[index], m_color.r, m_color.g, m_color.b);
				index += 6;
			}
		}
	}
}
