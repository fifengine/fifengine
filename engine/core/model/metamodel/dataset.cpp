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
#include "model/metamodel/geometry_type.h"

#include "dataset.h"
#include "object.h"

namespace FIFE { namespace model {

	Dataset::Dataset(const std::string& type, const std::string& filename)
		: m_typename(type), m_filename(filename)
	{
	}

	Dataset::~Dataset() {
		purge(m_datasets);
	}

	void Dataset::addDataset(Dataset* dataset) {
		m_datasets.push_back(dataset);
	}

	Object* Dataset::getObject(const std::string& name) {

		std::vector<Object*>::iterator it = m_prototypes.begin();
		for(; it != m_prototypes.end(); ++it) {
			if((*it)->getName() == name)
				return *it;
		}

		Object* p = 0;
		std::vector<Dataset*>::iterator jt = m_datasets.begin();
		for(; jt != m_datasets.end(); ++jt) {
			p = (*jt)->getObject(name);
			if(p) {
				return p;
			}
		}

		return 0;
	}

	GeometryType* Dataset::getGeometryType(const std::string& name) {

		std::vector<GeometryType*>::iterator it = m_geometry_types.begin();
		for(; it != m_geometry_types.end(); ++it) {
			if((*it)->geometry == name)
				return *it;
		}

		GeometryType* p = 0;
		std::vector<Dataset*>::iterator jt = m_datasets.begin();
		for(; jt != m_datasets.end(); ++jt) {
			p = (*jt)->getGeometryType(name);
			if(p) {
				return p;
			}
		}

		return 0;
	}

}}
