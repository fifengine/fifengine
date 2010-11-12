/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_RESOURCE_LOCATION_H
#define FIFE_RESOURCE_LOCATION_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	enum ResourceLocationType {
		RES_TYPE_FILE = 0,
		RES_TYPE_IMAGE = 1
	};

	/** Contains information about the Location of a Resource
	 *
	 *  This class is used to give ResoureProvider the information
	 *  where to find the data.
	 *
	 *  WARNING: It is \b very important that the comparison operators work correctly,
	 *  otherwise the pools will silently consume more and more memory. So before you change
	 *  something there, think about the implications. Please.
	 */
	class ResourceLocation {
	public:

		// LIFECYCLE
		/** Default constructor.
		 */
		ResourceLocation(const std::string& filename): m_filename(filename),m_type(RES_TYPE_FILE) {}

		/** Destructor.
		 */
		virtual ~ResourceLocation() {};

		/** Returns the filename.
		 * @return The filename.
		 */
		const std::string& getFilename() const { return m_filename; };

		/** Compares two ResourceLocations for equality.
		 */
		virtual bool operator ==(const ResourceLocation& loc) const {
			if( m_type != loc.m_type ) {
				return false;
			}

			if (m_filename.length() != loc.m_filename.length()) {
				return false;
			}
			if (!std::equal(m_filename.rbegin(), m_filename.rend(), loc.m_filename.rbegin())) {
				return false;
			}
			return true;
		}

		/** Compares two ResourceLocations
		 *  This is needed as the locations should be stored in a \c std::map
		 */
		virtual bool operator <(const ResourceLocation& loc) const {
			if( m_type < loc.m_type )
				return true;
			if( m_type > loc.m_type )
				return false;
			return m_filename < loc.m_filename;
		}

		/** Creates copy of this location
		 *  All derived classes must implement this and return their corresponding instance
		 */
		virtual ResourceLocation* clone() const {
			return new ResourceLocation(m_filename);
		}

		ResourceLocationType getType() const { return m_type; }

	protected:
		std::string m_filename;
		ResourceLocationType m_type;
	};
} //FIFE

#endif
