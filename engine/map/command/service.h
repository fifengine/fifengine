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
#ifndef FIFE_MAP_SERVICE_H
#define FIFE_MAP_SERVICE_H

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/structures/location.h"


namespace FIFE { namespace map {

	class View;
	class Map;
	typedef boost::shared_ptr<Map> MapPtr;

	class Service {
		public:
			Service(const std::string& name) : m_name(name),
				m_map(), m_view(0) {}

			virtual ~Service() {}

			void initialize(MapPtr map, View* view);

			const std::string& getName() const { return m_name; }

		private:
			std::string m_name;

		protected:
			MapPtr m_map;
			View* m_view;

			virtual void postInitialize();
	};
} } // FIFE::map 
#endif //FIFE_MAPSERVICE_H
