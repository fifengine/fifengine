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
#include "gridrenderer.h"


namespace FIFE {
	static Logger _log(LM_VIEWVIEW);

	GridRenderer::GridRenderer(RenderBackend* renderbackend, int position):
		RendererBase(renderbackend, position) {
		setEnabled(false);
	}

 	GridRenderer::GridRenderer(const GridRenderer& old):
		RendererBase(old) {
		setEnabled(false);
	}

	RendererBase* GridRenderer::clone() {
		return new GridRenderer(*this);
	}

	GridRenderer::~GridRenderer() {
	}

	void GridRenderer::render(Camera* cam, Layer* layer, std::vector<Instance*>& instances) {
		CellGrid* cg = layer->getCellGrid();
		if (!cg) {
			FL_WARN(_log, "No cellgrid assigned to layer, cannot draw grid");
			return;
		}
//
//
//		//render elev_coord box
//		//draw front quad
//		// 1,1,1
//		//1,-1,1
//		//-1,-1,1
//		//-1,1,1
		Point a,b,c,d;


		ScreenPoint copt1 =cam->toScreenCoordinates(ExactModelCoordinate(1,1,1) );
		ScreenPoint copt2 =cam->toScreenCoordinates(ExactModelCoordinate(1,-1,1) );
		Point coptt1(copt1.x,copt1.y);
		Point coptt2(copt2.x,copt2.y);
		m_renderbackend->drawLine(coptt1,coptt2 ,15, 15, 200);
		a = coptt1;

		copt1 =cam->toScreenCoordinates(ExactModelCoordinate(1,-1,1) );
		copt2 =cam->toScreenCoordinates(ExactModelCoordinate(-1,-1,1) );
		coptt1 = Point(copt1.x,copt1.y);
		coptt2 = Point(copt2.x,copt2.y);
		m_renderbackend->drawLine(coptt1,coptt2 ,15, 15, 200);
		b = coptt1;

		copt1 =cam->toScreenCoordinates(ExactModelCoordinate(-1,-1,1) );
		copt2 =cam->toScreenCoordinates(ExactModelCoordinate(-1,1,1) );
		coptt1 = Point(copt1.x,copt1.y);
		coptt2 = Point(copt2.x,copt2.y);
		m_renderbackend->drawLine(coptt1,coptt2 ,15, 15, 200);
		c = coptt1;

		copt1 =cam->toScreenCoordinates(ExactModelCoordinate(-1,1,1) );
		copt2 =cam->toScreenCoordinates(ExactModelCoordinate(1,1,1) );
		coptt1 = Point(copt1.x,copt1.y);
		coptt2 = Point(copt2.x,copt2.y);
		m_renderbackend->drawLine(coptt1,coptt2 ,15, 15, 20);
		d = coptt1;

		m_renderbackend->drawQuad(a,b,c,d,15, 15, 200);
//
//
//		//draw back quad
		copt1 =cam->toScreenCoordinates(ExactModelCoordinate(-1,-1,-1) );
		copt2 =cam->toScreenCoordinates(ExactModelCoordinate(-1,1,-1) );
		coptt1 = Point(copt1.x,copt1.y);
		coptt2 = Point(copt2.x,copt2.y);
		m_renderbackend->drawLine(coptt1,coptt2 ,200, 200, 200);

		copt1 =cam->toScreenCoordinates(ExactModelCoordinate(-1,1,-1) );
		copt2 =cam->toScreenCoordinates(ExactModelCoordinate(1,1,-1) );
		coptt1 = Point(copt1.x,copt1.y);
		coptt2 = Point(copt2.x,copt2.y);
		m_renderbackend->drawLine(coptt1,coptt2 ,200, 200, 200);

		copt1 =cam->toScreenCoordinates(ExactModelCoordinate(1,1,-1) );
		copt2 =cam->toScreenCoordinates(ExactModelCoordinate(1,-1,-1) );
		coptt1 = Point(copt1.x,copt1.y);
		coptt2 = Point(copt2.x,copt2.y);
		m_renderbackend->drawLine(coptt1,coptt2 ,200, 200, 200);

		copt1 =cam->toScreenCoordinates(ExactModelCoordinate(1,-1,-1) );
		copt2 =cam->toScreenCoordinates(ExactModelCoordinate(-1,-1,-1) );
		coptt1 = Point(copt1.x,copt1.y);
		coptt2 = Point(copt2.x,copt2.y);
		m_renderbackend->drawLine(coptt1,coptt2 ,200, 200, 200);


		Rect cv = cam->getViewPort();
		std::vector<Instance*>::const_iterator instance_it = instances.begin();
		for (;instance_it != instances.end(); ++instance_it) {
			Instance* instance = *instance_it;
			std::vector<ExactModelCoordinate> vertices;
			cg->getVertices(vertices, instance->getLocationRef().getLayerCoordinates());
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
				/* FIXME: limit grid drawing to current camera view port
				   code below does not do it, but may act as a starting point

				int cvx2 = cv.x+cv.w;
				int cvy2 = cv.y+cv.h;

				if (((pt1.x < cv.x) && (pt2.x < cv.x)) ||
				    ((pt1.x > cvx2) && (pt2.x > cvx2)) ||
				    ((pt1.y < cv.y) && (pt2.y < cv.y)) ||
				    ((pt1.y > cvy2) && (pt2.y > cvy2))) {
				    pt1 = pt2;
				    continue;
				}

				if (cpt1.x < cv.x) cpt1.x = cv.x;
				if (cpt2.x < cv.x) cpt2.x = cv.x;
				if (cpt1.y < cv.y) cpt1.y = cv.y;
				if (cpt2.y < cv.y) cpt2.y = cv.y;
				if (cpt1.x > cvx2) cpt1.x = cvx2;
				if (cpt2.x > cvx2) cpt2.x = cvx2;
				if (cpt1.y > cvy2) cpt1.y = cvy2;
				if (cpt2.y > cvy2) cpt2.y = cvy2;
				*/
				m_renderbackend->drawLine(cpt1, cpt2, 0, 255, 0);
				pt1 = pt2;
			}
			m_renderbackend->drawLine(pt2, Point(firstpt.x, firstpt.y), 0, 255, 0);
		}
	}
}
