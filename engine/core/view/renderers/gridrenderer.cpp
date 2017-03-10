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
#include "gridrenderer.h"


namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_VIEWVIEW);

	GridRenderer::GridRenderer(RenderBackend* renderbackend, int32_t position):
		RendererBase(renderbackend, position) {
		setEnabled(false);
		m_color.r = 0;
		m_color.g = 255;
		m_color.b = 0;
	}

 	GridRenderer::GridRenderer(const GridRenderer& old):
		RendererBase(old),
		m_color(old.m_color) {
		setEnabled(false);
	}

	RendererBase* GridRenderer::clone() {
		return new GridRenderer(*this);
	}

	GridRenderer::~GridRenderer() {
	}

	GridRenderer* GridRenderer::getInstance(IRendererContainer* cnt) {
		return dynamic_cast<GridRenderer*>(cnt->getRenderer("GridRenderer"));
	}

	void GridRenderer::render(Camera* cam, Layer* layer, RenderList& instances) {
		CellGrid* cg = layer->getCellGrid();
		if (!cg) {
			FL_WARN(_log, "No cellgrid assigned to layer, cannot draw grid");
			return;
		}

//
//		//render elev_coord box
//		//draw front quad
//		// 1,1,1
//		//1,-1,1
//		//-1,-1,1
//		//-1,1,1
		//We don't need the elevation box atm
//		Point a,b,c,d;
//
//
//		ScreenPoint copt1 =cam->toScreenCoordinates(ExactModelCoordinate(1,1,1) );
//		ScreenPoint copt2 =cam->toScreenCoordinates(ExactModelCoordinate(1,-1,1) );
//		Point coptt1(copt1.x,copt1.y);
//		Point coptt2(copt2.x,copt2.y);
//		m_renderbackend->drawLine(coptt1,coptt2 ,15, 15, 200);
//		a = coptt1;
//
//		copt1 =cam->toScreenCoordinates(ExactModelCoordinate(1,-1,1) );
//		copt2 =cam->toScreenCoordinates(ExactModelCoordinate(-1,-1,1) );
//		coptt1 = Point(copt1.x,copt1.y);
//		coptt2 = Point(copt2.x,copt2.y);
//		m_renderbackend->drawLine(coptt1,coptt2 ,15, 15, 200);
//		b = coptt1;
//
//		copt1 =cam->toScreenCoordinates(ExactModelCoordinate(-1,-1,1) );
//		copt2 =cam->toScreenCoordinates(ExactModelCoordinate(-1,1,1) );
//		coptt1 = Point(copt1.x,copt1.y);
//		coptt2 = Point(copt2.x,copt2.y);
//		m_renderbackend->drawLine(coptt1,coptt2 ,15, 15, 200);
//		c = coptt1;
//
//		copt1 =cam->toScreenCoordinates(ExactModelCoordinate(-1,1,1) );
//		copt2 =cam->toScreenCoordinates(ExactModelCoordinate(1,1,1) );
//		coptt1 = Point(copt1.x,copt1.y);
//		coptt2 = Point(copt2.x,copt2.y);
//		m_renderbackend->drawLine(coptt1,coptt2 ,15, 15, 20);
//		d = coptt1;
//
//		m_renderbackend->drawQuad(a,b,c,d,15, 15, 200);
//
//
//		//draw back quad
//		copt1 =cam->toScreenCoordinates(ExactModelCoordinate(-1,-1,-1) );
//		copt2 =cam->toScreenCoordinates(ExactModelCoordinate(-1,1,-1) );
//		coptt1 = Point(copt1.x,copt1.y);
//		coptt2 = Point(copt2.x,copt2.y);
//		m_renderbackend->drawLine(coptt1,coptt2 ,200, 200, 200);
//
//		copt1 =cam->toScreenCoordinates(ExactModelCoordinate(-1,1,-1) );
//		copt2 =cam->toScreenCoordinates(ExactModelCoordinate(1,1,-1) );
//		coptt1 = Point(copt1.x,copt1.y);
//		coptt2 = Point(copt2.x,copt2.y);
//		m_renderbackend->drawLine(coptt1,coptt2 ,200, 200, 200);
//
//		copt1 =cam->toScreenCoordinates(ExactModelCoordinate(1,1,-1) );
//		copt2 =cam->toScreenCoordinates(ExactModelCoordinate(1,-1,-1) );
//		coptt1 = Point(copt1.x,copt1.y);
//		coptt2 = Point(copt2.x,copt2.y);
//		m_renderbackend->drawLine(coptt1,coptt2 ,200, 200, 200);
//
//		copt1 =cam->toScreenCoordinates(ExactModelCoordinate(1,-1,-1) );
//		copt2 =cam->toScreenCoordinates(ExactModelCoordinate(-1,-1,-1) );
//		coptt1 = Point(copt1.x,copt1.y);
//		coptt2 = Point(copt2.x,copt2.y);
//		m_renderbackend->drawLine(coptt1,coptt2 ,200, 200, 200);


		// make the viewport 25% larger
		Rect cv = cam->getViewPort();
		int32_t cvx2 = round((cv.x+cv.w) * 1.25);
		int32_t cvy2 = round((cv.y+cv.h) * 1.25);
		cv.x -= round((cv.x+cv.w) * 0.125);
		cv.y -= round((cv.y+cv.h) * 0.125);
		RenderList::const_iterator instance_it = instances.begin();
		for (;instance_it != instances.end(); ++instance_it) {
			Instance* instance = (*instance_it)->instance;
			std::vector<ExactModelCoordinate> vertices;
			cg->getVertices(vertices, instance->getLocationRef().getLayerCoordinates());
			std::vector<ExactModelCoordinate>::const_iterator it = vertices.begin();
			ScreenPoint firstpt = cam->toScreenCoordinates(cg->toMapCoordinates(*it));
			Point pt1(firstpt.x, firstpt.y);
			Point pt2;
			++it;
			for (; it != vertices.end(); it++) {
				ScreenPoint pts = cam->toScreenCoordinates(cg->toMapCoordinates(*it));
				pt2.x = pts.x;
				pt2.y = pts.y;
				Point cpt1 = pt1;
				Point cpt2 = pt2;

				if (cpt1.x < cv.x) cpt1.x = cv.x;
				if (cpt2.x < cv.x) cpt2.x = cv.x;
				if (cpt1.y < cv.y) cpt1.y = cv.y;
				if (cpt2.y < cv.y) cpt2.y = cv.y;
				if (cpt1.x > cvx2) cpt1.x = cvx2;
				if (cpt2.x > cvx2) cpt2.x = cvx2;
				if (cpt1.y > cvy2) cpt1.y = cvy2;
				if (cpt2.y > cvy2) cpt2.y = cvy2;

				m_renderbackend->drawLine(cpt1, cpt2, m_color.r, m_color.g, m_color.b);
				pt1 = pt2;
			}
			if ((pt2.x >= cv.x) && (pt2.x <= cvx2) && (pt2.y >= cv.y) && (pt2.y <= cvy2)) {
				if ((firstpt.x >= cv.x) && (firstpt.x <= cvx2) && (firstpt.y >= cv.y) && (firstpt.y <= cvy2)) {
					m_renderbackend->drawLine(pt2, Point(firstpt.x, firstpt.y), m_color.r, m_color.g, m_color.b);
				}
			}
		}
	}

	void GridRenderer::setColor(Uint8 r, Uint8 g, Uint8 b) {
		m_color.r = r;
		m_color.g = g;
		m_color.b = b;
	}
}
