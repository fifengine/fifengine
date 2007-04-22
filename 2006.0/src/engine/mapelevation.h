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
#ifndef FIFE_MAPELEVATION_H
#define FIFE_MAPELEVATION_H

#include "maptile.h"
#include <list>

namespace FIFE {

	class Object;
	class MapElevation {
		public:
			typedef std::list<Object*> type_objects;

			MapElevation(unsigned int with, unsigned int height);
			~MapElevation();

			inline MapTile* getTile(unsigned int x, unsigned int y) const {
				return &(m_tiles[x + y*m_width]);
			}

			int getWidth() const;
			int getHeight() const;

			void addObject(Object* obj);
			// FIXME: This is a horrible hack to add animated floor tiles
			void addObjectFront(Object* obj);

			const type_objects& getObjects() const;

		private:
			int m_width;
			int m_height;

			MapTile* m_tiles;
			type_objects m_objects;

			void cleanup();

			MapElevation(const MapElevation&);
			MapElevation& operator=(const MapElevation&);
	};

}//FIFE

#endif
