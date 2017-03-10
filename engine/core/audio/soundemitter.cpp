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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"
#include "util/time/timemanager.h"
#include "util/base/exception.h"
#include "soundemitter.h"
#include "soundmanager.h"
#include "soundclipmanager.h"

namespace FIFE {
	static Logger _log(LM_AUDIO);

	SoundEmitter::SoundEmitter(SoundManager* manager, uint32_t uid) : m_manager(manager), m_source(0), m_soundclip(), m_soundclipid(0), m_streamid(0),
															m_emitterid(uid), m_loop(false) {
		if (!m_manager->isActive()) {
			return;
		}

		TimeManager::instance()->registerEvent(this);
		setPeriod(-1);
		alGenSources(1, &m_source);
		CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error creating source")
	}

	SoundEmitter::~SoundEmitter() {
		if (!m_manager->isActive()) {
			return;
		}

		setPeriod(-1);
		TimeManager::instance()->unregisterEvent(this);
		reset();
		alDeleteSources(1, &m_source);
	}

	void SoundEmitter::reset(bool defaultall) {
		if (m_soundclip) {

			setPeriod(-1);
			alSourceStop(m_source);

			// Release all buffers
			alSourcei(m_source, AL_BUFFER, AL_NONE);
			alGetError();

			if (m_soundclip->isStream()) {
				m_soundclip->quitStreaming(m_streamid);
			}

			// release the soundclip
			//SoundClipManager::instance()->free(m_soundclipid);
			m_soundclip.reset();

			// default source properties
			if (defaultall) {
				setPosition(0.0f, 0.0f, 0.0f);
				setVelocity(0.0f, 0.0f, 0.0f);
				setGain(1.0f);
				setPositioning(false);
				alSourcei(m_source, AL_LOOPING, AL_FALSE);
			}
		}
	}

	void SoundEmitter::release() {
		m_manager->releaseEmitter(m_emitterid);
	}

	void SoundEmitter::setSoundClip(SoundClipPtr soundclip) {
		m_soundclipid = soundclip->getHandle();
		m_soundclip = soundclip;

		attachSoundClip();
	}

	void SoundEmitter::setCallback(const type_callback& cb) {
		m_callback = cb;
	}

	void SoundEmitter::attachSoundClip() {
		if (!m_soundclip->isStream()) {
			// non-streaming
			alSourceQueueBuffers(m_source, m_soundclip->countBuffers(), m_soundclip->getBuffers());
			alSourcei(m_source, AL_LOOPING, m_loop ? AL_TRUE : AL_FALSE);

		} else {
			// streaming
			m_streamid = m_soundclip->beginStreaming();
			m_soundclip->acquireStream(m_streamid);

			// queue initial buffers
			alSourceQueueBuffers(m_source, BUFFER_NUM, m_soundclip->getBuffers(m_streamid));
			alSourcei(m_source, AL_LOOPING, AL_FALSE);
		}

		CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error attaching sound clip")
	}

	void SoundEmitter::updateEvent(uint32_t time) {
		ALint procs;
		ALint bufs;
		ALuint buffer;

		alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &procs);

		while (procs--) {
			alSourceUnqueueBuffers(m_source, 1, &buffer);

			if (m_soundclip->getStream(m_streamid, buffer)) {
				// EOF!
				if (m_loop) {
					// play again from the beginning
					m_soundclip->setStreamPos(m_streamid, SD_BYTE_POS, 0);
					m_soundclip->getStream(m_streamid, buffer);
				} else {

					// check if the playback has been finished
					alGetSourcei(m_source, AL_BUFFERS_QUEUED, &bufs);
					if (bufs == 0) {
						setPeriod(-1);
						alSourceStop(m_source);
						if(m_callback) {
							m_callback();
						}
					}
					continue;
				}
			}
			alSourceQueueBuffers(m_source, 1, &buffer);
		}

		CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error while streaming")
	}

	void SoundEmitter::setLooping(bool loop) {
		if (m_soundclip) {
			if (!m_soundclip->isStream()) {
				alSourcei(m_source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
			} else {
				alSourcei(m_source, AL_LOOPING, AL_FALSE);
			}
		}
		m_loop = loop;
	}

	void SoundEmitter::play() {
		if (m_soundclip) {
			alSourcePlay(m_source);
			if (m_soundclip->isStream()) {
				setPeriod(5000);
			}
		}
	}

	void SoundEmitter::stop() {
		if (m_soundclip) {
			alSourceStop(m_source);

			if (m_soundclip->isStream()) {
				setPeriod(-1);
				setCursor(SD_BYTE_POS, 0);
			} else {
				alSourceRewind(m_source);
			}
		}
	}

	void SoundEmitter::setCursor(SoundPositionType type, float value) {
		if (!m_soundclip) {
			return;
		}

		ALint state = 0;

		if (!m_soundclip->isStream()) {
			switch(type) {
			case SD_BYTE_POS:
				alSourcef(m_source, AL_BYTE_OFFSET, value);
				break;
			case SD_SAMPLE_POS:
				alSourcef(m_source, AL_SAMPLE_OFFSET, value);
				break;
			case SD_TIME_POS:
				alSourcef(m_source, AL_SEC_OFFSET, value);
				break;
			}

			CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error setting cursor position")
		}
		else {
			alGetSourcei(m_source, AL_SOURCE_STATE, &state);

			if (state == AL_PLAYING || AL_PAUSED) {
				setPeriod(-1);
				alSourceStop(m_source);
			}

			m_soundclip->setStreamPos(m_streamid, type, value);

			// detach all buffers
			alSourcei(m_source, AL_BUFFER, 0);

			// queue the buffers with new data
			m_soundclip->acquireStream(m_streamid);
			alSourceQueueBuffers(m_source, BUFFER_NUM, m_soundclip->getBuffers(m_streamid));

			if (state == AL_PLAYING) {
				setPeriod(5000);
				alSourcePlay(m_source);
			}

			CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error setting stream cursor position")
		}
	}

	float SoundEmitter::getCursor(SoundPositionType type) {
		if (!m_soundclip) {
			return 0.0f;
		}

		ALfloat pos = 0.0f;

		switch(type) {
			case SD_BYTE_POS:
				alGetSourcef(m_source, AL_BYTE_OFFSET, &pos);
				break;
			case SD_SAMPLE_POS:
				alGetSourcef(m_source, AL_SAMPLE_OFFSET, &pos);
				break;
			case SD_TIME_POS:
				alGetSourcef(m_source, AL_SEC_OFFSET, &pos);
				break;
		}

		if (m_soundclip->isStream()) {
			pos += m_soundclip->getStreamPos(m_streamid, type);
		}

		CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error getting cursor")

		return pos;
	}
}
