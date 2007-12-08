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

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/logger.h"
#include "util/exception.h"

#include "sounddecoder_wav.h"

namespace FIFE {
	static Logger _log(LM_AUDIO);

	SoundDecoderWav::SoundDecoderWav(RawDataPtr ptr) {
		unsigned long 	tmp;
		uint8_t 		buffer[4];
		m_declength = 0;
		m_data = NULL;
		m_datasize = 0;
		
		// Check that the initial chunk ID is "RIFF" -- 4 bytes
		ptr->readInto(buffer, 4);
		if (buffer[0] != 'R' || buffer[1] != 'I' || buffer[2] != 'F' || buffer[3] != 'F') {
			FL_ERR(_log, LMsg() << "Invalid wav chunk-id");
			throw InvalidFormat("");
			return;
		}
		
		// skip the 4 bytes chunk size
		ptr->moveIndex(4);
		
		// Check format to be "WAVE" -- 4 bytes
		ptr->readInto(buffer, 4);
		if (buffer[0] != 'W' || buffer[1] != 'A' || buffer[2] != 'V' || buffer[3] != 'E') {
			FL_ERR(_log, LMsg() << "Invalid wav chunk-id");
			throw InvalidFormat("");
			return;
		}
		
		// Check SubChunk ID to be "fmt " -- 4 bytes
		ptr->readInto(buffer, 4);
		if (buffer[0] != 'f' || buffer[1] != 'm' || buffer[2] != 't' || buffer[3] != ' ') {
			FL_ERR(_log, LMsg() << "Invalid wav format");
			throw InvalidFormat("");
			return;
		}
	
		// Check subchunk size (to be 16) -- 4 bytes
		ptr->readInto(buffer, 4);
		memcpy(&tmp, buffer, 4);
		if (tmp != 16) {
			FL_ERR(_log, LMsg() << "Invalid wav format");
			throw InvalidFormat("");
			return;
		}
	
		// Check audio format (only PCM supported currently) -- 2 bytes
		ptr->readInto(buffer, 2);
		tmp = 0;
		memcpy(&tmp, buffer, 2);
		if (tmp != 1) { // PCM == 1
			FL_ERR(_log, LMsg() << "Invalid wav format");
			throw InvalidFormat("");
			return;
		}
	
		// Get the number of channels (only mono and stereo supported) -- 2 bytes
		ptr->readInto(buffer, 2);
		tmp = 0;
		memcpy(&tmp, buffer, 2);
		
		if (tmp == 1) {
			m_isstereo = false;
		} else if (tmp == 2) {
			m_isstereo = true;
		}
		else {
			FL_ERR(_log, LMsg() << "Invalid nr of channels");
			throw InvalidFormat("");
			return;
		}
		
		// Get sample rate (usually 11025, 22050, or 44100 Hz) -- 4 bytes
		ptr->readInto(buffer, 4);
		memcpy(&m_samplerate, buffer, 4);
	
		/* skip byte rate -- 4 bytes
		 * skip block alignment (channels * bits_per_sample / 8) -- 2 bytes
		 */
		ptr->moveIndex(6);
		
		// Get bits per sample -- 2 bytes
		ptr->readInto(buffer, 2);
		memcpy(&tmp, buffer, 2);
	
		if (tmp == 8) {
			m_is8bit = true;	
		} else if (tmp == 16) {
			m_is8bit = false;
		} else {
			FL_ERR(_log, LMsg() << "Invalid wav format");
			throw InvalidFormat("");
			return;
		}
	
		// Check subchunk 2 ID (to be "data") -- 4 bytes
		ptr->readInto(buffer, 4);
		if (buffer[0] != 'd' || buffer[1] != 'a' || buffer[2] != 't' || buffer[3] != 'a') {
			FL_ERR(_log, LMsg() << "Invalid wav format");
			throw InvalidFormat("");
			return;
		}
	
		// Check subchunk 2 size -- 4 bytes
		ptr->readInto(buffer, 4);
		memcpy(&m_declength, buffer, 4);

		m_beginning = ptr->getCurrentIndex();
		m_file = ptr;
	}

	bool SoundDecoderWav::decode(unsigned long length) {
		m_datasize = 0;
		if (m_file->getCurrentIndex()+length > m_file->getDataLength()) {
			length = m_file->getDataLength()-m_file->getCurrentIndex();
			if (length <= 0) {
				return true;
			}
		}

		try {
		releaseBuffer();
		m_data = new char[length];
		m_file->readInto(reinterpret_cast<uint8_t*>(m_data), length);
		}
		catch (...) {
			return true;
		}
		m_datasize = length;
		return false;
	}

	bool SoundDecoderWav::setCursor(unsigned long pos) {
		try {
			m_file->setIndex(m_beginning + pos);
		}
		catch (...) {
			return true;
		}
		return false;
	}
}
