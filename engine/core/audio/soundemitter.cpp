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
#include "util/time/timemanager.h"

#include "soundemitter.h"
#include "soundmanager.h"
#include "soundclipmanager.h"

namespace FIFE {
	static Logger _log(LM_AUDIO);

	SoundEmitter::SoundEmitter(SoundManager* manager, uint32_t uid) :
		m_manager(manager),
		m_source(0),
		m_directFilter(NULL),
		m_soundClip(),
		m_soundClipId(0),
		m_streamId(0),
		m_emitterId(uid),
		m_group(""),
		m_samplesOffset(0),
		m_active(false),
		m_fadeIn(false),
		m_fadeOut(false),
		m_origGain(0.0f),
		m_fadeInStartTimestamp(0),
		m_fadeInEndTimestamp(0),
		m_fadeOutStartTimestamp(0),
		m_fadeOutEndTimestamp(0) {

		if (!m_manager->isActive()) {
			return;
		}

		resetInternData();
	}

	SoundEmitter::~SoundEmitter() {
		if (!m_manager->isActive()) {
			return;
		}

		reset();
	}

	void SoundEmitter::setSource(ALuint source) {
		if (!source && m_source) {
			alSourceStop(m_source);

			// Release all buffers
			alSourcei(m_source, AL_BUFFER, AL_NONE);
			alGetError();

			deactivateEffects();
		}
		m_source = source;
		if (m_source > 0) {
			m_active = true;
			activateEffects();
			syncData();
		} else {
			m_active = false;
		}
	}

	ALuint SoundEmitter::getSource() const {
		return m_source;
	}

	bool SoundEmitter::isActive() const {
		return m_active;
	}

	void SoundEmitter::update() {
		if (!m_soundClip || m_internData.soundState == SD_STOPPED_STATE) {
			return;
		}
		if (m_fadeIn || m_fadeOut) {
			checkFade();
		}
		// non streaming
		if (!m_soundClip->isStream()) {
			if (getState() == SD_STOPPED_STATE) {
				stop();
			}
			return;
		}

		ALint procs;
		ALint bufs;
		ALuint buffer;
		alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &procs);

		while (procs--) {
			// needed for correct cursor position
			float samplesOffset, newOffset;
			alGetSourcef(m_source, AL_SAMPLE_OFFSET, &samplesOffset);

			alSourceUnqueueBuffers(m_source, 1, &buffer);

			alGetSourcef(m_source, AL_SAMPLE_OFFSET, &newOffset);
			m_samplesOffset += (samplesOffset - newOffset);

			if (m_soundClip->getStream(m_streamId, buffer)) {
				// EOF!
				if (m_internData.loop) {
					// play again from the beginning
					m_soundClip->setStreamPos(m_streamId, SD_BYTE_POS, 0);
					m_soundClip->getStream(m_streamId, buffer);
				} else {
					// check if the playback has been finished
					alGetSourcei(m_source, AL_BUFFERS_QUEUED, &bufs);
					if (bufs == 0) {
						stop();
					}
					continue;
				}
			}
			alSourceQueueBuffers(m_source, 1, &buffer);
		}

		CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error while streaming")
	}

	uint32_t SoundEmitter::getId() const {
		return m_emitterId;
	}

	void SoundEmitter::setRelativePositioning(bool relative) {
		if (isActive()) {
			alSourcei(m_source, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
		}
		m_internData.relative = relative;
	}

	bool SoundEmitter::isRelativePositioning() const {
		return m_internData.relative;
	}

	void SoundEmitter::setDirection(const AudioSpaceCoordinate& direction) {
		if (isActive()) {
			ALfloat vec[3] = { static_cast<ALfloat>(direction.x), static_cast<ALfloat>(direction.y), static_cast<ALfloat>(direction.z) };
			alSourcefv(m_source, AL_DIRECTION, vec);
		}
		m_internData.direction = direction;
	}

	AudioSpaceCoordinate SoundEmitter::getDirection() const {
		return m_internData.direction;
	}

	void SoundEmitter::setPitch(float pitch) {
		if (pitch > 0.0) {
			if (isActive()) {
				alSourcef(m_source, AL_PITCH, pitch);
			}
			m_internData.pitch = pitch;
		}
	}

	float SoundEmitter::getPitch() const {
		return m_internData.pitch;
	}

	void SoundEmitter::setRolloff(float rolloff) {
		if (isActive()) {
			alSourcef(m_source, AL_ROLLOFF_FACTOR, rolloff);
		}
		m_internData.rolloff = rolloff;
	}

	float SoundEmitter::getRolloff() const {
		return m_internData.rolloff;
	}

	void SoundEmitter::reset(bool defaultall) {
		// remove effects and filter
		if (m_directFilter) {
			m_manager->deactivateFilter(m_directFilter, this);
		}
		std::vector<SoundEffect*>effects = m_effects;
		for (std::vector<SoundEffect*>::iterator it = effects.begin(); it != effects.end(); ++it) {
			if (*it) {
				m_manager->removeEmitterFromSoundEffect(*it, this);
			}
		}
		// release buffer and source handle
		if (isActive()) {
			alSourceStop(m_source);
			alSourcei(m_source, AL_BUFFER, AL_NONE);
			alGetError();
			m_manager->releaseSource(this);
		}
		// reset clip
		if (m_soundClip) {
			if (m_soundClip->isStream()) {
				m_soundClip->quitStreaming(m_streamId);
				m_streamId = 0;
			}
			m_soundClipId = 0;
			// release the soundClip
			//SoundClipManager::instance()->free(m_soundClipId);
			m_soundClip.reset();
		}

		// default source properties
		if (defaultall) {
			resetInternData();
			if (isActive()) {
				syncData();
			}
		}

		if (m_group != "") {
			m_manager->removeFromGroup(this);
			m_group = "";
		}
		m_samplesOffset = 0;
		m_fadeIn = false;
		m_fadeOut = false;
		m_origGain = 0;
	}

	void SoundEmitter::release() {
		m_manager->releaseEmitter(m_emitterId);
	}

	void SoundEmitter::setSoundClip(SoundClipPtr soundClip) {
		// equal clip
		if (m_soundClipId == soundClip->getHandle()) {
			return;
		}

		detachSoundClip();
		m_soundClipId = soundClip->getHandle();
		m_soundClip = soundClip;

		attachSoundClip();
	}

	SoundClipPtr SoundEmitter::getSoundClip() {
		return m_soundClip;
	}

	void SoundEmitter::setSoundClip(const std::string& name) {
		SoundClipPtr clip = SoundClipManager::instance()->get(name);
		setSoundClip(clip);
	}

	void SoundEmitter::attachSoundClip() {
		if (!m_soundClip->isStream()) {
			if (!isActive()) {
				return;
			}
			// non-streaming
			alSourceQueueBuffers(m_source, m_soundClip->countBuffers(), m_soundClip->getBuffers());
			alSourcei(m_source, AL_LOOPING, m_internData.loop ? AL_TRUE : AL_FALSE);

		} else {
			// streaming
			m_streamId = m_soundClip->beginStreaming();
			m_soundClip->acquireStream(m_streamId);
			if (!isActive()) {
				return;
			}
			// queue initial buffers
			alSourceQueueBuffers(m_source, BUFFER_NUM, m_soundClip->getBuffers(m_streamId));
			alSourcei(m_source, AL_LOOPING, AL_FALSE);
		}

		CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error attaching sound clip")
	}

	void SoundEmitter::detachSoundClip() {
		if (!m_soundClip) {
			return;
		}
		SoundStateType state = getState();
		if (state == SD_PLAYING_STATE || state == SD_PAUSED_STATE) {
			stop();
		}
		if (isActive()) {
			// detach all buffers
			alSourcei(m_source, AL_BUFFER, AL_NONE);
			CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error detaching sound clip");
		}
		if (m_soundClip->isStream()) {
			m_soundClip->quitStreaming(m_streamId);
			m_streamId = 0;
		}
		m_soundClipId = 0;
		m_soundClip.reset();
	}

	void SoundEmitter::setLooping(bool loop) {
		if (m_soundClip && isActive()) {
			if (!m_soundClip->isStream()) {
				alSourcei(m_source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
			} else {
				alSourcei(m_source, AL_LOOPING, AL_FALSE);
			}
		}
		m_internData.loop = loop;
	}

	bool SoundEmitter::isLooping() const {
		return m_internData.loop;
	}

	void SoundEmitter::play() {
		if (m_soundClip && isActive()) {
			alSourcePlay(m_source);
		}
		m_internData.playTimestamp = TimeManager::instance()->getTime();
		// resume
		if (m_internData.soundState == SD_PAUSED_STATE) {
			m_internData.playTimestamp -= static_cast<uint32_t>(getCursor(SD_TIME_POS) * 1000);
		}
		m_internData.soundState = SD_PLAYING_STATE;
	}

	void SoundEmitter::play(float inTime, float outTime) {
		float zero = 0;
		m_origGain = m_internData.volume;
		if (!Mathf::Equal(zero, inTime)) {
			m_fadeIn = true;
			setGain(0.0f);
			play();
			m_fadeInStartTimestamp = m_internData.playTimestamp;
			m_fadeInEndTimestamp = m_fadeInStartTimestamp + static_cast<uint32_t>(inTime * 1000.0f);
		}
		if (getState() != SD_PLAYING_STATE) {
			play();
		}
		if (!Mathf::Equal(zero, outTime)) {
			m_fadeOut = true;
			setGain(0.0f);
			m_fadeOutEndTimestamp = m_internData.playTimestamp + getDuration();
			m_fadeOutStartTimestamp = m_fadeOutEndTimestamp - static_cast<uint32_t>(outTime * 1000.0f);
		}
	}

	void SoundEmitter::stop() {
		if (m_soundClip && isActive()) {
			alSourceStop(m_source);
			rewind();
		}
		m_internData.soundState = SD_STOPPED_STATE;
		m_internData.playTimestamp = 0;
		callOnSoundFinished();
	}

	void SoundEmitter::stop(float time) {
		m_fadeOut = true;
		m_origGain = m_internData.volume;
		m_fadeOutStartTimestamp = TimeManager::instance()->getTime();
		m_fadeOutEndTimestamp = m_fadeOutStartTimestamp + static_cast<uint32_t>(time * 1000.0f);
	}

	void SoundEmitter::pause() {
		if (m_soundClip && isActive()) {
			alSourcePause(m_source);
		}
		m_internData.soundState = SD_PAUSED_STATE;
	}

	void SoundEmitter::rewind() {
		m_internData.playTimestamp = 0;
		m_samplesOffset = 0;
		if (!isActive() || !m_soundClip) {
			return;
		}
		if (m_soundClip->isStream()) {
			setCursor(SD_BYTE_POS, 0);
		} else {
			alSourceRewind(m_source);
		}
	}

	void SoundEmitter::setGain(float gain) {
		if (isActive()) {
			alSourcef(m_source, AL_GAIN, gain);
		}
		m_internData.volume = gain;
	}

	float SoundEmitter::getGain() const {
		return m_internData.volume;
	}

	void SoundEmitter::setMaxGain(float gain) {
		if (isActive()) {
			alSourcef(m_source, AL_MAX_GAIN, gain);
		}
		m_internData.maxVolume = gain;
	}

	float SoundEmitter::getMaxGain() const {
		return m_internData.maxVolume;
	}

	void SoundEmitter::setMinGain(float gain) {
		if (isActive()) {
			alSourcef(m_source, AL_MIN_GAIN, gain);
		}
		m_internData.minVolume = gain;
	}

	float SoundEmitter::getMinGain() const {
		return m_internData.minVolume;
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

	uint32_t SoundEmitter::getPlayTimestamp() {
		return m_internData.playTimestamp;
	}

	bool SoundEmitter::isFinished() {
		if (isLooping()) {
			return false;
		}
		if (isActive()) {
			return getState() == SD_STOPPED_STATE;
		}
		if (getState() == SD_STOPPED_STATE) {
			return true;
		}
		// roughly check, in the case the clip do not plays (is not active)
		return (m_internData.playTimestamp + static_cast<uint32_t>(getDuration())) <= TimeManager::instance()->getTime();
	}

	void SoundEmitter::setCursor(SoundPositionType type, float value) {
		if (!m_soundClip || !isActive()) {
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
			switch (type) {
			case SD_BYTE_POS:
				m_samplesOffset = value / (getBitResolution() / 8 * (isStereo() ? 2 : 1));
				break;
			case SD_SAMPLE_POS:
				m_samplesOffset = value;
				break;
			case SD_TIME_POS:
				m_samplesOffset = value * getSampleRate();
				break;
			}
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
		if (!m_soundClip || !isActive()) {
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
			switch (type) {
			case SD_BYTE_POS:
				pos += m_samplesOffset * (getBitResolution() / 8 * (isStereo() ? 2 : 1));
				break;
			case SD_SAMPLE_POS:
				pos += m_samplesOffset;
				break;
			case SD_TIME_POS:
				pos += m_samplesOffset / getSampleRate();
				break;
			}
		}

		CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error getting cursor")

		return pos;
	}

	void SoundEmitter::setPosition(const AudioSpaceCoordinate& position) {
		if (isActive()) {
			alSource3f(m_source, AL_POSITION, static_cast<ALfloat>(position.x), static_cast<ALfloat>(position.y), static_cast<ALfloat>(position.z));
		}
		m_internData.position = position;
	}

	AudioSpaceCoordinate SoundEmitter::getPosition() const {
		return m_internData.position;
	}

	bool SoundEmitter::isPosition() const {
		double zero = 0;
		return !(Mathd::Equal(zero, m_internData.position.x) && Mathd::Equal(zero, m_internData.position.y) && Mathd::Equal(zero, m_internData.position.z));
	}

	void SoundEmitter::setReferenceDistance(float distance) {
		if (isActive()) {
			alSourcef(m_source, AL_REFERENCE_DISTANCE, distance);
		}
		m_internData.refDistance = distance;
	}

	float SoundEmitter::getReferenceDistance() const {
		return m_internData.refDistance;
	}

	void SoundEmitter::setMaxDistance(float distance) {
		if (isActive()) {
			alSourcef(m_source, AL_MAX_DISTANCE, distance);
		}
		m_internData.maxDistance = distance;
	}

	float SoundEmitter::getMaxDistance() const {
		return m_internData.maxDistance;
	}

	void SoundEmitter::setVelocity(const AudioSpaceCoordinate& velocity) {
		if (isActive()) {
			alSource3f(m_source, AL_VELOCITY, static_cast<ALfloat>(velocity.x), static_cast<ALfloat>(velocity.y), static_cast<ALfloat>(velocity.z));
		}
		m_internData.velocity = velocity;
	}

	AudioSpaceCoordinate SoundEmitter::getVelocity() const {
		return m_internData.velocity;
	}

	void SoundEmitter::setConeInnerAngle(float inner) {
		if (isActive()) {
			alSourcef(m_source, AL_CONE_INNER_ANGLE, inner);
		}
		m_internData.coneInnerAngle = inner;
	}

	float SoundEmitter::getConeInnerAngle() const {
		return m_internData.coneInnerAngle;
	}

	void SoundEmitter::setConeOuterAngle(float outer) {
		if (isActive()) {
			alSourcef(m_source, AL_CONE_OUTER_ANGLE, outer);
		}
		m_internData.coneOuterAngle = outer;
	}

	float SoundEmitter::getConeOuterAngle() const {
		return m_internData.coneOuterAngle;
	}

	void SoundEmitter::setConeOuterGain(float gain) {
		if (isActive()) {
			alSourcef(m_source, AL_CONE_OUTER_GAIN, gain);
		}
		m_internData.coneOuterGain = gain;
	}

	float SoundEmitter::getConeOuterGain() const {
		return m_internData.coneOuterGain;
	}

	SoundStateType SoundEmitter::getState() {
		if (!isActive()) {
			return m_internData.soundState;
		}
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

	void SoundEmitter::setGroup(const std::string& group) {
		if (group != m_group) {
			if (m_group != "") {
				m_manager->removeFromGroup(this);
			}
			m_group = group;
			if (m_group != "") {
				m_manager->addToGroup(this);
			}
		}
	}

	const std::string& SoundEmitter::getGroup() {
		return m_group;
	}

	void SoundEmitter::syncData() {
		setGain(m_internData.volume);
		setMaxGain(m_internData.maxVolume);
		setMinGain(m_internData.minVolume);
		setReferenceDistance(m_internData.refDistance);
		setMaxDistance(m_internData.maxDistance);
		setRolloff(m_internData.rolloff);
		setPitch(m_internData.pitch);
		setConeInnerAngle(m_internData.coneInnerAngle);
		setConeOuterAngle(m_internData.coneOuterAngle);
		setConeOuterGain(m_internData.coneOuterGain);
		setPosition(m_internData.position);
		setDirection(m_internData.direction);
		setVelocity(m_internData.velocity);
		setLooping(m_internData.loop);
		setRelativePositioning(m_internData.relative);
		if (m_internData.soundState == SD_PLAYING_STATE) {
			uint32_t timediff = TimeManager::instance()->getTime() - m_internData.playTimestamp;
			if (m_internData.loop) {
				timediff = timediff % getDuration();
			}
			float time = static_cast<float>(timediff) / 1000.0f;
			attachSoundClip();
			setCursor(SD_TIME_POS, time);
			if (m_soundClip && isActive()) {
				m_internData.playTimestamp = TimeManager::instance()->getTime() - timediff;
				alSourcePlay(m_source);
			}
		}
	}

	void SoundEmitter::resetInternData() {
		m_internData.volume = 1.0;
		m_internData.maxVolume = 1.0;
		m_internData.minVolume = 0.0;
		m_internData.refDistance = 1.0;
		m_internData.maxDistance = 1000000.0;
		m_internData.rolloff = 1.0;
		m_internData.pitch = 1.0;
		m_internData.coneInnerAngle = 360.0;
		m_internData.coneOuterAngle = 360.0;
		m_internData.coneOuterGain = 0.0;
		m_internData.position = AudioSpaceCoordinate(0.0, 0.0, 0.0);
		m_internData.direction = AudioSpaceCoordinate(0.0, 0.0, 0.0);
		m_internData.velocity = AudioSpaceCoordinate(0.0, 0.0, 0.0);
		m_internData.playTimestamp = 0;
		m_internData.soundState = SD_UNKNOWN_STATE;
		m_internData.loop = false;
		m_internData.relative = false;
	}

	void SoundEmitter::checkFade() {
		uint32_t timestamp = TimeManager::instance()->getTime();
		if (m_fadeIn) {
			float delta = m_origGain / static_cast<float>(m_fadeInEndTimestamp - m_fadeInStartTimestamp);
			if (timestamp >= m_fadeInEndTimestamp) {
				m_fadeIn = false;
				setGain(m_origGain);
			} else {
				float gain = delta * static_cast<float>(timestamp - m_fadeInStartTimestamp);
				gain = std::min(gain, m_origGain);
				setGain(gain);
			}
		} else if (m_fadeOut) {
			float delta = m_origGain / static_cast<float>(m_fadeOutEndTimestamp - m_fadeOutStartTimestamp);
			if (timestamp >= m_fadeOutEndTimestamp) {
				m_fadeOut = false;
				stop();
				setGain(m_origGain);
			} else {
				float gain = delta * static_cast<float>(m_fadeOutEndTimestamp - timestamp);
				gain = std::max(gain, 0.0f);
				setGain(gain);
			}
		}
	}

	void SoundEmitter::addEffect(SoundEffect* effect) {
		bool added = false;
		for (std::vector<SoundEffect*>::iterator it = m_effects.begin(); it != m_effects.end(); ++it) {
			if (!(*it)) {
				(*it) = effect;
				added = true;
				break;
			}
		}
		if (!added) {
			m_effects.push_back(effect);
		}
	}

	void SoundEmitter::removeEffect(SoundEffect* effect) {
		for (std::vector<SoundEffect*>::iterator it = m_effects.begin(); it != m_effects.end(); ++it) {
			if (effect == *it) {
				(*it) = NULL;
				break;
			}
		}
	}

	uint8_t SoundEmitter::getEffectCount() {
		uint8_t counter = 0;
		for (std::vector<SoundEffect*>::iterator it = m_effects.begin(); it != m_effects.end(); ++it) {
			if (*it) {
				++counter;
			}
		}
		return counter;
	}

	uint8_t SoundEmitter::getEffectNumber(SoundEffect* effect) {
		uint8_t number = 0;
		for (std::vector<SoundEffect*>::iterator it = m_effects.begin(); it != m_effects.end(); ++it) {
			if (effect == *it) {
				break;
			}
			++number;
		}
		return number;
	}

	void SoundEmitter::setDirectFilter(SoundFilter* filter) {
		m_directFilter = filter;
	}

	SoundFilter* SoundEmitter::getDirectFilter() {
		return m_directFilter;
	}

	void SoundEmitter::activateEffects() {
		for (std::vector<SoundEffect*>::iterator it = m_effects.begin(); it != m_effects.end(); ++it) {
			if (*it) {
				m_manager->activateEffect(*it, this);
			}
		}
		if (m_directFilter) {
			m_manager->activateFilter(m_directFilter, this);
		}
	}

	void SoundEmitter::deactivateEffects() {
		for (std::vector<SoundEffect*>::iterator it = m_effects.begin(); it != m_effects.end(); ++it) {
			if (*it) {
				m_manager->deactivateEffect(*it, this);
			}
		}
		if (m_directFilter) {
			m_manager->deactivateFilter(m_directFilter, this);
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
