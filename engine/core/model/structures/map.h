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

#ifndef FIFE_MAP_MAP_H
#define FIFE_MAP_MAP_H

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/attributedclass.h"

#include "location.h"

namespace FIFE { namespace model {

	class Elevation;

	class Dataset;

	/** A container of \c Elevation(s).
	 *
	 * The actual data is contained in \c Elevation objects; only one
	 * \c Elevation can be displayed at any time.
	 *
	 * Fallout legacy: A map (file or this object) contains a number of
	 * levels in one file (to switch between some level w/o loading).
	 * This has nothing to do with height-levels.
	 *
	 * @see MapElevation
	 * @see MapView
	 * @see MapLoader
	 */
	class Map : public AttributedClass {
		public:

			/** Construct a map
			 */
			Map();

			/** Destructor
			 */
			~Map();

			/** Use a Dataset
			 */
			void useDataset(Dataset* dataset);

			/** Add an elevation to this map, and get a pointer
			 * to it; the returned pointer is owned by the Map
			 * so don't delete it!
			 */
			Elevation* addElevation();
			
			/** Remove an elevation from this map
			 */
			void removeElevation(Elevation*);

      /** Get a set of elevations by a value.
			 *
			 * @param the field to search on
			 * @param the value to be searched for in the field
			 */
			template<typename T>
			std::list<Elevation*> getElevations(const std::string& field, const T& value) const;

			/** Return the number of elevations on this map
			 */
			size_t getNumElevations() const;

			/** Remove all elevations from a map
			 */
			void clearElevations();

		private:

			std::string m_mapname;

			std::vector<Dataset*> m_datasets;

			std::vector<Elevation*> m_elevations;

			Map(const Map& map);
			Map& operator=(const Map& map);

			friend class MapLoader;
	};

} } //FIFE::model

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
