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

#ifndef FIFE_ATTRIBUTEDCLASS_H
#define FIFE_ATTRIBUTEDCLASS_H

// Standard C++ library includes
#include <map>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "logger.h"
#include "fifeclass.h"

namespace FIFE {

	/** Base for classes with metadata attributes.
	 */
	class AttributedClass: public FifeClass {

		public:
			/** Create a new attributed class instance
			 *  Usually AttributedClass is inherited and the
			 *  facilities used by it's offspring (so to speak)
			 *
			 *  This constructor reflects this.
			 *
			 *  @param identifier A string identifier for this object
			 */
			AttributedClass(const std::string& identifier);

			/** Destructor
			 *
			 */
			virtual ~AttributedClass();

			/** Get the (string) identifier associated with this object
			 */
			const std::string& Id() const;

			/** List the fields contained in this archetype
			 */
			virtual std::vector<std::string> listFields() const;

			/** Set the value of a field.
			 */
			virtual void set(const std::string& field, const std::string& value);

			/** Get the value of a field.
			 *
			 *  If an attribute is requested, that does not exist
			 *  the empty string is returned.
			 *
			 *  @param field The field to be retrieved.
			 */
			virtual const std::string& get(const std::string& field);

			/** Remove a field.
			 */
			void remove(const std::string& field);

		private:
			std::map<std::string,std::string> m_fields;

			std::string m_id;
	};

}; //FIFE
#endif
