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


namespace FIFE {
	class View;

	class Camera {
	public:
		/** Constructor
		 * Cameras are bind to a view that takes care also for drawing them
		 * therefore this method should really be called only from view
		 * or test code
		 */
		Camera(View* view);
		~Camera();

		void setTilt(double tilt) { m_tilt = tilt; }
		double getTilt() { return m_tilt; }

		void setRotation(double rotation) { m_rotation = rotation; }
		double getRotation() { return m_rotation; }

		void setZoom(double zoom) { m_zoom = zoom; }
		double getZoom() { return m_zoom; }

		void update();

	private:
		View* m_view;
		double m_tilt;
		double m_rotation;
		double m_zoom;
	};

}
#endif
