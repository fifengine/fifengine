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
 
#ifndef FIFE_AUDIO_BUFFER_H
#define FIFE_AUDIO_BUFFER_H

// Standard C++ library includes
#include <string>

// Platform specific includes
#include "util/fifeint.h"

// 3rd party library includes
#include <AL/al.h>
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE { namespace audio {
	
	class Decoder;
	typedef ::boost::shared_ptr<Decoder> type_decptr;
	
	/** The base class for the different types of buffers (streaming and not-streaming).
	 */
	class Buffer {
		public:
			virtual ~Buffer() {}

			/** Does this Buffer requires a streaming mechanism?
			 * 
			 * @return Returns true if streaming is required, false if not.
			 */
			bool isStreaming() { return m_stream; }

			/** Checks for an end-of-file status.
			 * 
			 * @return Returns true if the end of file is reached, false if not.
			 */
			bool isEOF() { return m_eof; }

			/** Factory function for the different Buffer classes
			 * 
			 * @param name The name of the audio file.
			 * @param ad A shared_pointer to the audio decoder
			 * @return A pointer to an Buffer (you have to free it).
			 */
			static Buffer* create(const std::string &name, type_decptr ad);

			/** Returns the number of the internal buffers used by the Buffer.
			 * 
			 * @return Returns the number of internal buffers.
			 */
			virtual uint8_t countBuffers() = 0;

			/** Returns the OpenAL id of the next buffer that is ready (filled with data).
			 * 
			 * @return The id of the next ready buffer. 0 if no buffer is ready.
			 */
			virtual ALuint  acquireBuffer() = 0;

			/** Request that the next buffer is filled with data.
			 */
			virtual void    fillBuffer() = 0;

			/** Please ignore this function.
			 * 
			 * @return 
			 */
			virtual ALuint  releaseBuffer() = 0;

			const std::string m_name;

		protected:
			Buffer(const std::string &name) : m_name(name), m_stream(false), m_eof(false) {}
			bool m_stream, m_eof;
	};

	typedef ::boost::shared_ptr<Buffer> type_bufptr;

} } // namespace FIFE::audio

#endif /*AUDIOBUFFER_H_*/
