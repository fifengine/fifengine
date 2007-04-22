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

#include "renderable_location.h"

namespace FIFE {

	RenderableLocation::RenderableLocation( enum LocationTypes type, const std::string& loc ) {
		m_type = type;
		m_string = loc;
	};

	RenderableLocation::RenderableLocation( const std::string& loc ) :
		m_type(RL_FILE), m_string(loc), m_frame_idx(0), m_direction_idx(0) {}

	RenderableLocation::RenderableLocation( const std::string& loc, unsigned int frame_idx) :
		m_type(RL_FILE_FRAME), m_string(loc), m_frame_idx(frame_idx), m_direction_idx(0) {}

	RenderableLocation::RenderableLocation( const std::string& loc, 
			unsigned int frame_idx, unsigned int dir_idx ) :
		m_type(RL_FILE_FRAME_DIR), m_string(loc), m_frame_idx(frame_idx), m_direction_idx(dir_idx) {}


	RenderableLocation::RenderableLocation( const RenderableLocation& loc ) {
		m_type = loc.m_type;
		m_string = loc.m_string;
		m_frame_idx = loc.m_frame_idx;
		m_direction_idx = loc.m_direction_idx;
	};

	RenderableLocation::~RenderableLocation() { };

	std::string RenderableLocation::getFileExtension() const {
		if( m_string.length() <= 3 )
			return m_string;
		return std::string(m_string, m_string.length() - 3, 3 );
	};

	std::string RenderableLocation::toString() const {
		switch(m_type) {
			case RL_UNDEFINED:
				break;
			case RL_FILE:
				return std::string("File: ") + toFilename();
		    default:
				break;
			case RL_FILE_FRAME:
				break;
			case RL_FILE_FRAME_DIR:
				break;
		};
		return "Undefined Location.";
	};

	RenderableLocation::operator bool() const {
		return m_type != RL_UNDEFINED;
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

		if (m_type == RL_FILE)
			return(std::equal(m_string.rbegin(), m_string.rend(), loc.m_string.rbegin()));

		// m_type > RL_FILE
		if(! std::equal(m_string.rbegin(), m_string.rend(), loc.m_string.rbegin()))
			return false;
		
		if (m_type == RL_FILE_FRAME)
			return (m_frame_idx == loc.m_frame_idx);

		// m_type > RL_FILE_FRAME
		if (m_frame_idx != loc.m_frame_idx)
			return false;
		
		if (m_type == RL_FILE_FRAME_DIR)
			return(m_direction_idx == loc.m_direction_idx);
	
		// repeating last check without condition to suppress compiler warning
		return(m_direction_idx == loc.m_direction_idx);
	};

	// It doesn't really matter what this function returns on which input,
	// as long as it's conststent. It is used by the location to cache map
	// in the ImageCache.
	bool RenderableLocation::operator<(const RenderableLocation& loc) const {
		
		// implicit order of different types
		if( int(m_type) < int(loc.m_type) )
			return true;

		if (m_type == 0)
			return false; // catch for special case; is this correct?
		
		// assume m_type > 0

		int k = std::lexicographical_compare(
				m_string.rbegin(), m_string.rend(), loc.m_string.rbegin(), loc.m_string.rend()
				) ? -1 :
			 std::lexicographical_compare(loc.m_string.rbegin(), loc.m_string.rend(),
					 m_string.rbegin(), m_string.rend()
				) ? 1 : 0;
		
		// catch RL_FILE types
		if (m_type == RL_FILE)
			if (k == -1)
				return true;
			else
				return false;

		if (m_type > RL_FILE)
			if (k == -1)
				return true;
			else if (k == 1)
				return false;

		// if we get here: m_type > RL_FILE && equal filenames
		if (m_type == RL_FILE_FRAME)
			return (m_frame_idx < loc.m_frame_idx);

		if (m_type > RL_FILE_FRAME)
			if (m_frame_idx < loc.m_frame_idx)
				return true;
			else if (m_frame_idx > loc.m_frame_idx)
				return false;

		return (m_direction_idx >= loc.m_direction_idx);
	};
};//FIFE
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
