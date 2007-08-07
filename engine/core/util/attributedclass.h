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

// 3rd party library includes
#include <boost/variant.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/point.h"
#include "util/rect.h"

#include "util/debugutils.h"

namespace FIFE {

	/** A class with dynamically typed fields. These correspond to
	 * metadata attributes in the native xml map format.
	 */
	class AttributedClass {
		public:

			// Use boost::any instead?
			typedef boost::variant<bool,long,size_t,Point,Rect,std::string> value_type;

		public:
			/** Create a new attributed class instance
			 *  Usually AttributedClass is inherited and the
			 *  facilities used by it's offspring (so to speak)
			 *
			 *  This constructor reflects this.
			 *
			 *  @param class_name The class name of the inheriting class
			 */
			AttributedClass(const std::string& class_name = "Table");

			AttributedClass(const AttributedClass& ac);

			/** Destructor
			 *
			 */
			~AttributedClass();

			/** Set the value of a field.
			 */
			template<typename T>
			void set(const std::string& field, const T& value) {
				m_fields[field] = value_type(value);
			}

			/** Get the value of a field.
			 *
			 *  If an attribute is requested, that does not exist
			 *  or a type mismatch occures, a static default value
			 *  is returned.
			 *
			 *  @param field The field to be retrieved.
			 *  @return The value of the field or a const ref
			 *  to a default value.
			 */
			template<typename T>
			const T& get(const std::string& field) {
				static const value_type const_value;
				if(m_fields.find(field) == m_fields.end()) {
					return boost::get<T>(const_value);
				}

				T* value = boost::get<T>(&(m_fields[field]));
				if(value == 0) {
					Debug("attributed_class")
						<< "type mismatch in " << className() 
						<< " field: " << field;

					return boost::get<T>(const_value);
				}
				return *value;
			}

			/** Remove a field.
			 */
			void remove(const std::string& field) {
				m_fields.erase(field);
			}

			const std::type_info& getTypeInfo(const std::string& field)  {
				return m_fields[field].type();
			}

			/** Check whether a field exists
			 *  @param field The field to check
			 *  @return True if the field has been set
			 */
			bool hasField(const std::string& field) const;

			/** Read fields from another AttributedClass instance (DEPRECATED?)
			 */
			void updateAttributes(const AttributedClass* attrObject, bool override = true);

			/** Get class name of the inheriting class
			 * Poor mans rtti
			 */
			const std::string& className() const;

			/** Print debuging information
			 */
			void debugPrint() const {}

		private:
			std::map<std::string,value_type> m_fields;

			std::string m_className;
	};

	/** A anonymous table of values.
	 */
	typedef AttributedClass Table;

// Inline Functions

	inline
	bool AttributedClass::hasField(const std::string& field) const {
		bool found = m_fields.find(field) != m_fields.end();
		return found;
	}

	inline
	const std::string& AttributedClass::className() const {
		return m_className;
	}

}; //FIFE
#endif
