/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
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
#include "model/structures/elevation.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "view/camera.h"
#include "gridrenderer.h"


namespace FIFE {
	static Logger _log(LM_VIEWVIEW);
	
	GridRenderer::GridRenderer(RenderBackend* renderbackend):
		m_renderbackend(renderbackend) {
	}
	
	GridRenderer::~GridRenderer() {
	}
	
	void GridRenderer::render(Camera* cam, Layer* layer, std::vector<Instance*>& instances, int stackpos) {
		if (stackpos != 0) {
			return;
		}
		
		CellGrid* cg = layer->getCellGrid();
		if (!cg) {
			FL_WARN(_log, "No cellgrid assigned to layer, cannot draw grid");
			return;
		}
		
		std::vector<Instance*>::const_iterator instance_it = instances.begin();
		for (;instance_it != instances.end(); ++instance_it) {
			Instance* instance = *instance_it;
			std::vector<ExactModelCoordinate> vertices;
			cg->getVertices(vertices, instance->getLocation().getLayerCoordinates());
			std::vector<ExactModelCoordinate>::const_iterator it = vertices.begin();
			ScreenPoint firstpt = cam->toScreenCoordinates(cg->toElevationCoordinates(*it));
			Point pt1(firstpt.x, firstpt.y);
			Point pt2;
			++it;
			while (it != vertices.end()) {
				ScreenPoint pts = cam->toScreenCoordinates(cg->toElevationCoordinates(*it));
				pt2.x = pts.x; pt2.y = pts.y;
				m_renderbackend->drawLine(pt1, pt2, 0, 255, 0);
				pt1 = pt2;
				++it;
			}
			m_renderbackend->drawLine(pt2, Point(firstpt.x, firstpt.y), 0, 255, 0);
		}
	}
}
