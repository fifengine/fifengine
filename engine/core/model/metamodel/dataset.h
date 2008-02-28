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
#include "util/base/attributedclass.h"
#include "model/metamodel/grids/cellgrid.h"

#include "object.h"


namespace FIFE {

	class Object;
	class CellGrid;

	class Dataset : public AttributedClass {
		public:

			/** Constructor
			 * Datasets are created by calling addDataset from metamodel, thus
			 * this method should really be called only by metamodel or test code
			 */
			Dataset(const std::string& identifier);

			/** Destructor
			 */
			~Dataset();

			/** Set this dataset's source xml filepath.
			 */
			void setSource(const std::string src) {
				source = src;
			}

			/** Get this dataset's source xml file (path). Null if no such file
			 * exists.
			 */
			const std::string& getSource() {
				return source;
			}

			/** Add a reference to a (nested) dataset
			 */
			void addDataset(Dataset& dataset);

			/** Add an object to this dataset; objects may optionally inherit
			 * values from other objects (see object.h for details).
			 *
			 * @note This object belongs to this dataset, so don't
			 * delete the returned pointer!
			 */
			Object* createObject(const std::string& identifier, Object* inherited = 0);

			/** Get the datasets referred to by this dataset.
			 */
			std::list<Dataset*> getDatasets();

			/** Get the datasets referred to by this dataset. Recursive.
			 */
			std::list<Dataset*> getDatasetsRec();

			/** Get datasets from this dataset having the given value in the given field.
			 */
			std::list<Dataset*> getDatasets(const std::string& field, const std::string& value);

			/** Get all the objects in this dataset
			 */
			std::list<Object*> getObjects();

			/** Get objects from this dataset having the given value in the
			 * given field.
			 *
			 * @note Objects are owned by the dataset, so don't delete
			 * returned pointers!
			 */
			std::list<Object*> getObjects(const std::string& field, const std::string& value);

		private:

			// prototypes contained in this dataset
			std::vector<Object*> m_objects;

			// nested datasets
			std::set<Dataset*> m_datasets;

			// source xml file
			std::string source;
	};

}

#endif
