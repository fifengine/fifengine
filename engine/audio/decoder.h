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

#ifndef FIFE_AUDIO_DECODER_H
#define FIFE_AUDIO_DECODER_H

// Standard C++ library includes
#include <string>

// Platform specific includes
#include "util/fife_stdint.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "config.h"
#include "fife_openal.h"

namespace FIFE { namespace audio {

	/** The base class for the different audio decoder.
	 */
	class Decoder {
		public:
			/** A factory function that returns an appropriate AudioDecoder.
			 * 
			 * @param filename Specifies the file that shall be opened.
			 * @return Returns a pointer to an allocated AudioDecoder (it's up to you
			 * to delete it again), 0 in case of an failure.
			 */
			static Decoder* create(const std::string &filename);
		
			Decoder() : m_isstereo(false), m_is8bit(false), m_samplerate(0) {}
			virtual ~Decoder() {}

			/** Returns a educated guess about length of the decoded stream.
			 *
			 * To decide if the stream requires streaming or not, we need to know the
			 * length of the decoded stream. The class that implements AudioDecoder 
			 * must return here a guessed length of the decoder length.
			 */
			virtual uint32_t guessedLength() = 0;
			
			/** A streaming buffer or not?
			 * 
			 * The decision if we decode the whole audio file in one buffer or use a 
			 * kind of streaming depends on the value of MAX_KEEP_IN_MEM from 
			 * audioconfig.h.
			 * 
			 * @return Return true for a streaming decoder, false if the sound is 
			 * decoded in one buffer
			 */
			bool needsStreaming() { return guessedLength() > MAX_KEEP_IN_MEM; }
		 	// depends on audioconfig.h
			
			/** Request that the whole stream is decoded in one go.
			 *
			 * Please keep in mind that it could take up a lot of memory.
			 */
			virtual bool decodeAll() = 0;

			/** Request the decoding of the next part of the stream.
			 */
			virtual uint32_t decodePart(uint32_t) = 0;

			/** Returns the next decoded buffer.
			 *
			 * The length of the buffer is returned by getBufferSize().
			 */
			virtual void *getBuffer() = 0;

			/** Returns the size of a buffer as returned by getBuffer().
			 */
			virtual uint32_t getBufferSize() = 0;

			/** Tests if the audio data is stereo data or mono.
			 * 
			 * @return Returns true if the audio data is stereo, false if mono.
			 */		
			bool isStereo() { return m_isstereo; }
			
			ALenum getALFormat() {
				if (m_isstereo) {
					return m_is8bit ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
				} else {
					return m_is8bit ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
				}
			}
			short getBitResolution() { return m_is8bit ? 8 : 16; }
			uint32_t getSampleRate() { return m_samplerate; }
		
		protected:
			bool m_isstereo, m_is8bit;
			uint32_t m_samplerate;
	};

} }

#endif
