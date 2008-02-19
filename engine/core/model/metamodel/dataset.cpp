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
#include "util/purge.h"
#include "model/metamodel/grids/cellgrid.h"
#include "util/exception.h"

#include "dataset.h"
#include "object.h"

namespace FIFE {

	Dataset::Dataset(const std::string& identifier)
		: AttributedClass(identifier)
	{
	}

	Dataset::~Dataset() {
		purge(m_objects);
	}

	void Dataset::addDataset(Dataset& dataset) {
		m_datasets.insert(&dataset);
	}

	Object* Dataset::createObject(const std::string& identifier, Object* inherited) {
		std::vector<Object*>::const_iterator it = m_objects.begin();
		for(; it != m_objects.end(); ++it) {
			if(identifier == (*it)->Id())
				throw NameClash(identifier);
		}

		Object* object = new Object(identifier, inherited);
		m_objects.push_back(object);
		return object;
	}

	std::list<Dataset*> Dataset::getDatasets() {
		std::list<Dataset*> datasets;

		std::set<Dataset*>::iterator it = m_datasets.begin();
		for(; it != m_datasets.end(); ++it) {
			datasets.push_back(*it);
		}

		return datasets;
	}

	std::list<Dataset*> Dataset::getDatasetsRec() {
		std::list<Dataset*> datasets;

		// Logic is buggy here (duplicates). Probably return values should be std::set too, but that's
		// beyond the scope of this changeset. --jwt
		std::set<Dataset*>::iterator it = m_datasets.begin();
		for(; it != m_datasets.end(); ++it) {
			std::list<Dataset*> tmp = (*it)->getDatasets();
			datasets.splice(datasets.end(), tmp);
			datasets.push_back(*it);
		}

		return datasets;
	}

	std::list<Dataset*> Dataset::getDatasets(const std::string& field, const std::string& value) {
		std::list<Dataset*> datasets;

		std::set<Dataset*>::iterator it = m_datasets.begin();
		for(; it != m_datasets.end(); ++it) {
			if((*it)->get(field) == value)
				datasets.push_back(*it);
		}

		std::set<Dataset*>::iterator jt = m_datasets.begin();
		for(; jt != m_datasets.end(); ++jt) {
			std::list<Dataset*> tmp = (*jt)->getDatasets(field, value);
			datasets.splice(datasets.end(), tmp);
		}

		return datasets;
	}

	std::list<Object*> Dataset::getObjects() {
		std::list<Object*> objects;

		std::vector<Object*>::iterator it = m_objects.begin();
		for(; it != m_objects.end(); ++it) {
				objects.push_back(*it);
		}

		std::set<Dataset*>::iterator jt = m_datasets.begin();
		for(; jt != m_datasets.end(); ++jt) {
			std::list<Object*> tmp = (*jt)->getObjects();
			objects.splice(objects.end(), tmp);
		}

		return objects;
	}

	std::list<Object*> Dataset::getObjects(const std::string& field, const std::string& value) {
		std::list<Object*> objects;

		std::vector<Object*>::const_iterator it = m_objects.begin();
		for(; it != m_objects.end(); ++it) {
			if((*it)->get(field) == value)
				objects.push_back(*it);
		}

		std::set<Dataset*>::iterator jt = m_datasets.begin();
		for(; jt != m_datasets.end(); ++jt) {
			std::list<Object*> tmp = (*jt)->getObjects(field, value);
			objects.splice(objects.end(), tmp);
		}

		return objects;
	}
}
