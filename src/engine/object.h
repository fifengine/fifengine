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
	This represents an Object on the map. To get an idea what is an object
	in fife, toggle it's drawing by pressing 'o'. Currently only it's
	visual representation is attached to it. Later other stuff, like
	scripts, it's in game value, etc. will follow.
		-phoku
*/
#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <SDL_video.h>
#include "graphix_primitives.h"

namespace FIFE {

	class MapView;
	class Visual;

	class Object {
		public:
			static const int POS_NOT_ON_MAP = -1;

			Object(const std::string& name, int position = POS_NOT_ON_MAP);
			virtual ~Object();

			const std::string& getName() const;

			int getPosition() const;
			void setPosition(int pos);

			void addToInventory(Object* obj);

			void setVisual(Visual* visual);
			Visual* getVisual() const;
		private:
			std::string m_name;
			int m_position;
			Visual* m_visual;

			typedef std::vector<Object*> type_inventory;
			type_inventory m_inventory;

			void init();
			void cleanup();

			Object(const Object&);
			Object& operator=(const Object&);

			friend class MapView;
	};

}//FIFE

#endif

