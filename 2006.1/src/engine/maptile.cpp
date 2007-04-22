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

// #include "maptile.h"
// #include "imagecache.h"
// #include "log.h"
// #include "video/renderable.h"
// 
// namespace FIFE {
// 
// 	MapTile::MapTile() : m_roofAlpha(255), m_image_roof(0), m_image_floor(0), m_roofshift_x(0), m_roofshift_y(0)
// 		{}
// 
// 	MapTile::~MapTile() {}
// 
// 	MapTile::MapTile(const MapTile&) : m_roofAlpha(255), m_image_roof(0), m_image_floor(0), m_roofshift_x(0), m_roofshift_y(0) {}
// 
// 	MapTile& MapTile::operator=(const MapTile&) {
// 		return *this;
// 	}
// 
// 	void MapTile::setImageIDs(size_t floorid, size_t roofid) {
// 		setFloorImageID(floorid);
// 		setRoofImageID(roofid);
// 	}
// 
// 	void MapTile::setFloorImageID(size_t id) {
// 		m_image_floor = id;
// 	}
// 
// 	void MapTile::setRoofImageID(size_t id) {
// 		m_image_roof = id;
// 	}
// 
// 	void MapTile::setRoofShift(int x, int y) {
// 		m_roofshift_x = x;
// 		m_roofshift_y = y;
// 	}
// 
// 	void MapTile::renderFloor(const Rect& targetrect, Screen* surface) {
// 		if (m_image_floor) {
// 			ImageCache::instance()->getImage(m_image_floor)->render(targetrect, surface);
// 		}
// 	}
// 
// 	void MapTile::renderRoof(const Rect& targetrect, Screen* surface) {
// 		if (m_image_roof) {
// 			ImageCache::instance()->getImage(m_image_roof)->render(targetrect, surface, m_roofAlpha);
// 		}
// 	}
// 
// }//FIFE
