/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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

#ifndef FIFE_SOUNDDECODER_H
#define FIFE_SOUNDDECODER_H

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "soundconfig.h"
#include "fife_openal.h"

namespace FIFE {

	class SoundDecoder {
	public:

		virtual ~SoundDecoder() {}

		/** Returns the decoded length of the file in bytes
		 */
		virtual uint64_t getDecodedLength() const = 0;

		/** A stream or not?
		 *
		 * The decision if we decode the whole audio file in one buffer or use a
		 * kind of streaming depends on the value of MAX_KEEP_IN_MEM from
		 * soundconfig.h
		 *
		 * @return Return true for a streaming decoder, false if the sound is
		 * decoded in one buffer
		 */
		bool needsStreaming() const { return getDecodedLength() > MAX_KEEP_IN_MEM; }

		/** Sets the current position in the file (in bytes)
		 *
		 * @return 0 (False), if the positioning was successful
		 */
		virtual bool setCursor(uint64_t pos) = 0;

		/** Request the decoding of the next part of the stream.
		 *
		 * @param length The length of the decoded part
		 * @return 0 (False), if decoding was successful
		 */
		virtual bool decode(uint64_t length) = 0;

		/** Returns the next decoded buffer.
		 *
		 * The length of the buffer is returned by getBufferSize().
		 */
		virtual void *getBuffer() const = 0;

		/** Returns the byte-size of the buffer returned by getBuffer().
		 */
		virtual uint64_t getBufferSize() = 0;

		/** Releases the buffer returned by getBuffer()
		 */
		virtual void releaseBuffer() = 0;

		/** Tests if the audio data is stereo data or mono.
		 *
		 * @return Returns true if the audio data is stereo, false if mono.
		 */
		bool isStereo() const {
			return m_isstereo;
		}

		/** Returns the openAL-Format of the audio file
		 */
		ALenum getALFormat() const {
			if (m_isstereo) {
				return m_is8bit ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
			} else {
				return m_is8bit ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
			}
		}

		/** Returns the bit resolution
		 */
		int16_t getBitResolution() const {
			return m_is8bit ? 8 : 16;
		}

		/** Returns the sample rate
		 */
		uint64_t getSampleRate() const{
			return m_samplerate;
		}

	protected:
		bool					m_isstereo;
		bool					m_is8bit;
		uint64_t	m_samplerate;
	};
}

#endif
