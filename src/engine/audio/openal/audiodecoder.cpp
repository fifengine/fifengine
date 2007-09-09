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

#include "audiodecoder.h"
#include "vfs.h"
#include "rawdata.h"
#include "audiodecoder_oggvorbis.h"
#include "audiodecoder_acm.h"

namespace FIFE {

	AudioDecoder *AudioDecoder::createDecoder(const std::string &filename) {
		RawDataPtr rdptr( VFS::instance()->open(filename) );
		if (!rdptr)
			return 0;
		
		if (OggVorbisDecoder::isOggVorbisFile(rdptr)) {
			return new OggVorbisDecoder(rdptr);
		}
		
		if (ACMDecoder::isACMFile(rdptr)) {
			return new ACMDecoder(rdptr, filename);
		}
		
		return 0;
	}

	ALenum AudioDecoder::getALFormat() {
		if (m_is8bit && m_isstereo)
			return AL_FORMAT_STEREO8;
		if (!m_is8bit && m_isstereo)
			return AL_FORMAT_STEREO16;
		if (m_is8bit && !m_isstereo)
			return AL_FORMAT_MONO8;
		if (!m_is8bit && !m_isstereo)
			return AL_FORMAT_MONO16;
		
		return AL_FORMAT_STEREO16;
	}

}

