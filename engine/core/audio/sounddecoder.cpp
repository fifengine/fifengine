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

// Platform specific includes

// 3rd party library includes
#include <boost/scoped_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "util/logger.h"
#include "util/exception.h"

#include "sounddecoder_ogg.h"

namespace FIFE {
	static Logger _log(LM_AUDIO);

	SoundDecoder* SoundDecoder::create(const std::string& filename) {
		RawData* rdptr = VFS::instance()->open(filename);
		
		SoundDecoder* ptr;
		if(filename.find(".ogg", filename.size() - 4) != std::string::npos) {
			ptr = new SoundDecoderOgg(rdptr);
			
		} else {
			FL_WARN(_log, LMsg() << "No audio-decoder available for file \"" << filename << "\"!");
			throw Exception("");
		}
		
		return ptr;
	}	
}
