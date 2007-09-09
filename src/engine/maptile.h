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

#ifndef MAPTILE_H
#define MAPTILE_H

#include <cstddef>
/*
namespace FIFE {
	class RenderAble;
	class Screen;
	class Rect;

	class MapTile {
		public:
			MapTile();
			~MapTile();

			void setImageIDs(size_t floorid, size_t roofid); // uses the image with id from ImageCache
			void setFloorImageID(size_t floorid);
			void setRoofImageID(size_t floorid);
			void setRoofShift(int x, int y);

			void renderFloor(const Rect& targetrect, Screen* surface);
			void renderRoof(const Rect& targetrect, Screen* surface);

			/// opacity for roof, to allow looking into building.
			unsigned char m_roofAlpha;

		private:
			size_t m_image_roof;
			size_t m_image_floor;

			int m_roofshift_x;
			int m_roofshift_y;

			MapTile(const MapTile&);
			MapTile& operator=(const MapTile&);
	};

}//FIFE*/

#endif
