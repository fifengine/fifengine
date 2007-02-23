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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "attributedclass.h"

namespace FIFE { namespace map {
	class ObjectInfo;

	class Archetype {
		public:
			Archetype(const std::string& type, const std::string& filename);
			virtual ~Archetype();

			/** Load a prototype of an object
			 *  Loading a prototype of an object will set the objects
			 *  data to default values, that can be overridden.
			 */
			void loadPrototype(ObjectInfo* object, size_t proto_id);

			/** Get ImageCache ID if tile with id id
			 */
			virtual size_t getTileImageId(size_t id) = 0;

			const std::string& getTypeName() const;
			const std::string& getFilename() const;

		protected:
			std::string m_typename;
			std::string m_filename;
	};

	inline
	const std::string& Archetype::getTypeName() const {
		return m_typename;
	}

	inline
	const std::string& Archetype::getFilename() const {
		return m_filename;
	}

}}

#endif
