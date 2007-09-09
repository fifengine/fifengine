/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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
	};

	RenderableLocation::~RenderableLocation() { };

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
		m_isValid = filename != "";
		m_string = filename;
	}

	bool RenderableLocation::isValid() const {
		return m_isValid;
	};

	bool RenderableLocation::operator==(const RenderableLocation& loc) const {
		// different types => not equal
		if( m_type != loc.m_type )
			return false;

		// catch undefined types
		if (m_type == 0)
			return true; // is this correct? does it matter?

		// if we get here both have a valid m_string
		if( m_string.length() != loc.m_string.length() )
			return false;

		if(!std::equal(m_string.rbegin(), m_string.rend(), loc.m_string.rbegin()))
			return false;
		
		if (m_frame_idx != loc.m_frame_idx)
			return false;
		
		return(m_direction_idx == loc.m_direction_idx);
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

		if( !isValid() )
			return false; // catch for special case; is this correct?
		
		// assume m_type > 0

		if( m_frame_idx < loc.m_frame_idx )
			return true;

		if( m_frame_idx != loc.m_frame_idx )
			return false;

		if( m_direction_idx < loc.m_direction_idx )
			return true;

		if( m_direction_idx != loc.m_direction_idx )
			return false;

		return m_string < loc.m_string;
	}
};//FIFE
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
