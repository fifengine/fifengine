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

#ifndef FIFE_MAP_VIEW_H
#define FIFE_MAP_VIEW_H

// Standard C++ library includes
#include <cstddef>
#include <list>
#include <map>
#include <string>
#include <vector>

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/point.h"
#include "util/rect.h"
#include "eventchannel/mouse/ec_imouselistener.h"

namespace FIFE {

	class Screen;

}

namespace FIFE { namespace map {

	class Map;
	typedef boost::shared_ptr<Map> MapPtr;

	class Camera;

	class Elevation;
	typedef boost::shared_ptr<Elevation> ElevationPtr;

	class Layer;
	typedef boost::shared_ptr<Layer> LayerPtr;
	class Visual;
	class VisualTree;

	class Geometry;

	/** Renders a map to a surface 
	 */
	class View: public IMouseListener {
		public:

			/** Create a new empty View object.
			 * Use @c setMap() to initialize to show a map.
			 */
			View();

			/** Destructor
			 * Does not cleanup Map object!
			 */
			~View();

			/**   Set the surface where the map should be rendered
			 *
			 * @param surface the surface to render at
			 * @param rect mapview will render into this rect on surface
			 */
			void setViewport(Screen* surface, const Rect& rect);

			/** Set the surface where the map should be rendered
			 *
			 * @param surface the surface render at
			 */
			void setViewport(Screen* surface);

			/** Set the map elevation to render
			 *
			 * @param elevation the mapelevation to render
			 */
			void setMap(MapPtr map, size_t elev);

			/** Renders the map */
			void render();

			/** Set the current x-position
			 *
			 * Set the X-pos of the map. ATM in tiles, but this will change
			 * once the whole class gets optimized and ready to use.
			 * @param pos new x position
			 */
			void setXPos(unsigned int pos);

			/** Set the current y-position
			 *
			 * Set the Y-pos of the map. ATM in tiles, but this will change
			 * once the whole class gets optimized and ready to use.
			 * @param pos new y position
			 */
			void setYPos(unsigned int pos);

			/** Return the current xpos
			 *
			 * @return current xpos
			 * @see setXPos
			 */
			int getXPos() const;

			/** Return the current ypos
			 *
			 * @return current ypos
			 * @see setYPos
			 */
			int getYPos() const;


			/** Get current elevation id
			 */
			size_t getElevationNum() const;

			/** Get total number of elevations
			 */
			size_t getNumElevations() const;

			/** Set the current elevation id
			 */
			void setElevationNum(size_t);

			/** Get the current elevation
			 */
			ElevationPtr getCurrentElevation() const;

			void onMouseClick(unsigned char type, unsigned char button, unsigned short x, unsigned short y);

			void mouseEntered(IMouseEvent& evt) {}
			void mouseExited(IMouseEvent& evt) {}
			void mousePressed(IMouseEvent& evt);
			void mouseReleased(IMouseEvent& evt) {}
			void mouseClicked(IMouseEvent& evt) {}
			void mouseWheelMovedUp(IMouseEvent& evt) {}
			void mouseWheelMovedDown(IMouseEvent& evt) {}
			void mouseMoved(IMouseEvent& evt) {}
			void mouseDragged(IMouseEvent& evt) {}


			/** Set the overlay image for the hex grid selection
			 */
			void loadOverlayImage(const std::string& filename);

			/** Toggles coordinate info from upper left corner
			 */
			void toggleCoordInfoEnabled();

			/** Toggles a fadeout effect for mouse-selected objects
			 */
			void toggleFadeOutEffect();


			//////////////  NEW INTERFACE ///////////////////////

			size_t addVisual( Visual* visual );
			void removeVisual( size_t vidualId );
			Visual* getVisual(size_t id);

			bool isValidVisualId(size_t id) const;

			void notifyVisualChanged(size_t visualId);

			/** Clean up internal state
			 */
			void reset();

		private:
			/// The helper class for handling Visuals
			VisualTree* m_vtree;

			/// The screen surface
			Screen* m_surface;
			Rect m_rect;

			/// Current Map
			MapPtr m_map;
			/// Current elevation
			ElevationPtr m_elevation;

			/// Current elevation id
			size_t m_elevation_id;

			/// Current screen offset for rendering
			Point m_offset;

			/// Current position for the mouseclick overlay
			Point m_layer_pos;
			Point m_tilemask_pos;
			/// Images for grid overlay 
			size_t m_layer_id;
			size_t m_tilemask;
			
			/// Coordinate information shown on screen
			bool m_coordinfo;
			std::string m_tilecoordinfo;
			std::string m_objcoordinfo;

			/// Objects fadeout on mouse-selection
			bool m_fadeout;

			void renderTiles(LayerPtr layer);
			void renderGridOverlay(LayerPtr grid);

			void renderVisuals(size_t);

			void renderLayerOverlay(LayerPtr grid, const Point& pos);

			void elevationChange();
			void clearVisuals();

			/// Utility Function
			size_t getGridOverlayImageId(LayerPtr grid);

			/// Prevent copy construction
			View(const View&);
			View& operator=(const View&);

			friend class Camera;
	};

} } //FIFE::map

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
