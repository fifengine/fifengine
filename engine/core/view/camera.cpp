/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
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
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/layer.h"
#include "model/structures/instancetree.h"
#include "model/structures/instance.h"
#include "util/logger.h"
#include "util/fife_math.h"
#include "video/image.h"

#include "camera.h"
#include "view.h"
#include "visual.h"


namespace FIFE {
	static Logger _log(LM_CAMERA);

	Camera::Camera(const std::string& id, Layer *layer, Rect viewport, ExactModelCoordinate emc):
			m_id(id),
			m_matrix(),
			m_inverse_matrix(),
			m_tilt(0),
			m_rotation(0),
			m_zoom(1),
			m_location(),
			m_prev_origo(ScreenPoint(0,0,0)),
			m_cur_origo(ScreenPoint(0,0,0)),
			m_viewport(),
			m_screen_cell_width(1),
			m_screen_cell_height(1),
			m_reference_scale(1),
			m_enabled(true),
			m_attachedto(NULL),
			m_image_dimensions(),
			m_iswarped(false) {
		m_location.setLayer(layer);
		m_location.setExactLayerCoordinates(emc);
		m_viewport = viewport;
	}

	Camera::~Camera() {
	}

	int Camera::getAngleBetween(const Location& loc1, const Location& loc2) {
		ScreenPoint pt1 = this->toScreenCoordinates(loc1.getMapCoordinates());
		ScreenPoint pt2 = this->toScreenCoordinates(loc2.getMapCoordinates());
		double dy = pt2.y - pt1.y;
		double dx = pt2.x - pt1.x;

		int angle = static_cast<int>(atan2(-dy,dx)*(180.0/M_PI));

		FL_DBG(_log, LMsg("-> angle, pt1=") << pt1 << ", pt2=" << pt2 << ", angle=" << angle);
		return angle;
	}

	void Camera::setTilt(double tilt) {
		m_tilt = tilt;
		updateReferenceScale();
		updateMatrices();
		m_iswarped = true;
	}

	double Camera::getTilt() const {
		return m_tilt;
	}

	void Camera::setRotation(double rotation) {
		m_rotation = rotation;
		updateReferenceScale();
		updateMatrices();
		m_iswarped = true;
	}

	double Camera::getRotation() const {
		return m_rotation;
	}

	void Camera::setZoom(double zoom) {
		m_zoom = zoom;
		if (m_zoom < 0.001) {
			m_zoom = 0.001;
		}
		updateMatrices();
		m_iswarped = true;
	}

	double Camera::getZoom() const {
		return m_zoom;
	}

	void Camera::setCellImageDimensions(unsigned int width, unsigned int height) {
		m_screen_cell_width = width;
		m_screen_cell_height = height;
		updateReferenceScale();
		updateMatrices();
		m_iswarped = true;
	}

	ScreenPoint Camera::getLatestMovement() {
		return m_prev_origo - m_cur_origo;
	}

	void Camera::setLocation(const Location& location) {
		// initialize first set properly
		if ((m_prev_origo == m_cur_origo) && (m_prev_origo == ScreenPoint(0,0,0))) {
			m_cur_origo = toScreenCoordinates(ExactModelCoordinate(0,0,0));
			m_prev_origo = m_cur_origo;
		}	
		m_location = location;
		
		CellGrid* cg = NULL;
		if (m_location.getLayer()) {
			cg = m_location.getLayer()->getCellGrid();
		} else {
			throw Exception("Location without layer given to Camera::setLocation");
		}
		if (!cg) {
			throw Exception("Camera layer has no cellgrid specified");
		}
		
		updateMatrices();
		
		m_prev_origo = m_cur_origo;
		m_cur_origo = toScreenCoordinates(ExactModelCoordinate(0,0,0));
	}

	Point Camera::getCellImageDimensions() {
		return getCellImageDimensions(m_location.getLayer());
	}

	Point Camera::getCellImageDimensions(Layer* layer) {
		if (layer == m_location.getLayer()) {
			return Point( m_screen_cell_width, m_screen_cell_height );
		}
		std::map<Layer*, Point>::iterator it = m_image_dimensions.find(layer);
		if (it != m_image_dimensions.end()) {
			return it->second;
		}
		Point p;
		CellGrid* cg = layer->getCellGrid();
		assert(cg);
		DoublePoint dimensions = getLogicalCellDimensions(layer);
		p.x = static_cast<int>(round(m_reference_scale * dimensions.x));
		p.y = static_cast<int>(round(m_reference_scale * dimensions.y));
		m_image_dimensions[layer] = p;
		return p;
	}
	
	Location Camera::getLocation() const {
		return m_location;
	}

	Location& Camera::getLocationRef() {
		return m_location;
	}
	
	void Camera::setViewPort(const Rect& viewport) {
		m_viewport = viewport;
	}

	const Rect& Camera::getViewPort() const {
		return m_viewport;
	}

	void Camera::setEnabled(bool enabled) {
		m_enabled = enabled;
	}
	
	bool Camera::isEnabled() {
		return m_enabled;
	}

	void Camera::updateMatrices() {
		double scale = m_reference_scale;
		m_matrix.loadScale(scale, scale, scale);
		if (m_location.getLayer()) {
			CellGrid* cg = m_location.getLayer()->getCellGrid();
			if (cg) {
				ExactModelCoordinate pt = m_location.getMapCoordinates();
				m_matrix.applyTranslate( -pt.x *m_reference_scale,-pt.y *m_reference_scale, 0);
			}
		}
		scale = m_zoom;
		m_matrix.applyScale(scale, scale, scale);
		m_matrix.applyRotate(-m_rotation, 0.0, 0.0, 1.0);
		m_matrix.applyRotate(-m_tilt, 1.0, 0.0, 0.0);
		m_inverse_matrix = m_matrix.inverse();
	}

	ExactModelCoordinate Camera::toMapCoordinates(ScreenPoint screen_coords) {
		screen_coords.x -= m_viewport.w / 2;
		screen_coords.y -= m_viewport.h / 2;

		return m_inverse_matrix  * intPt2doublePt(screen_coords);
	}

	ScreenPoint Camera::toScreenCoordinates(ExactModelCoordinate elevation_coords) {
		ExactModelCoordinate p = elevation_coords;
		ScreenPoint pt = doublePt2intPt( m_matrix* p  );
		pt.x += m_viewport.w / 2;
		pt.y += m_viewport.h / 2;
		return pt;
	}

	DoublePoint Camera::getLogicalCellDimensions(Layer* layer) {
		CellGrid* cg = NULL;
		if (layer) {
			cg = layer->getCellGrid();
		}
		assert(cg);

		ModelCoordinate cell(0,0);
		std::vector<ExactModelCoordinate> vertices;
		cg->getVertices(vertices, cell);

		DoubleMatrix mtx;
		mtx.loadRotate(m_rotation, 0.0, 0.0, 1.0);
		mtx.applyRotate(m_tilt, 1.0, 0.0, 0.0);
		double x1, x2, y1, y2;
		for (unsigned int i = 0; i < vertices.size(); i++) {
			vertices[i] = cg->toMapCoordinates(vertices[i]);
			vertices[i] = mtx * vertices[i];
			if (i == 0) {
				x1 = x2 = vertices[0].x;
				y1 = y2 = vertices[0].y;
			} else {
				x1 = std::min(vertices[i].x, x1);
				x2 = std::max(vertices[i].x, x2);
				y1 = std::min(vertices[i].y, y1);
				y2 = std::max(vertices[i].y, y2);
			}
		}
		return DoublePoint( x2 - x1, y2 - y1 );
	}

	void Camera::updateReferenceScale() {
		DoublePoint dim = getLogicalCellDimensions(m_location.getLayer());
		m_reference_scale = static_cast<double>(m_screen_cell_width) / dim.x;

		FL_DBG(_log, "Updating reference scale");
		FL_DBG(_log, LMsg("   tilt=") << m_tilt << " rot=" << m_rotation);
		FL_DBG(_log, LMsg("   m_screen_cell_width=") << m_screen_cell_width);
	}
	
	void Camera::getMatchingInstances(ScreenPoint& screen_coords, Layer& layer, std::list<Instance*>& instances) {
		instances.clear();
		const std::vector<Instance*>& layer_instances = layer.getInstances();
		std::vector<Instance*>::const_iterator instance_it = layer_instances.begin();
		for (;instance_it != layer_instances.end(); ++instance_it) {
			Instance* i = (*instance_it);
			InstanceVisual* visual = i->getVisual<InstanceVisual>();
			InstanceVisualCacheItem& vc = visual->getCacheItem(this);
			if ((vc.dimensions.contains(Point(screen_coords.x, screen_coords.y)))) {
				assert(vc.image);
				Uint8 r, g, b, a;
				vc.image->getPixelRGBA(screen_coords.x - vc.dimensions.x, screen_coords.y - vc.dimensions.y, &r, &b, &g, &a);
				// instance is hit with mouse if not totally transparent
				if (a != 0) {
					instances.push_back(i);
				}
			}
		}
	}

	void Camera::attachToInstance( Instance *instance ) {
		m_attachedto = instance;
	}

	void Camera::detach() {
		m_attachedto = NULL;
	}

	void Camera::update() {
		if( !m_attachedto ) {
			return;
		}
		Location loc(m_location);
		loc.setExactLayerCoordinates( m_attachedto->getLocationRef().getExactLayerCoordinates(m_location.getLayer()) );
		setLocation(loc);
		updateMatrices();
	}
	
	void Camera::refresh() {
		updateMatrices();
		m_iswarped = true;
	}
	
	void Camera::resetUpdates() {
		m_iswarped = false;
		m_prev_origo = m_cur_origo;
	}
}
