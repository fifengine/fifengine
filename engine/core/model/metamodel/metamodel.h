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

namespace FIFE { namespace model {

	class Dataset;
	class Object;

	/**
	 * A meta model is just a collection of datasets. Perhaps this class
	 * will eventually have more responsibilities.
	 */
	class MetaModel {
		public:

			/** Destructor
			 *
			 */
			~MetaModel();

			/** Add a new dataset to the metamodel
			 *
			 * @note MetaModel owns this dataset, so don't delete
			 * the pointer it gives you!
			 */
			Dataset* addDataset();

			/** Gets a list of objects that with the given value
			 * in the given field.
			 */
			template<typename T>
			std::list<Object*> getObjects(const std::string& field, const T& value) const;

		private:

			std::vector<Dataset*> m_datasets;	
	};

}}; //FIFE::model
#endif
