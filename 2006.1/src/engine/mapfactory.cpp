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

#include "mapfactory.h"
#include "maploader.h"
#include "debugutils.h"
#include "exception.h"

#include "maploaders/fallout/mffallout.h"
#ifdef HAVE_XMLMAP
#include "maploaders/xmlmap/xmlmap.h"
#endif

namespace FIFE {

	MapFactory::MapFactory() : m_loaders() {
		registerLoader(new MFFallout());
#ifdef HAVE_XMLMAP
		registerLoader(new XMLMap());
#endif
	}


	MapFactory::~MapFactory() {
		cleanup();
	}

	void MapFactory::cleanup() {
		type_loaders::const_iterator end = m_loaders.end();
		for (type_loaders::iterator i = m_loaders.begin(); i != end; ++i) {
			delete i->second;
		}

		m_loaders.clear();
	}

	void MapFactory::registerLoader(MapLoader* loader) {
		m_loaders.insert(std::make_pair(loader->getName(), loader));
		Log("maploader") << "new maploader: " << loader->getName();
	}

	void MapFactory::unregisterLoader(MapLoader* loader) {
		type_loaders::iterator i = m_loaders.find(loader->getName());
		if (i == m_loaders.end()) {
			return;
		}

		m_loaders.erase(i);
	}

	Map* MapFactory::loadFile(const std::string& path) {
		type_loaders::const_iterator end = m_loaders.end();
		for (type_loaders::iterator i = m_loaders.begin(); i != end; ++i) {
			try {
				MapLoader* loader = i->second;
				return loader->loadFile(path);
			} catch(const Exception& ex) {
				Log("maploader") << ex.getMessage();
				continue;
			}
		}

		Log("maploader") << "no loader suceeded for " << path << " :(";
		return 0;
	}

}//FIFE
