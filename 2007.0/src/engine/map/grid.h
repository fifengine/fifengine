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

#ifndef FIFE_MAP_GRID_H
#define FIFE_MAP_GRID_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/point.h"
#include "attributedclass.h"

namespace FIFE { namespace map {

	class Geometry;
	/** A basic layer on a map elevation
	 *
	 *  This class represents a layer on the Map.
	 *  This can be for example a Tile layer 
	 *  as the roofs and floors of a fo2 map
	 *  but can also just contain "objects"
	 *  on grid coords.
	 * 
	 *  The tiles are *not* allways created only on
	 *  a first "setTileImage".
 	 *  
	 *  The most important features of this class are
	 *  "geometry", "shift" and "size":
	 *
	 *  The geometry is used to position objects on this
	 *  Grid and the Tiles too.
	 *
	 *  The shift is added to all screen coords and
	 *  will create the illusion of a height-difference :-)
	 *
	 *  The size simply is the maximum allowd size in grid
	 *  coords this grid covers.
	 *
	 *  Attributes:
	 *  <pre>
	 *	grid-overlay:	Image of the grid cell outline ("tile_outline.png")
	 * 	name:		Name of the grid ("floor")
	 *	do-render:	true, if the grid shall be rendered.
	 *  </pre>
	 *  
	 *  Future:
	 *  	Connections between grids to walk through (Elevators...)
	 *	Grouping of grids (These grids are roofs ... etc)
	 */
	class Grid : public AttributedClass {
		public:
			/** Constructs a MapGrid instance
			 */
			Grid(const Point& size, size_t geometry);

			/** Get the Geometry as set in the constructor
			 *  @return a valid Geometry
			 */
			Geometry* getGeometry();

			/** Get the size as set in the constructor
			 *  @return the size in grid coords
			 */
			const Point& getSize() const;

			/** Set the screen coord shift
			 *  @param shift The new shift in screen coords
			 */
			void setShift(const Point& shift);

			/** Get the screen coord shift
			 *  @return The shift in screen coords
			 */
			const Point& getShift() const;

			/** Get the tile image id of a position
			 *  @note If the position is invalid or the grid
			 *	is not yet created 0 is returned (a DummyImage id)
			 *  @param position A valid grid position
			 *  @return An image id for the position
			 */
			size_t getTileImage(const Point&) const;

			/** Get the tile image id of a position
			 *  @note If the position is invalid nothing is changed
			 *  @note If no tile have previously set, this
			 *  will allocate the needed memory.
			 *  @param position A valid grid position
			 *  @param image_id An image id for the position
			 */
			void setTileImage(const Point& position, size_t image_id);

			void setTilesVisible(bool vis);
			void toggleTilesVisible();
			bool areTilesVisible() const;

			void setObjectsVisible(bool vis);
			void toggleObjectsVisible();
			bool areObjectsVisible() const;

			uint8_t getGlobalAlpha() const;
			void setGlobalAlpha(uint8_t);
			void addToGlobalAlpha(int delta);

			bool isGridOverlayEnabled() const;
			void setGridOverlayEnabled(bool e);
			void toggleGridOverlayEnabled();

			/** Check whether a position is on the grid
			 *  @param positon A grid position to check
			 *  @return True, only if the position is on the grid
			 */
			bool isValidPosition(const Point& pos) const;

			const std::string& getOverlayImage() const;
			void setOverlayImage(const std::string& fname);

		protected:
			Point m_size;

			uint8_t m_global_alpha;

			bool m_tiles_visibility;
			bool m_objects_visibility;
			bool m_grid_overlay;

			std::vector<size_t> m_tiles;

			Point m_shift;
			Geometry* m_geometry;
			std::string m_overlay_image;
	};
} } // FIFE::map

#endif
