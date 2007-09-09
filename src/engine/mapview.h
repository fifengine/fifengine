/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
/*
	This is a view on a map. On a map elevation to be precise.
	This currently only steers the drawing order of tiles, roof tiles
	and objects.
	It also keeps track of the various flags, that may be set.
		-phoku
*/
#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <SDL_video.h>

namespace FIFE {

	class Map;
	class MapElevation;

	/** Renders a map to a surface */
	class MapView {
		public:
			MapView();
			~MapView();

			/**   Set the surface where the map should be rendered
			 *
			 * @param surface the surface to render at
			 * @param rect mapview will render into this rect on surface
			 */
			void setViewport(SDL_Surface* surface, const SDL_Rect& rect);

			/** Set the surface where the map should be rendered
			 *
			 * @param the surface render at
			 */
			void setViewport(SDL_Surface* surface);

			/** Set the map to render
			 *
			 * @param map the map to render
			 * @param elevation the mapelevation to render
			 */
			void setMap(Map* map, size_t elevation);

			void handleFPSCounter();

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

			/** Render transparent Roofs?
			 *
			 * @param render if true roofs are rendered transparent
			 */
			void setRoofTransparent(bool render);

			/** Which alpha is used to paint transparent roofs?
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

			/** Render transparent Roofs?
			 *
			 * @param render if true roofs are rendered transparent
			 */
			bool getRoofTransparent() const;

			/** Which alpha is used to paint transparent roofs?
			*/
			int getRoofAlpha() const;

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

			size_t getElevationNum() const;
			size_t getElevationCount() const;
			void setElevationNum(size_t);
			MapElevation* getCurrentElevation() const;

		private:
			SDL_Surface* m_surface;
			SDL_Rect m_rect;

			Map* m_map;
			MapElevation* m_elevation;

			int m_xpos;
			int m_ypos;

			int m_x_range;
			int m_y_range;

			size_t m_elevation_id;

			bool m_render_floor;
			bool m_render_objects;
			bool m_render_roof;
			bool m_render_roof_transparent;

			int m_roof_alpha;

			void renderFloor();
			void renderRoof();
			void renderObjects();

			MapView(const MapView&);
			MapView& operator=(const MapView&);
	};

}//FIFE

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
