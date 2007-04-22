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

// FIFE includes
#include "vfs.h"
#include "vfswriteformat.h"

namespace FIFE {

	VFSWriteFormat::VFSWriteFormat() : m_supported_formats() {}


	VFSWriteFormat::~VFSWriteFormat() {
		VFS::instance()->removeFormat(this);
	}

	bool VFSWriteFormat::supportsFormat(file_format* format){
		for( unsigned int i = 0; i < m_supported_formats.size(); i++ ){
			if( format == m_supported_formats[i]){
				return true;
			}
		}
		return false;
	}
	format_list VFSWriteFormat::getSupportedFormats(){
		return m_supported_formats;
	}	
}
