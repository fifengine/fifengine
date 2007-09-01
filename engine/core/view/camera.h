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

#ifndef FIFE_VIEW_CAMERA_H
#define FIFE_VIEW_CAMERA_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/location.h"
#include "util/matrix.h"
#include "util/rect.h"

namespace FIFE {

	/** Camera describes properties of a view port shown in the main screen
	 *  Main screen can have multiple cameras active simultanously
	 *  Different cameras can have different properties, like location
	 *  to shoot, zoom or tilt
	 */
	class Camera {
	public:
		/** Constructor
		 * Camera needs to be added to the view. If not done so, it is not rendered.
		 */
		Camera();

		/** Destructor
		 */
		virtual ~Camera();

		/** Sets tilt for the camera. 
		 * e.g. overhead camera has tilt 0, while traditional isometric camera has tilt 45
		 * @param tilt tilt for the camera
		 */
		void setTilt(double tilt) {
			m_tilt = tilt;
			updateMatrices();
		}

		/** Gets camera tilt
		 * @return tilt of camera
		 */ 
		double getTilt() const { return m_tilt; }

		/** Sets rotation for the camera. 
		 * Rotation can be visualized by thinking camera that rotates around an object 
		 * that it is rendering
		 * @param rotation rotation for the camera
		 */
		void setRotation(double rotation) {
			m_rotation = rotation;
			updateMatrices();
		}

		/** Gets camera rotation
		 * @return rotation of the camera
		 */ 
		double getRotation() const { return m_rotation; }

		/** Sets zoom for the camera. 
		 * @param zoom zoom for the camera
		 */
		void setZoom(double zoom) {
			m_zoom = zoom;
			updateMatrices();
		}

		/** Gets camera zoom
		 * @return zoom of the camera
		 */ 
		double getZoom() const { return m_zoom; }

		void setScreenCellWidth(unsigned int width) { 
			m_screen_cell_width = width;
			updateMatrices();
		}

		/** Sets the location for camera
		 * @param location location (center point) to render
		 */ 
		void setLocation(const Location& location) { 
			m_location = location;
			updateMatrices();
		}

		/** Gets the location camera is rendering
		 * @return camera location
		 */ 
		const Location& getLocation() const { return m_location; }

		/** Sets the viewport for camera
		 * viewport is rectangle inside the view where camera renders
		 * @param viewport area for camera render
		 */ 
		void setViewPort(const Rect& viewport) { m_viewport = viewport; }

		/** Gets the viewport for camera
		 * @return camera viewport
		 */ 
		const Rect& getViewPort() const { return m_viewport; }

		/** Transforms given point from screen coordinates to elevation coordinates
		 *  @return point in elevation coordinates
		 */
		DoublePoint toElevationCoords(Point screen_coords);

		/** Transforms given point from elevation coordinates to screen coordinates
		 *  @return point in screen coordinates
		 */
		Point toScreenCoords(DoublePoint elevation_coords);

	private:
		void updateMatrices();

		DoubleMatrix m_matrix;
		DoubleMatrix m_inverse_matrix;
		double m_tilt;
		double m_rotation;
		double m_zoom;
		Location m_location;
		Rect m_viewport;
		bool m_view_updated;
		unsigned int m_screen_cell_width;
	};
}
#endif
