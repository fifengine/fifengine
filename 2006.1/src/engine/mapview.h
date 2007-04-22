/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#ifndef MAPVIEW_H
#define MAPVIEW_H

#include "video/rect.h"
#include <cstddef>
#include <string>
#include "point.h"

namespace FIFE {

	class Map;
	class MapElevation;
	class Screen;
	class MapTileGrid;
	class MapGrid;

	class GridGeometry;

	/** Renders a map to a surface 
	 * @warning Currently positoning code is in motion. IF something seems odd, please still don't touch it!
	 */
	class MapView {
		public:

			/** Create a new empty MapView object.
			 * Use @c setMap() to initialize to show a map.
			 */
			MapView();

			/** Destructor
			 * Does not cleanup Map object!
			 */
			~MapView();

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

			/** Set the map to render
			 *
			 * @param map the map to render
			 * @param elevation the mapelevation to render
			 */
			void setMap(Map* map, size_t elevation);

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

			/** Render Roofs?
			 *
			 * @param render if true roofs are rendered
			 */
			void setRoofVisible(bool render);

			/** apply alpha to all roofs on current elevation(map?)
				@param alpha - alpha to add to all roofs. Reasonable range is [-255,255].
			*/
			void applyRoofAlpha(int alpha);

			/** set alpha to all roofs on current elevation(map?)
				@param alpha - alpha to set to all roofs. Reasonable range is [0,255].
			*/
			void setRoofAlpha(int alpha);

			/** Render Objects?
			 *
			 * @param render if true objects are rendered
			 */
			void setObjectsVisible(bool render);

			/** Render Floors?
			 *
			 * @param render if true floors are rendered
			 */
			void setFloorVisible(bool render);

			/** Are roofs rendered?
			 *
			 * @return true if roofs are renderd
			 */
			bool getRoofVisible() const;

			/** Render Object/Hex Grid?
			 *
			 * @param render if true the object grid is rendered
			 */
			void setObjectGridVisible(bool render);
			
			/** Render Object/Hex Grid?
			 *
			 * @return true if the object grid is rendered
			 */
			bool getObjectGridVisible() const;
			
			/** Render Tile Grid?
			 *
			 * @param render if true the tile grid is rendered
			 */
			void setTileGridVisible(bool render);
			
			/** Render Tile Grid?
			 *
			 * @return true if  the tile grid is rendered
			 */
			bool getTileGridVisible() const;
			
			/** Are objects rendered?
			 *
			 * @return true if objects are renderd
			 */
			bool getObjectsVisible() const;

			/** Are floors rendered?
			 *
			 * @return true if floors are renderd
			 */
			bool getFloorVisible() const;

			/** Get current elevation id
			 */
			size_t getElevationNum() const;

			/** Get total number of elevations
			 */
			size_t getElevationCount() const;

			/** Set the current elevation id
			 */
			void setElevationNum(size_t);

			/** Get the current elevation
			 */
			MapElevation* getCurrentElevation() const;

			/** Handle a mouseclick
			 */
			void onMouseClick(unsigned char type, unsigned char button, unsigned short x, unsigned short y);

			/** Set the overlay image for the hex grid selection
			 */
			void loadOverlayImage(const std::string& filename);

			/** Return the current offset in pixels for rendering
			 *  The offsets are set in @c updateOffsets()
			 */
			//Point getOffsetPos() const { return Point(m_xoff, m_yoff); } ;


			//////////////  NEW INTERFACE ///////////////////////


		private:
			/// The screen surface
			Screen* m_surface;
			Rect m_rect;

			/// The map structure
			Map* m_map;
			/// Current elevation
			MapElevation* m_elevation;

			/// Tile grid geometry
			GridGeometry* m_tile_geometry;
			/// Object grid geometry
			GridGeometry* m_obj_geometry;

			/// Current map position (tile grid)
			int m_xpos;
			int m_ypos;

			/// Range of tiles to display, set in setViewport
			int m_x_range;
			int m_y_range;

			/// Current elevation id
			size_t m_elevation_id;

			/// Flags for rendering
			bool m_render_floor;
			bool m_render_objects;
			bool m_render_roof;
			bool m_render_tile_grid;
			bool m_render_obj_grid;
			bool m_render_roof_transparent;

			/// Current alpha value for the roofs
			int m_roof_alpha;

			/// Current screen offset for rendering, set by @c updateOffsets
			Point m_offset;

			/// Current position for the mouseclick overlay
			Point m_grid_pos;
			Point m_tilemask_pos;
			/// Images for grid overlay 
			size_t m_grid_id;
			size_t m_tilemask;


			void renderTiles(MapTileGrid* grid);
			void renderGrid(MapGrid* grid);

			void renderObjects();

			void renderTileSelection();
			void renderObjectSelection();

			/// Prevent copy construction
			MapView(const MapView&);
			MapView& operator=(const MapView&);
	};

}//FIFE

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
