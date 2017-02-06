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
#include <algorithm>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"
#include "util/base/exception.h"
#include "soundemitter.h"
#include "soundmanager.h"
//#include "soundclipmanager.h"

namespace FIFE {
	static Logger _log(LM_AUDIO);

	SoundEmitter::SoundEmitter(SoundManager* manager, uint32_t uid) :
		m_manager(manager),
		m_source(0),
		m_soundClip(),
		m_soundClipId(0),
		m_streamId(0),
		m_emitterId(uid),
		m_loop(false) {

		if (!m_manager->isActive()) {
			return;
		}

		alGenSources(1, &m_source);
		CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error creating source");
	}

	SoundEmitter::~SoundEmitter() {
		if (!m_manager->isActive()) {
			return;
		}

		reset();
		alDeleteSources(1, &m_source);
	}

	void SoundEmitter::update() {
		ALint procs;
		ALint bufs;
		ALuint buffer;

		alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &procs);

		while (procs--) {
			alSourceUnqueueBuffers(m_source, 1, &buffer);

			if (m_soundClip->getStream(m_streamId, buffer)) {
				// EOF!
				if (m_loop) {
					// play again from the beginning
					m_soundClip->setStreamPos(m_streamId, SD_BYTE_POS, 0);
					m_soundClip->getStream(m_streamId, buffer);
				} else {
					// check if the playback has been finished
					alGetSourcei(m_source, AL_BUFFERS_QUEUED, &bufs);
					if (bufs == 0) {
						alSourceStop(m_source);
						callOnSoundFinished();
					}
					continue;
				}
			} else {
				callOnSoundFinished();
			}
			alSourceQueueBuffers(m_source, 1, &buffer);
		}

		CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error while streaming")
	}

	uint32_t SoundEmitter::getId() const {
			return m_emitterId;
	}

	void SoundEmitter::setPositioning(bool relative) {
		alSourcei(m_source, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
	}

	bool SoundEmitter::isPositioning() const {
		ALint tmp;
		alGetSourcei(m_source, AL_SOURCE_RELATIVE, &tmp);
		return (tmp == AL_TRUE);
	}

	void SoundEmitter::setOrientation(const ExactModelCoordinate& orientation) {
		ALfloat vec[6] = { static_cast<ALfloat>(orientation.x), static_cast<ALfloat>(orientation.y), static_cast<ALfloat>(orientation.z),
			0.0, 0.0, 1.0};
		alSourcefv(m_source, AL_ORIENTATION, vec);
	}

	ExactModelCoordinate SoundEmitter::getOrientation() const {
		ALfloat vec[6];
		alGetSourcefv(m_source, AL_ORIENTATION, vec);
		return ExactModelCoordinate(vec[0], vec[1], vec[2]);
	}

	void SoundEmitter::setPitch(float pitch) {
		if (pitch > 0.0) {
			alSourcef(m_source, AL_PITCH, pitch);
		}
	}

	float SoundEmitter::getPitch() const {
		float tmp;
		alGetSourcef(m_source, AL_PITCH, &tmp);
		return tmp;
	}

	void SoundEmitter::setRolloff(float rolloff) {
		alSourcef(m_source, AL_ROLLOFF_FACTOR,  rolloff);
	}

	float SoundEmitter::getRolloff() const {
		float tmp;
		alGetSourcef(m_source, AL_ROLLOFF_FACTOR, &tmp);
		return tmp;
	}

	void SoundEmitter::reset(bool defaultall) {
		if (m_soundClip) {
			alSourceStop(m_source);

			// Release all buffers
			alSourcei(m_source, AL_BUFFER, AL_NONE);
			alGetError();

			if (m_soundClip->isStream()) {
				m_soundClip->quitStreaming(m_streamId);
			}

			// release the soundClip
			//SoundClipManager::instance()->free(m_soundClipId);
			m_soundClip.reset();

			// default source properties
			if (defaultall) {
				ExactModelCoordinate emc(0.0, 0.0, 0.0);
				setPosition(emc);
				setVelocity(emc);
				setGain(1.0f);
				setPositioning(false);
				setRolloff(1.0);
				alSourcei(m_source, AL_LOOPING, AL_FALSE);
				m_loop = false;
			}
		}
	}

	void SoundEmitter::release() {
		m_manager->releaseEmitter(m_emitterId);
	}

	void SoundEmitter::setSoundClip(SoundClipPtr soundClip) {
		m_soundClipId = soundClip->getHandle();
		m_soundClip = soundClip;

		attachSoundClip();
	}

	SoundClipPtr SoundEmitter::getSoundClip() {
		return m_soundClip;
	}

	void SoundEmitter::attachSoundClip() {
		if (!m_soundClip->isStream()) {
			// non-streaming
			alSourceQueueBuffers(m_source, m_soundClip->countBuffers(), m_soundClip->getBuffers());
			alSourcei(m_source, AL_LOOPING, m_loop ? AL_TRUE : AL_FALSE);

		} else {
			// streaming
			m_streamId = m_soundClip->beginStreaming();
			m_soundClip->acquireStream(m_streamId);

			// queue initial buffers
			alSourceQueueBuffers(m_source, BUFFER_NUM, m_soundClip->getBuffers(m_streamId));
			alSourcei(m_source, AL_LOOPING, AL_FALSE);
		}

		CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error attaching sound clip")
	}

	void SoundEmitter::setLooping(bool loop) {
		if (m_soundClip) {
			if (!m_soundClip->isStream()) {
				alSourcei(m_source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
			} else {
				alSourcei(m_source, AL_LOOPING, AL_FALSE);
			}
		}
		m_loop = loop;
	}

	bool SoundEmitter::isLooping() const {
		return m_loop;
	}

	void SoundEmitter::play() {
		if (m_soundClip) {
			alSourcePlay(m_source);
		}
	}

	void SoundEmitter::stop() {
		if (m_soundClip) {
			alSourceStop(m_source);

			if (m_soundClip->isStream()) {
				setCursor(SD_BYTE_POS, 0);
			} else {
				alSourceRewind(m_source);
			}
		}
	}

	void SoundEmitter::pause() {
		if (m_soundClip) {
			alSourcePause(m_source);
		}
	}

	void SoundEmitter::setGain(float gain) {
		alSourcef(m_source, AL_GAIN, gain);
	}

	float SoundEmitter::getGain() const {
		float tmp;
		alGetSourcef(m_source, AL_GAIN, &tmp);
		return tmp;
	}

	void SoundEmitter::setMaxGain(float gain) {
		alSourcef(m_source, AL_MAX_GAIN, gain);
	}

	float SoundEmitter::getMaxGain() const {
		float tmp;
		alGetSourcef(m_source, AL_MAX_GAIN, &tmp);
		return tmp;
	}

	void SoundEmitter::setMinGain(float gain) {
		alSourcef(m_source, AL_MIN_GAIN, gain);
	}

	float SoundEmitter::getMinGain() const {
		float tmp;
		alGetSourcef(m_source, AL_MIN_GAIN, &tmp);
		return tmp;
	}

	bool SoundEmitter::isStereo() {
		if (m_soundClip) {
			return m_soundClip->getDecoder()->isStereo();
		}
		return false;
	}

	int16_t SoundEmitter::getBitResolution() {
		if (m_soundClip) {
			return m_soundClip->getDecoder()->getBitResolution();
		}
		return 0;
	}

	uint64_t SoundEmitter::getSampleRate() {
		if (m_soundClip) {
			return m_soundClip->getDecoder()->getSampleRate();
		}
		return 0;
	}

	uint64_t SoundEmitter::getDecodedLength() {
		if (m_soundClip) {
			return m_soundClip->getDecoder()->getDecodedLength();
		}
		return 0;
	}

	uint64_t SoundEmitter::getDuration() {
		if (m_soundClip) {
			//convert to milliseconds
			double samplerate = static_cast<double>(getSampleRate()) / 1000.0;
			double bitres = static_cast<double>(getBitResolution());
			// convert to bits
			double size = static_cast<double>(getDecodedLength()) * 8.0;
			double stereo = (isStereo() ? 2.0 : 1.0);
			double time = ( size / (samplerate * bitres) ) / stereo;
			
			return static_cast<uint64_t>(time);
		}
		return 0;
	}

	void SoundEmitter::setCursor(SoundPositionType type, float value) {
		if (!m_soundClip) {
			return;
		}

		ALint state = 0;

		if (!m_soundClip->isStream()) {
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
		} else {
			alGetSourcei(m_source, AL_SOURCE_STATE, &state);

			if (state == AL_PLAYING || AL_PAUSED) {
				alSourceStop(m_source);
			}

			m_soundClip->setStreamPos(m_streamId, type, value);

			// detach all buffers
			alSourcei(m_source, AL_BUFFER, 0);

			// queue the buffers with new data
			m_soundClip->acquireStream(m_streamId);
			alSourceQueueBuffers(m_source, BUFFER_NUM, m_soundClip->getBuffers(m_streamId));

			if (state == AL_PLAYING) {
				alSourcePlay(m_source);
			}

			CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error setting stream cursor position")
		}
	}

	float SoundEmitter::getCursor(SoundPositionType type) {
		if (!m_soundClip) {
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

		if (m_soundClip->isStream()) {
			pos += m_soundClip->getStreamPos(m_streamId, type);
		}

		CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error getting cursor")

		return pos;
	}

	void SoundEmitter::setPosition(const ExactModelCoordinate& position) {
		alSource3f(m_source, AL_POSITION, static_cast<ALfloat>(position.x), static_cast<ALfloat>(position.y), static_cast<ALfloat>(position.z));
	}

	ExactModelCoordinate SoundEmitter::getPosition() const {
		ALfloat vec[3];
		alGetSourcefv(m_source, AL_POSITION, vec);
		return ExactModelCoordinate(vec[0], vec[1], vec[2]);
	}

	void SoundEmitter::setReferenceDistance(float distance) {
		alSourcef(m_source, AL_REFERENCE_DISTANCE, distance);
	}

	float SoundEmitter::getReferenceDistance() const {
		float distance;
		alGetSourcef(m_source, AL_REFERENCE_DISTANCE, &distance);
		return distance;
	}

	void SoundEmitter::setMaxDistance(float distance) {
		alSourcef(m_source, AL_MAX_DISTANCE, distance);
	}

	float SoundEmitter::getMaxDistance() const {
		float distance;
		alGetSourcef(m_source, AL_MAX_DISTANCE, &distance);
		return distance;
	}

	void SoundEmitter::setVelocity(const ExactModelCoordinate& velocity) {
		alSource3f(m_source, AL_VELOCITY, static_cast<ALfloat>(velocity.x), static_cast<ALfloat>(velocity.y), static_cast<ALfloat>(velocity.z));
	}

	ExactModelCoordinate SoundEmitter::getVelocity() const {
		ALfloat vec[3];
		alGetSourcefv(m_source, AL_VELOCITY, vec);
		return ExactModelCoordinate(vec[0], vec[1], vec[2]);
	}

	void SoundEmitter::setConeInnerAngle(float inner) {
		alSourcef(m_source, AL_CONE_INNER_ANGLE, inner);
	}

	float SoundEmitter::getConeInnerAngle() const {
		float inner;
		alGetSourcef(m_source, AL_CONE_INNER_ANGLE, &inner);
		return inner;
	}

	void SoundEmitter::setConeOuterAngle(float outer) {
		alSourcef(m_source, AL_CONE_OUTER_ANGLE, outer);
	}

	float SoundEmitter::getConeOuterAngle() const {
		float outer;
		alGetSourcef(m_source, AL_CONE_OUTER_ANGLE, &outer);
		return outer;
	}

	void SoundEmitter::setConeOuterGain(float gain) {
		alSourcef(m_source, AL_CONE_OUTER_GAIN, gain);
	}

	float SoundEmitter::getConeOuterGain() const {
		float gain;
		alGetSourcef(m_source, AL_CONE_OUTER_GAIN, &gain);
		return gain;
	}

	SoundStateType SoundEmitter::getState() {
		ALint state;
		alGetSourcei(m_source, AL_SOURCE_STATE, &state);
		switch(state) {
			case AL_INITIAL:
				return SD_INITIAL_STATE;
				break;
			case AL_PLAYING:
				return SD_PLAYING_STATE;
				break;
			case AL_PAUSED:
				return SD_PAUSED_STATE;
				break;
			case AL_STOPPED:
				return SD_STOPPED_STATE;
				break;
			default:
				return SD_UNKNOWN_STATE;
				break;
		}
	}

	void SoundEmitter::addListener(SoundEmitterListener* listener) {
		m_listeners.push_back(listener);
	}

	void SoundEmitter::removeListener(SoundEmitterListener* listener) {
		std::vector<SoundEmitterListener*>::iterator i = m_listeners.begin();
		while (i != m_listeners.end()) {
			if ((*i) == listener) {
				*i = NULL;
				return;
			}
			++i;
		}

		FL_WARN(_log, "Cannot remove unknown listener");
	}

	void SoundEmitter::callOnSoundFinished() {
		std::vector<SoundEmitterListener*>::iterator i = m_listeners.begin();
		for (; i != m_listeners.end(); ++i) {
			if (*i) {
				(*i)->onSoundFinished(m_emitterId, m_soundClipId);
			}
		}
		m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), (SoundEmitterListener*)NULL), m_listeners.end());
	}
}
