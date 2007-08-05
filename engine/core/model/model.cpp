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
#include "util/purge.h"
#include "model/metamodel/metamodel.h"
#include "structures/map.h"

#include "model.h"

namespace FIFE { namespace model {

	Model::Model() {
		m_meta = new MetaModel();
	}

	Model::~Model() {
		purge(m_maps);
		delete m_meta;
	}

	Map* Model::addMap() {
		Map* map = new Map();
		m_maps.push_back(map);
		return map;
	}

	std::list<Map*> Model::getMaps() const {
		std::list<Map*> lst;

		std::vector<Map*>::const_iterator it = m_maps.begin();
		for(; it != m_maps.end(); ++it) {
			lst.push_back(*it);
		}

		return lst;
	}

	void Model::removeMap(Map* map) {
    std::vector<Map*>::iterator it = m_maps.begin();
		for(; it != m_maps.end(); ++it) {
			if(*it == map) {
				delete *it;
				m_maps.erase(it);
				return ;
			}
		}
	}

	size_t Model::getNumMaps() const {
		return m_maps.size();
	}

	void Model::clearMaps() {
		purge(m_maps);
		m_maps.clear();
	}

	MetaModel* Model::getMetaModel() {
		return m_meta;
	}
	
} } //FIFE::model

