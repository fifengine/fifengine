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

#ifndef FIFE_METAMODEL_H
#define FIFE_METAMODEL_H

// Standard C++ library includes
#include <vector>
#include <list>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fifeclass.h"

#include "dataset.h"

namespace FIFE {

	class Dataset;
	class Object;

	/**
	 * A meta model is just a collection of datasets. Perhaps this class
	 * will eventually have more responsibilities.
	 */
	class MetaModel: public FifeClass {
	public:
		/** Constructor
		 *
		 */
		MetaModel();
		
		/** Destructor
		 *
		 */
		~MetaModel();

		/** Add a new dataset to the metamodel
		 *
		 * @note MetaModel owns this dataset, so don't delete
		 * the pointer it gives you!
		 */
		Dataset* createDataset(const std::string& identifier);

		/** Gets the list of datasets in the metamodel. Recursive.
		 */
		std::list<Dataset*> getDatasets();

		/** Gets a list of datasets that with the given value
		 * in the given field.
		 */
		std::list<Dataset*> getDatasets(const std::string& field, const std::string& value);

		/** Gets a list of objects that with the given value
		 * in the given field.
		 */
		std::list<Object*> getObjects(const std::string& field, const std::string& value);
		
		/** Clears all datasets from metamodel
		 */
		void deleteDatasets();

	private:

		std::vector<Dataset*> m_datasets;	
	};

}; //FIFE
#endif
