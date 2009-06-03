/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
	static Logger _log(LM_VIEWVIEW);

	CellSelectionRenderer::CellSelectionRenderer(RenderBackend* renderbackend, int position):
		RendererBase(renderbackend, position) {
		setEnabled(true);
	}

 	CellSelectionRenderer::CellSelectionRenderer(const CellSelectionRenderer& old):
		RendererBase(old) {
		setEnabled(true);
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
				if (*it == *loc) return;
			}

			m_locations.push_back(Location(*loc));
		}
	}

	void CellSelectionRenderer::deselectLocation(const Location* loc) {
		if (loc) {
			std::vector<Location>::iterator it = m_locations.begin();
			for (; it != m_locations.end(); it++) {
				if (*it == *loc) {
					m_locations.erase(it);
					break;
				}
			}
		}
	}

	void CellSelectionRenderer::render(Camera* cam, Layer* layer, std::vector<Instance*>& instances) {
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
				m_renderbackend->drawLine(cpt1, cpt2, 255, 0, 0);
				pt1 = pt2;
			}
			m_renderbackend->drawLine(pt2, Point(firstpt.x, firstpt.y), 255, 0, 0);
		}
	}
}
