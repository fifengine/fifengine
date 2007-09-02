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

#include "camera.h"
#include "view.h"


namespace FIFE {
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
		m_matrix.loadRotate(m_rotation, 0.0, 0.0, 1.0);
		m_matrix.applyRotate(m_tilt, 1.0, 0.0, 0.0);
		if (m_location.layer) {
			CellGrid* cg = m_location.layer->getCellGrid();
			if (cg) {
				DoublePoint pt = cg->toElevationCoords(m_location.position);
				m_matrix.applyTranslate(pt.x, pt.y, 0);
			}
		}
		double scale = 1.0 / (m_zoom * m_reference_scale);
		m_matrix.applyScale(scale, scale, scale);
		m_inverse_matrix = m_matrix.inverse();
	}

	DoublePoint Camera::toElevationCoords(Point screen_coords) {
		return m_matrix * intPt2doublePt(screen_coords);
	}

	Point Camera::toScreenCoords(DoublePoint elevation_coords) {
		//std::cout << "Camera::toScreenCoords:" << m_matrix;
		return doublePt2intPt(m_inverse_matrix * elevation_coords);
	}

	void Camera::updateReferenceScale() {
		CellGrid* cg = NULL;
		if (m_location.layer) {
			cg = m_location.layer->getCellGrid();
		}
		if (!cg) {
			return;
		}
		
		Point cell(0,0);
		std::vector<DoublePoint> vertices;
		cg->getVertices(vertices, cell);
		
		DoubleMatrix mtx;
		mtx.loadRotate(m_rotation, 0.0, 0.0, 1.0);
		mtx.applyRotate(m_tilt, 1.0, 0.0, 0.0);
		mtx = mtx.inverse();
		double x1, x2, y1, y2;
		for (unsigned int i = 0; i < vertices.size(); i++) {
			vertices[i] = cg->toElevationCoords(vertices[i]);
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
		double sw = static_cast<double>(m_screen_cell_width);
		double sh = static_cast<double>(m_screen_cell_height);
		m_reference_scale = static_cast<double>(m_screen_cell_width) / (x2 - x1);
		std::cout << "\n>>>>>>>> Camera::updateReferenceScale\n";
		std::cout << "   tilt=" << m_tilt << " rot=" << m_rotation << "\n";
		std::cout << "   x1=" << x1 << " x2=" << x2 << " y1=" << y1 << " y2=" << y2 << \
		             " ratio=" << (x2-x1)/(y2-y1);
		std::cout << "   imgw=" << sw << " imgh=" << sh << " ratio=" << sw/sh << "\n";
	}
}
