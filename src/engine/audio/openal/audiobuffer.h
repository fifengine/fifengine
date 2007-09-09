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
 
#ifndef AUDIOBUFFER_H_
#define AUDIOBUFFER_H_

#include <AL/al.h>
#include <boost/shared_ptr.hpp>
#include <string>
#include <stdint.h>

namespace FIFE {
	
	class AudioDecoder;
	typedef ::boost::shared_ptr<AudioDecoder> audiodec_sp;
	
	/** The base class for the different types of buffers (streaming and not-streaming).
	 */
	class AudioBuffer {
		public:
			virtual ~AudioBuffer() { }

			/** Does this AudioBuffer requires a streaming mechanism?
			 * 
			 * @return Returns true if streaming is required, false if not.
			 */
			bool isStreaming() { return m_stream; }
            
			/** Checks for an end-of-file status.
			 * 
			 * @return Returns true if the end of file is reached, false if not.
			 */
			bool isEOF() { return m_eof; }

			/** Factory function for the different AudioBuffer classes
			 * 
			 * @param name The name of the audio file.
			 * @param ad A shared_pointer to the audio decoder
			 * @return A pointer to an AudioBuffer (you have to free it).
			 */
			static AudioBuffer *createAudioBuffer(const std::string &name, audiodec_sp ad);

			virtual uint8_t countBuffers()=0;
			virtual ALuint  acquireBuffer()=0;
			virtual void    fillBuffer()=0;
			virtual ALuint  releaseBuffer()=0;

			const std::string m_name;

		protected:
			AudioBuffer(const std::string &name) : m_name(name), m_stream(false), m_eof(false) {}
			bool m_stream, m_eof;
	};

	typedef ::boost::shared_ptr<AudioBuffer> type_audiobufptr;

} // namespace FIFE

#endif /*AUDIOBUFFER_H_*/
