/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include "util/structures/purge.h"
#include "util/base/exception.h"

#include "metamodel.h"
#include "dataset.h"

namespace FIFE {
	
	MetaModel::MetaModel(): FifeClass() {
	}
	
	MetaModel::~MetaModel() {
		deleteDatasets();
	}

	void MetaModel::deleteDatasets() {
		purge(m_datasets);
		m_datasets.clear();
	}

	Dataset* MetaModel::createDataset(const std::string& identifier) {
		std::vector<Dataset*>::const_iterator it = m_datasets.begin();
		for(; it != m_datasets.end(); ++it) {
			if(identifier == (*it)->Id())
				throw NameClash(identifier);
		}

		Dataset* dataset = new Dataset(identifier);
		m_datasets.push_back(dataset);
		return dataset;
	}

	std::list<Dataset*> MetaModel::getDatasets() {
		std::list<Dataset*> datasets;

		std::vector<Dataset*>::const_iterator it = m_datasets.begin();
		for(; it != m_datasets.end(); ++it) {
			datasets.push_back(*it);
		}

		return datasets;
	}

	std::list<Dataset*> MetaModel::getDatasets(const std::string& field, const std::string& value) {
		std::list<Dataset*> datasets;

		std::vector<Dataset*>::const_iterator it = m_datasets.begin();
		for(; it != m_datasets.end(); ++it) {
			if((*it)->get(field) == value)
				datasets.push_back(*it);
		}

		return datasets;
	}

	std::list<Object*> MetaModel::getObjects(const std::string& field, const std::string& value) {
		std::list<Object*> objects;

		std::vector<Dataset*>::const_iterator it = m_datasets.begin();
		for(; it != m_datasets.end(); ++it) {
			std::list<Object*> tmp = (*it)->getObjects(field, value);
			objects.splice(objects.end(), tmp);
		}

		return objects;
	}

}; //FIFE

