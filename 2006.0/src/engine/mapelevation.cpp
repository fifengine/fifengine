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
#include "mapelevation.h"
#include "maptile.h"
#include "exception.h"
#include "object.h"

namespace FIFE {

	MapElevation::MapElevation(unsigned int width, unsigned int height) :
			m_width(width), m_height(height), m_tiles(new MapTile[m_width * m_height]), m_objects() {

	}


	MapElevation::~MapElevation() {
		cleanup();
	}

	void MapElevation::cleanup() {
		delete[] m_tiles;
		m_tiles = 0;

		type_objects::const_iterator end = m_objects.end();
		for (type_objects::iterator i = m_objects.begin(); i != end; ++i) {
			delete *i;
		}

		m_objects.clear();
	}

	MapElevation::MapElevation(const MapElevation&) : m_width(0), m_height(0), m_tiles(0), m_objects() {}

	MapElevation& MapElevation::operator=(const MapElevation&) {
		return *this;
	}

	int MapElevation::getWidth() const {
		return m_width;
	}

	int MapElevation::getHeight() const {
		return m_height;
	}

	void MapElevation::addObject(Object* obj) {
		if (obj)
			m_objects.push_back(obj);
	}
	void MapElevation::addObjectFront(Object* obj) {
		if (obj)
			m_objects.push_front(obj);
	}


	const MapElevation::type_objects& MapElevation::getObjects() const {
		return m_objects;
	}

}//FIFE
