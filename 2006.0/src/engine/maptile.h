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
#ifndef MAPTILE_H
#define MAPTILE_H

#include <cstddef>
#include <SDL_video.h>
#include "graphix_primitives.h"


namespace FIFE {

	class MapTile {
		public:
			MapTile();
			~MapTile();

			void setImageIDs(size_t floorid, size_t roofid); // uses the image with id from ImageCache
			void setFloorImageID(size_t floorid);
			void setRoofImageID(size_t floorid);
			void setRoofShift(int x, int y);
			
			size_t getFloorImageID() const;

			void renderFloor(SDL_Rect* targetrect, SDL_Surface* surface);
			inline void renderRoof(SDL_Rect* targetrect, SDL_Surface* surface) {
				renderRoof(targetrect, surface, 255);
			}
			void renderRoof(SDL_Rect* targetrect, SDL_Surface* surface, int alpha);

		private:
			Image* m_image_roof;
			Image* m_image_floor;
			size_t m_image_floor_id;

			int m_roofshift_x;
			int m_roofshift_y;

			MapTile(const MapTile&);
			MapTile& operator=(const MapTile&);
	};

}//FIFE

#endif
