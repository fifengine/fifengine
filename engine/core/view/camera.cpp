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
#include "model/structures/instancetree.h"
#include "model/structures/instance.h"
#include "util/logger.h"

#include "camera.h"
#include "view.h"


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
			m_viewport(),
			m_screen_cell_width(1),
			m_screen_cell_height(1),
			m_reference_scale(1),
			m_enabled(true),
			m_attachedto(NULL) {
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

	/** Sets tilt for the camera.
		 * e.g. overhead camera has tilt 0, while traditional isometric camera has tilt 45
		 * @param tilt tilt for the camera
		 */
	void Camera::setTilt(double tilt) {

		m_tilt = tilt;
		updateReferenceScale();
		updateMatrices();
	}

	/** Gets camera tilt
	 * @return tilt of camera
	 */
	double Camera::getTilt() const {
		return m_tilt;
	}

	/** Sets rotation for the camera.
	 * Rotation can be visualized by thinking camera that rotates around an object
	 * that it is rendering
	 * @param rotation rotation for the camera
	 */
	void Camera::setRotation(double rotation) {
		m_rotation = rotation;
		updateReferenceScale();
		updateMatrices();
	}



	/** Gets camera rotation
	 * @return rotation of the camera
	 */
	double Camera::getRotation() const {
		return m_rotation;
	}

	/** Sets zoom for the camera.
	 * @param zoom zoom for the camera
	 */
	void Camera::setZoom(double zoom) {
		m_zoom = zoom;
		if (m_zoom < 0.001) {
			m_zoom = 0.001;
		}

		updateMatrices();
	}

	/** Gets camera zoom
	 * @return zoom of the camera
	 */
	double Camera::getZoom() const {
		return m_zoom;
	}

	void Camera::setCellImageDimensions(unsigned int width, unsigned int height) {
		m_screen_cell_width = width;
		m_screen_cell_height = height;
		updateReferenceScale();
		updateMatrices();
	}

	/** Sets the location for camera
	 * @param location location (center point) to render
	 */
	void Camera::setLocation(const Location& location) {
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
	}

  Point Camera::getCellImageDimensions() {
    return Point( m_screen_cell_width, m_screen_cell_height );
  }

	/** Gets the location camera is rendering
	 * @return camera location
	 */
	const Location& Camera::getLocation() const {
		return m_location;
	}

	/** Sets the viewport for camera
	 * viewport is rectangle inside the view where camera renders
	 * @param viewport area for camera render
	 */
	void Camera::setViewPort(const Rect& viewport) {
		m_viewport = viewport;
	}

	/** Gets the viewport for camera
	 * @return camera viewport
	 */
	const Rect& Camera::getViewPort() const {
		return m_viewport;
	}

	/** Sets camera enabled / disabled
	 */
	void Camera::setEnabled(bool enabled) {
		m_enabled = enabled;
	}

	/** Gets if camera is enabled / disabled
	 */
	bool Camera::isEnabled() {
		return m_enabled;
	}

	/** Update the camera transformation matrix T with requested values.
	 *  The requests are done using these functions :
	 * setLocation
	 * setRotation
	 * setTilt
	 */

	void Camera::updateMatrices() {
		double scale =   m_reference_scale;
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
		m_reference_scale = static_cast<double>(m_screen_cell_width) / (x2 - x1);

		FL_DBG(_log, "Updating reference scale");
		FL_DBG(_log, LMsg("   tilt=") << m_tilt << " rot=" << m_rotation);
		FL_DBG(_log, LMsg("   x1=") << x1 << " x2=" << x2 << " y1=" << y1 << " y2=" << y2);
		FL_DBG(_log, LMsg("   m_screen_cell_width=") << m_screen_cell_width);
	}
	
	void Camera::getMatchingInstances(ScreenPoint& screen_coords, Layer& layer, std::list<Instance*>& instances) {
		instances.clear();
		ScreenPoint pt(screen_coords);
		int dy = -(pt.y - toScreenCoordinates(getLocation().getMapCoordinates()).y);
		pt.z = (int)(tan(getTilt()* (M_PI / 180.0)) * dy);
		ExactModelCoordinate ece = toMapCoordinates(pt);
		ModelCoordinate ecl = layer.getCellGrid()->toLayerCoordinates(ece);
		InstanceTree* itree = layer.getInstanceTree();
		itree->findInstances(ecl, 0, 0, instances);
	}

	void Camera::attachToInstance( Instance *instance ) {
		m_attachedto = instance;
		update();
	}

	void Camera::detach() {
		m_attachedto = NULL;
	}

	void Camera::update() {
		if( !m_attachedto ) {
			return;
		}

		m_location.setExactLayerCoordinates( m_attachedto->getLocation().getExactLayerCoordinates());
		updateMatrices();
	}
}
