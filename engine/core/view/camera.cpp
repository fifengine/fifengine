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
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/layer.h"
#include "util/logger.h"

#include "camera.h"
#include "view.h"


namespace FIFE {
	static Logger _log(LM_CAMERA);
	
	Camera::Camera():
		m_matrix(),
		m_inverse_matrix(),
		m_tilt(0),
		m_rotation(0),
		m_zoom(1),
		m_location(),
		m_viewport(),
		m_screen_cell_width(1),
		m_screen_cell_height(1),
		m_reference_scale(1) {
		updateMatrices();
	}

	Camera::~Camera() {
	}

	void Camera::updateMatrices() {
		double scale = 1.0 / (m_zoom * m_reference_scale);
		m_matrix.loadScale(scale, scale, scale);
		m_matrix.applyRotate(m_rotation, 0.0, 0.0, 1.0);
		m_matrix.applyRotate(m_tilt, 1.0, 0.0, 0.0);
		if (m_location.getLayer()) {
			CellGrid* cg = m_location.getLayer()->getCellGrid();
			if (cg) {
				ExactModelCoordinate pt = m_location.getElevationCoordinates();
				m_matrix.applyTranslate(pt.x  * m_reference_scale, pt.y * m_reference_scale, 0);
			}
		}
		m_inverse_matrix = m_matrix.inverse();
	}

	ExactModelCoordinate Camera::toElevationCoordinates(ScreenPoint screen_coords) {
		screen_coords.x -= m_viewport.w / 2;
		screen_coords.y -= m_viewport.h / 2;
		ExactModelCoordinate p = m_matrix * intPt2doublePt(screen_coords);
		if (p.y > 0) {
			p.y = sqrt(p.z*p.z + p.y*p.y) / cos(m_tilt * (M_PI / 180.0));
		}
		else {
			p.y = -sqrt(p.z*p.z + p.y*p.y) / cos(m_tilt * (M_PI / 180.0));
		}
		return p;
	}

	ScreenPoint Camera::toScreenCoordinates(ExactModelCoordinate elevation_coords) {
		ExactModelCoordinate p = elevation_coords;
		// The math seems good to me, but this projection inversion
		// seems to break things. Even more surprising: commenting it
		// out makes things work! No complaints I guess... --jwt
		//p.y = sqrt(p.y*p.y*cos(m_tilt)*cos(m_tilt) - p.z*p.z);
		ScreenPoint pt = doublePt2intPt(m_inverse_matrix * p);
		pt.x += m_viewport.w / 2;
		pt.y += m_viewport.h / 2;
		return pt;
	}

	void Camera::updateReferenceScale() {
		CellGrid* cg = NULL;
		if (m_location.getLayer()) {
			cg = m_location.getLayer()->getCellGrid();
		}
		if (!cg) {
			return;
		}
		
		ModelCoordinate cell(0,0);
		std::vector<ExactModelCoordinate> vertices;
		cg->getVertices(vertices, cell);
		
		DoubleMatrix mtx;
		mtx.loadRotate(m_rotation, 0.0, 0.0, 1.0);
		mtx.applyRotate(m_tilt, 1.0, 0.0, 0.0);
		mtx = mtx.inverse();
		double x1, x2, y1, y2;
		for (unsigned int i = 0; i < vertices.size(); i++) {
			vertices[i] = cg->toElevationCoordinates(vertices[i]);
			vertices[i] = mtx * vertices[i];
			if (i == 0) {
				x1 = x2 = vertices[0].x;
				y1 = y2 = vertices[0].y;
			}
			else {
				x1 = std::min(vertices[i].x, x1);
				x2 = std::max(vertices[i].x, x2);
				y1 = std::min(vertices[i].y, y1);
				y2 = std::max(vertices[i].y, y2);
			}
		}
		m_reference_scale = static_cast<double>(m_screen_cell_width) / (x2 - x1);
		
		FL_DBG(_log, "Updating reference scale");
		FL_DBG(_log, LMsg("   tilt=") << m_tilt << " rot=" << m_rotation);
		FL_DBG(_log, LMsg("   x1=") << x1 << " x2=" << x2 << " y1=" << y1 << " y2=" << y2);
		FL_DBG(_log, LMsg("   m_screen_cell_width=") << m_screen_cell_width);
	}
}
