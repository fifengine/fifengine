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

#ifndef FIFE_SOUNDCLIP_H_
#define FIFE_SOUNDCLIP_H_

// Standard C++ library includes
#include <vector>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/resource/resource.h"

#include "sounddecoder.h"

namespace FIFE {

	/** Different types of audio-file positions
	 */
	enum SoundPositionType {
		SD_SAMPLE_POS,
		SD_TIME_POS,
		SD_BYTE_POS
	};

	struct SoundBufferEntry {
		ALuint buffers[BUFFER_NUM];
		uint32_t usedbufs;
		uint64_t deccursor;
	};

	/**  Class to handle the buffers of an audio file
	 */
	class SoundClip : public IResource {
	public:

		SoundClip(IResourceLoader* loader = 0);
		SoundClip(const std::string& name, IResourceLoader* loader = 0);

		~SoundClip();

		/** Does this SoundClip require a streaming mechanism?
		 *
		 * @return Returns true if streaming is required, false if not.
		 */
		bool isStream() const {
			return m_isstream;
		}

		/** Returns the number of buffers used by the SoundClip
		 * (only for non-streaming sound clips)
		 *
		 * @return Returns the number of buffers.
		 */
		uint32_t countBuffers() const {
			return m_buffervec.at(0)->usedbufs;
		}

		/** Returns the array of buffers for queuing
		 *
		 */
		ALuint* getBuffers(uint32_t streamid = 0) const {
			return m_buffervec.at(streamid)->buffers;
		}

		/** Starts streaming the soundclip
		 * @return Returns the streamid
		 */
		uint32_t beginStreaming();

		/** Fills the streaming-buffers with initial data
		 *
		 * @param streamid The stream ID
		 */
		void acquireStream(uint32_t streamid);

		/** Sets the stream position
		 * @return True if position is invalid (EOF has been reached)
		 */
		bool setStreamPos(uint32_t streamid, SoundPositionType type, float value);

		/** Gets the stream position
		 */
		float getStreamPos(uint32_t streamid, SoundPositionType type) const;

		/** Refill a processed buffer with new data
		 *
		 *  @return True if file was EOF
		 *  @param streamid The stream ID
		 *  @param buffer The OpenAL buffer ID
		 */
		bool getStream(uint32_t streamid, ALuint buffer);

		/** Quits Streaming
		 */
		void quitStreaming(uint32_t streamid);

		/** Adopts a decoder to use so DONT delete it
		 */
		void adobtDecoder(SoundDecoder* decoder) { m_decoder = decoder; m_deletedecoder = true; }

		/** Sets the decoder to use so DONT delete it before this SoundClip is done with it
		 */
		void setDecoder(SoundDecoder* decoder) { m_decoder = decoder; m_deletedecoder = false; }

		/** Returns the attached decoder
		 */
		SoundDecoder* getDecoder() const {
			return m_decoder;
		}

		virtual size_t getSize() { return 0; }

		virtual void load();
		virtual void free();

	private:
		bool						m_isstream; 		// is stream?
		SoundDecoder*		m_decoder;			// attached decoder
		bool						m_deletedecoder;	// when loadFromDecoder-method is used, decoder shouldn't be deleted
		std::vector<SoundBufferEntry*> m_buffervec;

		std::string createUniqueClipName();
	};

	typedef SharedPtr<SoundClip> SoundClipPtr;
}

#endif
