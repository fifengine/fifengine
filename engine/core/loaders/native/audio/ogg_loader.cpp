/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "audio/soundclip.h"
#include "vfs/vfs.h"
#include "util/log/logger.h"
#include "util/base/exception.h"

#include "ogg_loader.h"
#include "sounddecoder_ogg.h"

namespace FIFE {
  static Logger _log(LM_NATIVE_LOADERS);

	void OggLoader::load(IResource* res) {
		VFS* vfs = VFS::instance();

		std::string filename = res->getName();

		RawData* rdptr = vfs->open(filename);
		dynamic_cast<SoundClip*>(res)->adobtDecoder(new SoundDecoderOgg(rdptr));
	}
}
