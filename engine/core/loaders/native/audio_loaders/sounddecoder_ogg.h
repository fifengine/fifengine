/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_SOUNDDECODER_OGG_H
#define FIFE_SOUNDDECODER_OGG_H

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes
#include <vorbisfile.h>
#include <boost/scoped_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "audio/sounddecoder.h"

namespace FIFE {
	class SoundDecoderOgg : public SoundDecoder {
	public:
		
		SoundDecoderOgg(RawData* ptr);
		
		~SoundDecoderOgg() {
			releaseBuffer();
		}
		
		/** Returns the decoded length of the file in bytes
		 */
		unsigned long getDecodedLength() const{
			return m_declength;
		}
		
		/** Sets the current position in the file (in bytes)
		 *
		 * @return 0 (False), if the positioning was successful
		 */
		bool setCursor(unsigned long pos);
		
		/** Request the decoding of the next part of the stream.
		 *
		 * @param length The length of the decoded part
		 * @return 0 (False), if decoding was successful
		 */
		bool decode(unsigned long length);
		
		/** Returns the next decoded buffer.
		 *
		 * The length of the buffer is returned by getBufferSize().
		 */
		void *getBuffer() const{
			return m_data;
		}
		
		/** Returns the byte-size of the buffer returned by getBuffer().
		 */
		unsigned long getBufferSize() {
			return m_datasize;
		}
		
		/** Releases the buffer returned by getBuffer()
		 */
		void releaseBuffer() {
			if (m_data != NULL) {
				delete[] m_data;
				m_data = NULL;
			}
		}
		
	private:
		boost::scoped_ptr<RawData> 	m_file;
		unsigned long		m_declength;
		unsigned long		m_datasize;
		char*						m_data;
		OggVorbis_File 	m_ovf;
	};
}

#endif
