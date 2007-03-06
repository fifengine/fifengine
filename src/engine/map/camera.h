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

#ifndef FIFE_MAP_CAMERA_H
#define FIFE_MAP_CAMERA_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/point.h"
#include "video/rect.h"
#include "timer.h"

namespace FIFE { namespace map {

	class View;
	class Layer;
	class Control;

	/** A Camera onto the MapView
	 *
	 *  This class is responsible for smoothly scrolling
	 *  to positions of interest.
	 *  Currently it just lineary moves to the designated
	 *  positon in the coordinates of the 'reference' grid
	 *  of the current elevation.
	 * 
	 *  In the long run, this class shall be responsible for
	 *  setting the viewport of the mapview alone.
	 *  With following modes of operation:
	 *  * Free Camera
	 *  * Tracking a critters movement.
	 *  * Zoom to different location
	 *
	 *  I can imagine this being put to good use for
	 *  story telling.
	 */
	class Camera {
		public:
			enum {
				ZOOMING  = 1,
				FREE     = 2,
				TRACKING = 3
			};

			Camera(Control* map_control);
			~Camera();

			/** Set on screen viewport
			 */
			void setViewport(const Rect& viewport);

			/** Start zooming to a specific positon.
			 * Switches to mode ZOOMING
			 */
			void zoomTo(const Point& gridPosition);

			/** Move (FREE) Camera by delta pixels
			 */
			void moveBy(const Point& delta);

			/** Move (FREE) Camera to grid pos
			 */
			void moveTo(const Point& gridPosition);

			/** Start tracking
			 * FIXME: NOT TESTED
			 */
			void track(size_t visualId);

			void update();

			void render();
		private:
			Layer* m_layer;
			View*  m_view;
			Point m_position;
			Point m_next_position;
			size_t m_tracked_visual;
			Rect  m_viewport;
			int   m_mode;
			Timer m_timer;

			friend class Control;
			Control* m_control;

			/** Reset Camera
			 */
			void reset();
			/** Control deleted
			 */
			void controlDeleted();
	};
} } //FIFE::map

#endif
