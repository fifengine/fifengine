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

#ifndef FIFE_XMLUtil_H
#define FIFE_XMLUtil_H
// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <boost/lexical_cast.hpp>
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "exception.h"
#include "xml/tinyxml/tinyxml.h"
#include "point.h"
#include "rect.h"

namespace FIFE {
	class AttributedClass;
}

namespace FIFE { namespace xmlutil {

	void loadMetadata(TiXmlElement* e, AttributedClass* t);
	void assertElement(TiXmlElement* e, const std::string& element);

	template<typename T>
	T queryElement(TiXmlElement* e) {
		if( !e )
			throw InvalidFormat("no xml element");

		const char* value = e->GetText();
		if( !value )
			throw InvalidFormat(std::string("empty xml element"));

		return boost::lexical_cast<T>(value);
	}

	template<typename T>
	T queryElement(TiXmlElement* e, const std::string& element) {
		if( !e )
			throw InvalidFormat("no xml element");

		e = e->FirstChildElement(element);
		if( !e )
			throw InvalidFormat(std::string("no xml element: ") + element);

		return queryElement<T>(e);
	}

	template<typename T>
	T queryElement(TiXmlElement* e, const std::string& element, const T& _default) {
		if( !e )
			throw InvalidFormat("no xml element");

		e = e->FirstChildElement(element);
		if( !e )
			return _default;

		return queryElement<T>(e);
	}

	template<>
	inline
	std::string queryElement(TiXmlElement* e) {
		if( !e )
			throw InvalidFormat("no xml element");

		const char* value = e->GetText();
		if( !value )
			throw InvalidFormat(std::string("empty xml element"));
		return std::string(value);
	}

	template<>
	inline
	Point queryElement(TiXmlElement* e) {
		return Point( queryElement<int16_t>(e,"x"), queryElement<int16_t>(e,"y") );
	}

	template<>
	inline
	Rect queryElement(TiXmlElement* e) {
		return Rect( queryElement<int16_t>(e,"x"), queryElement<int16_t>(e,"y"),
		             queryElement<int16_t>(e,"w"), queryElement<int16_t>(e,"h") );
	}

	template<>
	inline
	SDL_Color queryElement(TiXmlElement* e) {
		SDL_Color c;
		c.r = queryElement<int>(e,"r");
		c.g = queryElement<int>(e,"g");
		c.b = queryElement<int>(e,"b");
		return c;
	}
// 	template<>
// 	inline
// 	AttributedClass queryElement(TiXmlElement* e) {
// 		AttributedClass ac;
// 		loadMetadata(e,ac);
// 		return ac;
// 	}

	template<typename T>
	void foreachElement(TiXmlElement* e, const std::string& element, T& f) {
		if( !e )
			throw InvalidFormat("no xml element");

		e = e->FirstChildElement(element);
		while( e ) {
			f(e);
			e = e->NextSiblingElement(element);
		}
	}


} }
#endif
