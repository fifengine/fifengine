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

	/** A Class with dynamically typed attributes. These coorespond to
	 * metadata attributes in the native xml map format.
	 */
	class AttributedClass {
		public:
			typedef std::string type_attr_id;
			// Use boost::any instead?
			typedef boost::variant<bool,long,size_t,Point,Rect,std::string> type_attr;

		public:
			/** Create a new attributed class instance
			 *  Usually AttributedClass is inherited and the
			 *  facilities used by it's offspring (so to speak)
			 *
			 *  This constructor reflects this.
			 *
			 *  @param class_name The class name of the inheriting class
			 */
			AttributedClass(const type_attr_id& class_name = "Table");

			AttributedClass(const AttributedClass& ac);

			/** Destructor
			 *
			 */
			~AttributedClass();

			/** Set the value of an attribute
			 */
			template<typename T>
			void set(const type_attr_id& attr, const T& val) {
				m_attributes[ attr ] = type_attr(val);
			}

			/** Get the value of an attribute
			 *
			 *  Get a value for an attribute with an default value
			 *  If the attribute is not set already or in the case of
			 *  an type mismatch it is overwritten with the default value.
			 *
			 *  @param id Id of the attribute
			 *  @return The value of the attribute or a const ref
			 *  to a default value.
			 */
			template<typename T>
			T& get(const type_attr_id& attr, const T& def = T()) {
				if( m_attributes.find(attr) == m_attributes.end() ) {
// Uncomment this to see all default value creation of attributes.
//
// 					Debug("attributed_class")
// 						<< "creating attr for " << attr;

					m_attributes[ attr ] = type_attr(def);
				}

				T* val = boost::get<T>(&(m_attributes[ attr ]));
				if( val == 0 ) {
					Debug("attributed_class") 
						<< "type mismatch in "  << className() 
						<< " attr: " << attr;

					m_attributes[ attr ] = type_attr(def);
					val =	 boost::get<T>(&(m_attributes[ attr ]));
				}
				return *val;
			}

			/** Remove an attribute
			 */
			void del(const type_attr_id& attr) {
				m_attributes.erase( attr );
			}


			/** Get the value of an attribute
			 *
			 *  This is the const version of the above function,
			 *  thus does not change the objects state.
			 *  If an attribute is requested, that does not exist
			 *  or a type mismatch occures, a static default value
			 *  is returned, which is okay, since it is a const
			 *  reference.
			 *
			 *  @param id Id of the attribute
			 *  @return The value of the attribute or a const ref
			 *  to a default value.
			 */
			template<typename T>
			const T& get(const type_attr_id& id) const {
				static const type_attr const_attr;
				if( m_attributes.find(id) == m_attributes.end() ) {
					return boost::get<T>(const_attr);
				}

				T* val = boost::get<T>(&(m_attributes[ id ]));
				if( val == 0 ) {
					Debug("attributed_class")
						<< "type mismatch in " << className() 
						<< " attr: " << id;

					return boost::get<T>(const_attr);
				}
				return *val;
			}

			const std::type_info& getTypeInfo(const type_attr_id& id)  {
				return m_attributes[ id ].type();
			}

			/** Check whether an attribute exists
			 *  @param id Id of the attriute to check
			 *  @return True, if the attribute was set already
			 */
			bool hasAttribute(const type_attr_id& id) const;

			/** Get number of attributes
			 */
			size_t getNumAttributes() const { return 0; }

			/** Read attributes from another AttributedClass instance
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
			typedef std::map<type_attr_id,type_attr> type_attributes;
			type_attributes m_attributes;
			std::string m_className;
	};

	/** A anonymous table of values.
	 */
	typedef AttributedClass Table;

// Inline Functions

	inline
	bool AttributedClass::hasAttribute(const type_attr_id& attr) const {
		bool found = m_attributes.find(attr) != m_attributes.end();
		return found;
	}

	inline
	const std::string& AttributedClass::className() const {
		return m_className;
	}

}; //FIFE
#endif
