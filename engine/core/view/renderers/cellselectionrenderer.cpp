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

	void CellSelectionRenderer::reset() {
		m_locations.clear();
	}

	void CellSelectionRenderer::selectLocation(const Location* loc) {
		if (loc) {
			std::vector<Location>::const_iterator it = m_locations.begin();
			for (; it != m_locations.end(); it++) {
				if ((*it).getLayerCoordinates() == loc->getLayerCoordinates()) {
					return;
				}
			}

			m_locations.push_back(Location(*loc));
		}
	}

	void CellSelectionRenderer::deselectLocation(const Location* loc) {
		if (loc) {
			std::vector<Location>::iterator it = m_locations.begin();
			for (; it != m_locations.end(); it++) {
				if ((*it).getLayerCoordinates() == loc->getLayerCoordinates()) {
					m_locations.erase(it);
					break;
				}
			}
		}
	}

	void CellSelectionRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		if (m_locations.empty()) {
			return;
		}

		std::vector<Location>::const_iterator locit = m_locations.begin();
		for (; locit != m_locations.end(); locit++) {
			const Location loc = *locit;
			if (layer != loc.getLayer()) {
				continue;
			}

			CellGrid* cg = layer->getCellGrid();
			if (!cg) {
				FL_WARN(_log, "No cellgrid assigned to layer, cannot draw selection");
				continue;
			}

			std::vector<ExactModelCoordinate> vertices;
			cg->getVertices(vertices, loc.getLayerCoordinates());
			std::vector<ExactModelCoordinate>::const_iterator it = vertices.begin();
			ScreenPoint firstpt = cam->toScreenCoordinates(cg->toMapCoordinates(*it));
			Point pt1(firstpt.x, firstpt.y);
			Point pt2;
			++it;
			for (; it != vertices.end(); it++) {
				ScreenPoint pts = cam->toScreenCoordinates(cg->toMapCoordinates(*it));
				pt2.x = pts.x; pt2.y = pts.y;
				Point cpt1 = pt1;
				Point cpt2 = pt2;
				m_renderbackend->drawLine(cpt1, cpt2, m_color.r, m_color.g, m_color.b);
				pt1 = pt2;
			}
			m_renderbackend->drawLine(pt2, Point(firstpt.x, firstpt.y), m_color.r, m_color.g, m_color.b);
		}
	}

	void CellSelectionRenderer::setColor(uint8_t r, uint8_t g, uint8_t b) {
		m_color.r = r;
		m_color.g = g;
		m_color.b = b;
	}
}
