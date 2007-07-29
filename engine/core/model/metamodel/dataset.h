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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE { namespace model {

	class Object;
	class GeometryType;

	class Dataset {
		public:

			Dataset(const std::string& type, const std::string& filename);
			~Dataset();

			const std::string& getTypeName() const;
			const std::string& getFilename() const;

			/** Add a (nested) dataset
			 */
			void addDataset(Dataset* dataset);

			/** Get an object from this dataset. Objects are looked for in
			 * this dataset first, and if no match is found, the search is
			 * expanded to any datasets nested within this dataset. Null is
			 * returned if no match is found.
			 *
			 * @note Objects are owned by the dataset, so don't delete
			 * returned pointers!
			 */
			Object* getObject(const std::string& name);

			/** Get a geometry type from this dataset. GeometryTypes are looked for
			 * in this dataset first, and if no match is found, the search is expanded
			 * to any datasets nested within this dataset. Null is returned if no
			 * match is found.
			 *
			 * @note GeometryType objects are owned by the dataset, so don't delete
			 * returned pointers!
			 */
			GeometryType* getGeometryType(const std::string& name);

		private:

			// geometry definitions in this dataset
			std::vector<GeometryType*> m_geometry_types;

			// prototypes contained in this dataset
			std::vector<Object*> m_prototypes;

			// nested datasets
			std::vector<Dataset*> m_datasets;
		
			std::string m_typename;
			std::string m_filename;
	};

	inline
	const std::string& Dataset::getTypeName() const {
		return m_typename;
	}

	inline
	const std::string& Dataset::getFilename() const {
		return m_filename;
	}

}}

#endif
