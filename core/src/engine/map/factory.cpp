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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#ifdef HAVE_XMLMAP
#include "loaders/xml/xml.h"
#endif
#include "loaders/fallout/fallout.h"
#include "debugutils.h"
#include "exception.h"

#include "factory.h"
#include "geometry.h"
#include "loader.h"

namespace FIFE { namespace map {

	Factory::Factory() : m_loaders() {
		registerLoader(new loaders::fallout::Fallout());
#ifdef HAVE_XMLMAP
		registerLoader(new loaders::xml::XML());
#endif
		Geometry::registerGeometry(s_geometry_info(
			Geometry::FalloutTileGeometry,
			"RECTANGULAR",
			Point(80,36),  // TILE SIZE
			Point(48,24),  // TRANSFORM
			Point(),       // OFFSET
			0));           // FLAGS: NONE
		Geometry::registerGeometry(s_geometry_info(
			Geometry::FalloutObjectGeometry,
			"HEXAGONAL",
			Point(32,16),  // TILESIZE
			Point(16,12),  // TRANSFORM
			Point(32,10),  // OFFSET
			Geometry::ShiftXAxis));           // FLAGS: SHIFT AROUND X AXIS
	}

	// FIXME: use boost::ptr_map ?
	Factory::~Factory() {
		cleanup();
	}

	void Factory::cleanup() {
		type_loaders::const_iterator end = m_loaders.end();
		for (type_loaders::iterator i = m_loaders.begin(); i != end; ++i) {
			delete i->second;
		}

		m_loaders.clear();
	}

	void Factory::registerLoader(Loader* loader) {
		m_loaders.insert(std::make_pair(loader->getName(), loader));
		Log("maploader") << "new maploader: " << loader->getName();
	}

	Map* Factory::loadMap(const std::string& path) {
		type_loaders::const_iterator end = m_loaders.end();
		for (type_loaders::iterator i = m_loaders.begin(); i != end; ++i) {
			try {
				Loader* loader = i->second;
				Debug("maploader") << "trying to load " << path;
				return loader->loadFile(path);
			} catch (const Exception& ex) {
				Log("maploader") << ex.getMessage();
				continue;
			}
		}

		Log("maploader") << "no loader succeeded for " << path << " :(";
		return 0;
	}

} } //FIFE::map
