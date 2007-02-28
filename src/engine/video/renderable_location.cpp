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
#include <boost/lexical_cast.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "renderable_location.h"

namespace FIFE {

	RenderableLocation::RenderableLocation( LocationTypes type, const std::string& loc )
		: m_frame_idx(0),
		m_direction_idx(0) {
		setType(type);
		setFilename(loc);
	};

	RenderableLocation::RenderableLocation( const std::string& loc )
		: m_frame_idx(0),
		m_direction_idx(0) {

		setType( RenderAble::RT_UNDEFINED );
		setFilename( loc );
	}

	RenderableLocation::RenderableLocation( const RenderableLocation& loc ) {
		setType( loc.m_type );
		setFilename( loc.m_string );

		m_frame_idx = loc.m_frame_idx;
		m_direction_idx = loc.m_direction_idx;
		m_extensions = loc.m_extensions;
	};

	RenderableLocation::~RenderableLocation() {
	}

	std::string RenderableLocation::getFileExtension() const {
		size_t pointPos = m_string.find_last_of(".");
		if( pointPos == std::string::npos )
			return m_string;
		return std::string(m_string, pointPos + 1);
	}

	std::string RenderableLocation::toString() const {
		std::string typeName;
		std::string extraInfo;

		switch(m_type) {
			case RenderAble::RT_UNDEFINED:
				typeName = "Undefined Renderable Type";

				extraInfo = " Frame:";
				extraInfo += boost::lexical_cast<std::string>(getFrame());
				extraInfo += " Direction:";
				extraInfo += boost::lexical_cast<std::string>(getDirection());
				break;

			case RenderAble::RT_IMAGE:
				typeName = "Static Image";

				extraInfo = " Frame:";
				extraInfo += boost::lexical_cast<std::string>(getFrame());
				extraInfo += " Direction:";
				extraInfo += boost::lexical_cast<std::string>(getDirection());
				break;

			case RenderAble::RT_ANIMATION:
				typeName = "Static Animation";

				extraInfo = " Direction:";
				extraInfo += boost::lexical_cast<std::string>(getDirection());
				break;

			case RenderAble::RT_COMPLEX_ANIMATION:
				typeName = "Complex Animation";
				break;

			default:
				typeName = "BUG? No known Renderable Type";
				break;
		}
		return typeName + "(" + getFilename() + ")" + extraInfo;
	};

	void RenderableLocation::setFilename(const std::string& filename) {
		m_string = filename;
	}

	const std::string& RenderableLocation::getExtension(ExtensionType type) const {
		if( !m_extensions ) {
			m_extensions =  boost::shared_ptr<type_extensions>(new type_extensions());
		}
		return (*m_extensions)[ type ];
	}

	bool RenderableLocation::hasExtension(ExtensionType type) const {
		if( !m_extensions ) {
			return false;
		}
		return m_extensions->find(type) != m_extensions->end();
	}

	bool RenderableLocation::isValid() const {
		if( m_type < RenderAble::RT_SUBIMAGE ) {
			return m_string != "";
		} else {
			return m_extensions;
		}
	}

	bool RenderableLocation::operator==(const RenderableLocation& loc) const {
		// different types => not equal
		if( m_type != loc.m_type )
			return false;

		// if we get here both have a valid m_string
		if( m_string.length() != loc.m_string.length() )
			return false;

		if(!std::equal(m_string.rbegin(), m_string.rend(), loc.m_string.rbegin()))
			return false;
		
		if (m_frame_idx != loc.m_frame_idx)
			return false;
		
		if (m_direction_idx != loc.m_direction_idx)
			return false;

		if( m_extensions && !loc.m_extensions )
			return false;
		if( !m_extensions && loc.m_extensions )
			return false;
		if( !m_extensions && !loc.m_extensions )
			return true;

		if( m_extensions->size() != loc.m_extensions->size() )
			return false;
		
		return std::equal(m_extensions->begin(),m_extensions->end(),
		                  loc.m_extensions->begin());
	}

	// It doesn't really matter what this function returns on which input,
	// as long as it's conststent. It is used by the location to cache map
	// in the ImageCache.
	bool RenderableLocation::operator<(const RenderableLocation& loc) const {
		
		// implicit order of different types
		if( int(m_type) < int(loc.m_type) )
			return true;

		if( m_type != loc.m_type )
			return false;

		if( m_frame_idx < loc.m_frame_idx )
			return true;

		if( m_frame_idx != loc.m_frame_idx )
			return false;

		if( m_direction_idx < loc.m_direction_idx )
			return true;

		if( m_direction_idx != loc.m_direction_idx )
			return false;

		if( m_string < loc.m_string )
			return true;

		if( m_string != loc.m_string )
			return false;

		if( !(m_extensions && loc.m_extensions) )
			return m_extensions < loc.m_extensions;

		if( m_extensions->size() < loc.m_extensions->size() )
			return true;

		if( m_extensions->size() > loc.m_extensions->size() )
			return false;

		type_extensions::iterator it1(m_extensions->begin());
		type_extensions::iterator it2(loc.m_extensions->begin());

		for(; it1 != m_extensions->end(); ++it1, ++it2){
			if( it1->first != it2->first )
				return( it1->first < it2->first );

			if( it1->second != it2->second )
				return( it1->second < it2->second );
		}
		return false;
	}
};//FIFE
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
