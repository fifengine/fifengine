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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "attributedclass.h"
#include "exception.h"

#include "xmlutil.h"

namespace FIFE { namespace xmlutil {

	struct ac_setter {
		AttributedClass* t;

		ac_setter(AttributedClass* _t) : t(_t) {}

		void operator()(TiXmlElement* e) {
			const char* cname = e->Attribute("name");
			const char* ctype = e->Attribute("type");
			if( !cname )
				throw InvalidFormat(std::string("no 'name' attribute"));

			if( !ctype )
				throw InvalidFormat(std::string("no 'type' attribute"));

			std::string name(cname);
			std::string type(ctype);

			if( type == "text" ) {
				t->set<std::string>(name, queryElement<std::string>(e) );
				return;
			}
			if( type == "int" ) {
				t->set<long>(name, queryElement<int>(e) );
				return;
			}

			#define AC_SET_AND_RETURN(type_name)                                         \
				if( type == # type_name ) {                                            \
					t->set< type_name >(name, queryElement< type_name >(e) );        \
					return;                                                          \
				}

			AC_SET_AND_RETURN( bool );
			AC_SET_AND_RETURN( long );
			AC_SET_AND_RETURN( size_t );
			AC_SET_AND_RETURN( Point );
			AC_SET_AND_RETURN( Rect );


			#undef AC_SET_AND_RETURN
		}
	};


	void loadMetadata(TiXmlElement* e, AttributedClass* t) {
		if( !e )
			return;

		ac_setter setter(t);
		foreachElement(e, "param",setter);
	}


	void assertElement(TiXmlElement* e, const std::string& element) {
		if( !e )
			throw InvalidFormat(std::string("no '") + element + "' element");

		if( element != e->Value() )
			throw InvalidFormat(std::string("no '") + element + "' element: " + e->Value());
	}

}}
