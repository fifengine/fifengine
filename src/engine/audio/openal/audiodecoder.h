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
#ifndef AUDIODECODER_H_
#define AUDIODECODER_H_

#include <string>
#include <stdint.h>

#include <AL/al.h>
#include "audioconfig.h"

namespace FIFE {

	/** The base class for the different audio decoder.
	 */
	class AudioDecoder {
		public:
			/** A factory function that returns an appropriate AudioDecoder.
			 * 
			 * @param filename Specifies the file that shall be opened.
			 * @return Returns a pointer to an allocated AudioDecoder (it's up to you to delete it again), 0 in case of an failure.
			 */
			static AudioDecoder *createDecoder(const std::string &filename);
		
			AudioDecoder() : m_isstereo(false), m_is8bit(false), m_samplerate(0) {}
			virtual ~AudioDecoder() {}

			virtual uint32_t guessedLength()=0;
			
			/** A streaming buffer or not?
			 * 
			 * The decision if we decode the whole audio file in one buffer or use a kind
			 * of streaming depends on the value of AUDIO_MAX_KEEP_IN_MEM from audioconfig.h.
			 * 
			 * @return Return true for a streaming decoder, false if the sound is decoded in one buffer
			 */
			bool needsStreaming(); // depends on audioconfig.h
			
			virtual bool decodeAll()=0;
			virtual uint32_t decodePart(uint32_t)=0;
			virtual void *getBuffer()=0;
			virtual uint32_t getBufferSize()=0;

			/** Tests if the audio data is stereo data or mono.
			 * 
			 * @return Returns true if the audio data is stereo, false if mono.
			 */		
			bool isStereo();
			
			ALenum getALFormat();
			short getBitResolution();
			uint32_t getSampleRate();
		
		protected:
			bool m_isstereo, m_is8bit;
			uint32_t m_samplerate;
	};


	// Implementation of trivial functions to allow the compiler to inline them
	inline bool AudioDecoder::needsStreaming() { return guessedLength()>AUDIO_MAX_KEEP_IN_MEM; }
	inline bool AudioDecoder::isStereo() { return m_isstereo; }
	inline short AudioDecoder::getBitResolution() { return m_is8bit?8:16; }
	inline uint32_t AudioDecoder::getSampleRate() { return m_samplerate; }
}

#endif
