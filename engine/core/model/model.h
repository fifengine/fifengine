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

#ifndef FIFE_MODEL_H
#define FIFE_MODEL_H

// Standard C++ library includes
#include <list>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/fifeclass.h"

#include "model/structures/map.h"

namespace FIFE {

	class MetaModel;

	/**
	 * A model is a facade for everything in the model.
	 */
	class Model: public FifeClass {
	public:

		/** Constructor
		 *
		 */
		Model();

		/** Destructor
		 *
		 */
		~Model();
		
		/** Add a map this model, and get a pointer to it.
		 * The returned pointer is owned by the Model, so
		 * don't delete it!
		 */
		Map* createMap(const std::string& identifier);

		/** Remove a map from this model
		 */
		void deleteMap(Map*);

		/** Get all the maps in the model.
		 */
		std::list<Map*> getMaps() const;

		/** Get a set of maps by a value.
		 *
		 * @param the field to search on
		 * @param the value to be searched for in the field
		 */
		std::list<Map*> getMaps(const std::string& field, const std::string& value) const;

		/** Return the number of maps in this model
		 */
		size_t getNumMaps() const;

		/** Remove all elevations from a map
		 */
		void deleteMaps();

		/** Get a pointer to the MetaModel associated with
		 * this Model. The Model owns this pointer, so don't
		 * delete it!
		 */
		MetaModel* getMetaModel();

		/** Adds pather to model. Moves ownership to model
		 */
		void adoptPather(AbstractPather* pather);
		
		/** Returns pather corresponding given name. If none found, returns NULL
		 */
		AbstractPather* getPather(const std::string& pathername);
		
		/** Called periodically to update events on model
		 */
		void update();

	private:

		std::vector<Map*> m_maps;
		std::vector<AbstractPather*> m_pathers;
		MetaModel* m_meta;
	};

}; //FIFE
#endif
