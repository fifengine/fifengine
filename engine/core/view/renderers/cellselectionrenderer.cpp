/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE Team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"
#include "util/logger.h"

#include "util/fife_math.h"
#include "util/logger.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "view/camera.h"
#include "view/view.h"
#include "cellselectionrenderer.h"


namespace FIFE {
	static Logger _log(LM_VIEWVIEW);

	CellSelectionRenderer::CellSelectionRenderer(RenderBackend* renderbackend, int position):
		RendererBase(renderbackend, position),
		m_loc(NULL) {
		setEnabled(true);
	}

	CellSelectionRenderer::~CellSelectionRenderer() {
	}

	CellSelectionRenderer* CellSelectionRenderer::getInstance(View* view) {
		return dynamic_cast<CellSelectionRenderer*>(view->getRenderer("CellSelectionRenderer"));
	}
	
	void CellSelectionRenderer::reset() {
		delete m_loc;
		m_loc = NULL;
	}

	void CellSelectionRenderer::selectLocation(Location* loc) {
		delete m_loc;
		m_loc = NULL;
		if (loc) {
			m_loc = new Location(*loc);
		}
	}

	void CellSelectionRenderer::render(Camera* cam, Layer* layer, std::vector<Instance*>& instances) {
		if (!m_loc) {
			return;
		}
		
		if (layer != m_loc->getLayer()) {
			return;
		}
		
		CellGrid* cg = layer->getCellGrid();
		if (!cg) {
			FL_WARN(_log, "No cellgrid assigned to layer, cannot draw selection");
			return;
		}

		std::vector<ExactModelCoordinate> vertices;
		cg->getVertices(vertices, m_loc->getLayerCoordinates());
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
