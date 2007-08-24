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

#ifndef FIFE_MAP_ARCHETYPE_H
#define FIFE_MAP_ARCHETYPE_H

// Standard C++ library includes
#include <string>
#include <vector>
#include <list>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/attributedclass.h"
#include "model/metamodel/grids/cellgrid.h"

#include "object.h"


namespace FIFE { namespace model {

	class Object;
	class CellGrid;

	class Dataset : public AttributedClass {
		public:

			/** Constructor
			 * Datasets are created by calling addDataset from metamodel, thus
			 * this method should really be called only by metamodel or test code
			 */
			Dataset();

			/** Destructor
			 */
			~Dataset();

			/** Add a (nested) dataset
			 *
			 * @note This Dataset owns any datasets you add to it, so
			 * don't delete the returned pointer!
			 */
			Dataset* addDataset();

			/** Add an object to this dataset; objects may optionally inherit
			 * values from other objects (see object.h for details).
			 *
			 * @note This object belongs to this dataset, so don't
			 * delete the returned pointer!
			 */
			Object* addObject(Object* inherited = 0);

			/** Get datasets from this dataset having the given value in the
			 * given field.
			 *
			 * @note These datasets are owned by this dataset, so don't delete
			 * returned pointers!
			 */
			template<typename T>
			std::list<Dataset*> getDatasets(const std::string& field, const T& value) {
				std::list<Dataset*> datasets;

				std::vector<Dataset*>::const_iterator it = m_datasets.begin();
				for(; it != m_datasets.end(); ++it) {
					if((*it)->get<T>(field) == value)
						datasets.push_back(*it);
				}

				std::vector<Dataset*>::const_iterator jt = m_datasets.begin();
				for(; jt != m_datasets.end(); ++jt) {
					std::list<Dataset*> tmp = (*jt)->getDatasets<T>(field, value);
					datasets.splice(datasets.end(), tmp);
				}

				return datasets;
			}

			/** Get objects from this dataset having the given value in the
			 * given field.
			 *
			 * @note Objects are owned by the dataset, so don't delete
			 * returned pointers!
			 */
			template<typename T>
			std::list<Object*> getObjects(const std::string& field, const T& value) {
				std::list<Object*> objects;

				std::vector<Object*>::const_iterator it = m_objects.begin();
				for(; it != m_objects.end(); ++it) {
					if((*it)->oget<T>(field) == value)
						objects.push_back(*it);
				}

				std::vector<Dataset*>::const_iterator jt = m_datasets.begin();
				for(; jt != m_datasets.end(); ++jt) {
					std::list<Object*> tmp = (*jt)->getObjects<T>(field, value);
					objects.splice(objects.end(), tmp);
				}

				return objects;
			}

		private:

			// prototypes contained in this dataset
			std::vector<Object*> m_objects;

			// nested datasets
			std::vector<Dataset*> m_datasets;
	};

}}

#endif
