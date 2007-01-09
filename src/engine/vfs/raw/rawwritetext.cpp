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
#include <iostream>

// 3rd party library includes

// FIFE includes
#include "rawwritetext.h"

namespace FIFE {

	RawWriteText::RawWriteText(std::string& path) : m_file(path.c_str(), std::ios::trunc | std::ios::out){
		// If we couldn't open a file, this object has no purpose.
		// Delete it so it is invalid.  This is done at instantiation time so that 
		// no object will exist and the caller can handle the problem 
		// appropriately.
		if( !m_file.is_open() ){
			delete this;
		}
	}
	
	RawWriteText::~RawWriteText(){
		m_file.close();
	}

	void RawWriteText::writeLine(std::string& buffer){
		size_t eol_index;
		eol_index = buffer.find('\n', 0);
		write(buffer, 0, eol_index);
		m_file << std::endl;
	}
	
	void RawWriteText::write(std::string& buffer, size_t start_pos, size_t len){
		std::string write_buffer;
		
		write_buffer = buffer.substr(start_pos, len);
		m_file << write_buffer.c_str();
	}
	
	void RawWriteText::newLine(){
		m_file << std::endl;
	}
	void RawWriteText::close(){
		m_file.close();
	}
}
