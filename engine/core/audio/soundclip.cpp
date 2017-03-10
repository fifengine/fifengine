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

// Standard C++ library includes

// Platform specific includes
#include <sstream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "loaders/native/audio/ogg_loader.h"

#include "soundclip.h"

namespace FIFE {
	static Logger _log(LM_AUDIO);

	SoundClip::SoundClip(IResourceLoader* loader) :
		IResource(createUniqueClipName(), loader),
		m_isstream(false),
		m_decoder(NULL),
		m_deletedecoder(false) {

	}

	SoundClip::SoundClip(const std::string& name, IResourceLoader* loader) :
		IResource(name, loader),
		m_isstream(false),
		m_decoder(NULL),
		m_deletedecoder(false) {

	}

	void SoundClip::load(){
		if (m_loader){
			m_loader->load(this);
		}
		else {  //no loader specified so find one to use
			if(m_name.find(".ogg", m_name.size() - 4) != std::string::npos) {
				OggLoader loader;
				loader.load(this);
			} else {
				FL_WARN(_log, LMsg() << "No audio-decoder available for file \"" << m_name << "\"!");
				throw InvalidFormat("Error: Ogg loader can't load files without ogg extension");
			}
		}

		assert(m_decoder);  //should be set by now

		m_isstream = m_decoder->needsStreaming();

		if (!m_isstream) {

			// only for non-streaming buffers
			SoundBufferEntry* ptr = new SoundBufferEntry();

			// iterate the bufs and fill them with data
			for (int32_t i = 0; i < BUFFER_NUM; i++) {

				if (m_decoder->decode(BUFFER_LEN)) {
					// EOF or error
					break;
				}

				// generate buffer and fill it with data
				alGenBuffers(1, &ptr->buffers[i]);

				alBufferData(ptr->buffers[i], m_decoder->getALFormat(), m_decoder->getBuffer(),
					m_decoder->getBufferSize(), m_decoder->getSampleRate());

				CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error copying data to buffers")

				ptr->usedbufs++;
			}

			m_decoder->releaseBuffer();

			// push the buffer information to the vector
			m_buffervec.push_back(ptr);

		}

		m_state = IResource::RES_LOADED;
	}

	void SoundClip::free(){
		if (m_state == IResource::RES_LOADED) {
			if (m_isstream) {
				// erase all elements from the list
				std::vector<SoundBufferEntry*>::iterator it;

				for (it = m_buffervec.begin(); it != m_buffervec.end(); ++it) {
					if ((*it)->buffers[0] != 0) {
						alDeleteBuffers(BUFFER_NUM, (*it)->buffers);
					}
					delete (*it);
				}
				m_buffervec.clear();
			}
			else {
				// for non-streaming soundclips
				SoundBufferEntry* ptr = m_buffervec.at(0);

				for(uint32_t i = 0; i < ptr->usedbufs; i++) {
					alDeleteBuffers(1, &ptr->buffers[i]);
				}
			}
		}
		m_state = IResource::RES_NOT_LOADED;
	}

	uint32_t SoundClip::beginStreaming() {
		// create new sound buffer entry
		SoundBufferEntry* ptr = new SoundBufferEntry();
		ptr->usedbufs=0;
		alGenBuffers(BUFFER_NUM, ptr->buffers);

		CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error creating streaming-buffers")

		m_buffervec.push_back(ptr);

		return m_buffervec.size()-1;
	}

	bool SoundClip::setStreamPos(uint32_t streamid, SoundPositionType type, float value) {
		uint64_t pos = 0;

		// convert position to bytes
		switch (type) {
			case SD_BYTE_POS:
				pos = static_cast<uint64_t>(value);
				break;
			case SD_TIME_POS:
				value *= m_decoder->getSampleRate();
			case SD_SAMPLE_POS:
				pos = static_cast<uint64_t>((m_decoder->getBitResolution() / 8) * (m_decoder->isStereo() ? 2 : 1) * value);
				break;
		}

		if (pos > m_decoder->getDecodedLength()) {
			// EOF!
			return true;
		}

		// set cursor position
		m_buffervec.at(streamid)->deccursor = pos;
		return false;
	}

	float SoundClip::getStreamPos(uint32_t streamid, SoundPositionType type) const{
		uint64_t pos = m_buffervec.at(streamid)->deccursor;
		switch(type) {
			case SD_BYTE_POS:
				return pos;
			case SD_SAMPLE_POS:
				return pos / (m_decoder->getBitResolution() / 8 * (m_decoder->isStereo() ? 2 : 1));
			case SD_TIME_POS:
				return pos / (m_decoder->getBitResolution() / 8 * (m_decoder->isStereo() ? 2 : 1) * m_decoder->getSampleRate());
		}
		return 0.0f;
	}

	void SoundClip::acquireStream(uint32_t streamid) {

		SoundBufferEntry* ptr = m_buffervec.at(streamid);

		for (int32_t i = 0; i < BUFFER_NUM; i++) {
			getStream(streamid, ptr->buffers[i]);
		}
	}

	bool SoundClip::getStream(uint32_t streamid, ALuint buffer) {

		SoundBufferEntry* ptr = m_buffervec.at(streamid);

		if (ptr->deccursor >= m_decoder->getDecodedLength()) {
			// EOF!
			return true;
		}

		// set cursor of decoder
		m_decoder->setCursor(ptr->deccursor);

		// Error while decoding file?
		if (m_decoder->decode(BUFFER_LEN)) {
			throw Exception("error while reading from audio file");
		}

		// fill the buffer with data
		alBufferData(buffer, m_decoder->getALFormat(),
			m_decoder->getBuffer(), m_decoder->getBufferSize(), m_decoder->getSampleRate());

		m_decoder->releaseBuffer();

		// update cursor
		ptr->deccursor += BUFFER_LEN;

		CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error catching stream")

		return false;
	}

	void SoundClip::quitStreaming(uint32_t streamid) {
		// release the buffers
		SoundBufferEntry* ptr = m_buffervec.at(streamid);
		alDeleteBuffers(BUFFER_NUM, ptr->buffers);
		ptr->buffers[0] = 0;
	}

	SoundClip::~SoundClip() {
		free();

		// delete decoder
		if (m_deletedecoder && m_decoder != NULL) {
			delete m_decoder;
		}
	}

	std::string SoundClip::createUniqueClipName() {
	        // automated counting for name generation, in case the user doesn't provide a name
	        static uint32_t uniqueNumber = 0;
	        static std::string baseName = "soundclip";

	        std::ostringstream oss;
	        oss << uniqueNumber << "_" << baseName;

	        const std::string name = oss.str();
	        ++uniqueNumber;

	        return name;
	}
}
