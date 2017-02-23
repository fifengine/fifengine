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
#include "vfs/vfs.h"
#include "util/log/logger.h"
#include "util/base/exception.h"

#include "soundclipmanager.h"
#include "soundmanager.h"
#include "soundemitter.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_AUDIO);

	SoundManager::SoundManager() :
		m_context(0),
		m_device(0),
		m_muteVol(0),
		m_volume(1.0),
		m_maxDistance(10.0),
		m_distanceModel(SD_DISTANCE_INVERSE_CLAMPED),
		m_state(SM_STATE_INACTIV),
		m_sources(),
		m_createdSources(0) {
	}

	SoundManager::~SoundManager() {
		// delete all soundemitters
		for (std::vector<SoundEmitter*>::iterator it = m_emitterVec.begin(); it != m_emitterVec.end();  ++it) {
			if ((*it) != NULL) {
				delete (*it);
			}
		}
		m_emitterVec.clear();
		// delete all sources
		alDeleteSources(m_createdSources, m_sources);

		if (m_device) {
			alcDestroyContext(m_context);
			alcCloseDevice(m_device);
			m_device = NULL;
		}

		if (alcGetError(NULL) != ALC_NO_ERROR) {
			FL_ERR(_log, LMsg() << "error closing openal device");
		}
	}

	void SoundManager::init() {
		m_device = alcOpenDevice(NULL);

		if (!m_device || alcGetError(m_device) != ALC_NO_ERROR) {
			FL_ERR(_log, LMsg() << "Could not open audio device - deactivating audio module");
			m_device = NULL;
			return;
		}

		m_context = alcCreateContext(m_device, NULL);
		if (!m_context || alcGetError(m_device) != ALC_NO_ERROR) {
			FL_ERR(_log, LMsg() << "Couldn't create audio context - deactivating audio module");
			m_device = NULL;
			return;
		}

		alcMakeContextCurrent(m_context);
		if (alcGetError(m_device) != ALC_NO_ERROR) {
			FL_ERR(_log, LMsg() << "Couldn't change current audio context - deactivating audio module");
			m_device = NULL;
			return;
		}

		// set listener position
		alListener3f(AL_POSITION, 0.0, 0.0, 0.0);
		ALfloat vec1[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 1.0};
		alListenerfv(AL_ORIENTATION, vec1);

		// set volume
		alListenerf(AL_GAIN, m_volume);

		for (uint16_t i = 0; i < MAX_SOURCES; i++) {
			alGenSources(1, &m_sources[i]);
			if (alGetError() != AL_NO_ERROR) {
				break;
			}
			
			m_freeSources.push(m_sources[i]);
			m_createdSources++;
		}
		m_state = SM_STATE_PLAY;
	}

	bool SoundManager::isActive() const {
		return m_state != SM_STATE_INACTIV;
	}

	ALCcontext* SoundManager::getContext() const {
		return m_context;
	}

	void SoundManager::setVolume(float vol) {
		m_volume = vol;
		m_muteVol = vol;
		if (isActive()) {
			alListenerf(AL_GAIN, vol);
		}
	}

	float SoundManager::getVolume() const {
		return m_volume;
	}

	void SoundManager::mute() {
		if (isActive()) {
			alGetListenerf(AL_GAIN, &m_muteVol);
			alListenerf(AL_GAIN, 0);
		}
	}

	void SoundManager::unmute() {
		if (isActive()) {
			alListenerf(AL_GAIN, m_muteVol);
		}
	}

	void SoundManager::play() {
		m_state = SM_STATE_PLAY;
		for (std::vector<SoundEmitter*>::iterator it = m_emitterVec.begin(); it != m_emitterVec.end(); ++it) {
			SoundEmitter* emitter = (*it);
			if (!emitter) {
				continue;
			}
			emitter->play();
		}
	}

	void SoundManager::pause() {
		m_state = SM_STATE_PAUSE;
		for (std::vector<SoundEmitter*>::iterator it = m_emitterVec.begin(); it != m_emitterVec.end(); ++it) {
			SoundEmitter* emitter = (*it);
			if (!emitter) {
				continue;
			}
			emitter->pause();
		}
	}

	void SoundManager::stop() {
		m_state = SM_STATE_STOP;
		for (std::vector<SoundEmitter*>::iterator it = m_emitterVec.begin(); it != m_emitterVec.end(); ++it) {
			SoundEmitter* emitter = (*it);
			if (!emitter) {
				continue;
			}
			emitter->stop();
		}
	}

	void SoundManager::rewind() {
		for (std::vector<SoundEmitter*>::iterator it = m_emitterVec.begin(); it != m_emitterVec.end(); ++it) {
			SoundEmitter* emitter = (*it);
			if (!emitter) {
				continue;
			}
			emitter->rewind();
		}
	}

	void SoundManager::setDistanceModel(SoundDistanceModelType model) {
		m_distanceModel = model;
		if (!isActive()) {
			return;
		}

		switch (m_distanceModel) {
		case SD_DISTANCE_NONE:
			alDistanceModel(AL_NONE);
			break;
		case SD_DISTANCE_INVERSE:
			alDistanceModel(AL_INVERSE_DISTANCE);
			break;
		case SD_DISTANCE_INVERSE_CLAMPED:
			alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
			break;
		case SD_DISTANCE_LINEAR:
			alDistanceModel(AL_LINEAR_DISTANCE);
			break;
		case SD_DISTANCE_LINEAR_CLAMPED:
			alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
			break;
		case SD_DISTANCE_EXPONENT:
			alDistanceModel(AL_EXPONENT_DISTANCE);
			break;
		case SD_DISTANCE_EXPONENT_CLAMPED:
			alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);
			break;
		default:
			break;
		}
	}

	SoundDistanceModelType SoundManager::getDistanceModel() const {
		return m_distanceModel;
	}

	void SoundManager::setListenerPosition(const AudioSpaceCoordinate& position) {
		if (isActive()) {
			alListener3f(AL_POSITION, static_cast<ALfloat>(position.x), static_cast<ALfloat>(position.y), static_cast<ALfloat>(position.z));
		}
	}

	AudioSpaceCoordinate SoundManager::getListenerPosition() const {
		if (isActive()) {
			ALfloat vec[3];
			alGetListenerfv(AL_POSITION, vec);
			return AudioSpaceCoordinate(vec[0], vec[1], vec[2]);
		}
		return AudioSpaceCoordinate();
	}

	void SoundManager::setListenerOrientation(const AudioSpaceCoordinate& orientation) {
		if (isActive()) {
			ALfloat vec[6] = { static_cast<ALfloat>(orientation.x), static_cast<ALfloat>(orientation.y), static_cast<ALfloat>(orientation.z),
				0.0, 0.0, 1.0 };
			alListenerfv(AL_ORIENTATION, vec);
		}
	}

	AudioSpaceCoordinate SoundManager::getListenerOrientation() const {
		if (isActive()) {
			ALfloat vec[6];
			alGetListenerfv(AL_ORIENTATION, vec);
			return AudioSpaceCoordinate(vec[0], vec[1], vec[2]);
		}
		return AudioSpaceCoordinate();
	}

	void SoundManager::setListenerVelocity(const AudioSpaceCoordinate& velocity) {
		if (isActive()) {
			alListener3f(AL_VELOCITY, static_cast<ALfloat>(velocity.x), static_cast<ALfloat>(velocity.y), static_cast<ALfloat>(velocity.z));
		}
	}

	AudioSpaceCoordinate SoundManager::getListenerVelocity() const {
		if (isActive()) {
			ALfloat vec[3];
			alGetListenerfv(AL_VELOCITY, vec);
			return AudioSpaceCoordinate(vec[0], vec[1], vec[2]);
		}
		return AudioSpaceCoordinate();
	}

	void SoundManager::setDopplerFactor(float factor) {
		if (isActive()) {
			if (factor >= 0.0) {
				alDopplerFactor(factor);
			}
		}
	}

	float SoundManager::getDopplerFactor() const {
		if (isActive()) {
			return alGetFloat(AL_DOPPLER_FACTOR);
		}
		return 0.0;
	}

	void SoundManager::setListenerMaxDistance(float distance) {
		m_maxDistance = distance;
	}

	float SoundManager::getListenerMaxDistance() const {
		return m_maxDistance;
	}

	void SoundManager::update() {
		if (m_state != SM_STATE_PLAY) {
			return;
		}
		AudioSpaceCoordinate listenerPos = getListenerPosition();
		double maxDistance = static_cast<double>(m_maxDistance);

		// first check emitters
		for (std::vector<SoundEmitter*>::iterator it = m_emitterVec.begin(); it != m_emitterVec.end(); ++it) {
			SoundEmitter* emitter = (*it);
			if (!emitter) {
				continue;
			}

			bool active = emitter->isActive();
			bool clip = emitter->getSoundClip();
			// if emitter is in playing state and it's looping or have not reached the end
			bool plays = (emitter->getState() == SD_PLAYING_STATE) ? (emitter->isLooping() || !emitter->isEndTimestamp()) : false;

			// remove active without clip or stopped
			if (!clip || !plays) {
				if (active) {
					releaseSource(emitter);
				}
				continue;
			}

			bool inRange = true;
			if (emitter->isPosition()) {
				AudioSpaceCoordinate emitterPos = emitter->getPosition();
				double rx = listenerPos.x - emitterPos.x;
				double ry = listenerPos.y - emitterPos.y;
				double rz = listenerPos.z - emitterPos.z;
				inRange = maxDistance >= Mathd::Sqrt(rx*rx + ry*ry + rz*rz);
			}
			// remove active not in range
			if (!inRange) {
				if (active) {
					releaseSource(emitter);
				}
				continue;
			}
			if (!active && !m_freeSources.empty()) {
				setEmitterSource(emitter);
			}
		}
		// then update active
		for (std::map<SoundEmitter*, ALuint>::iterator it = m_activeEmitters.begin(); it != m_activeEmitters.end(); ++it) {
			it->first->update();
		}
	}

	SoundEmitter* SoundManager::getEmitter(uint32_t emitterId) const {
		return m_emitterVec.at(emitterId);
	}

	SoundEmitter* SoundManager::createEmitter() {
		SoundEmitter* ptr = NULL;
		for (uint32_t i = 0; i < m_emitterVec.size(); i++) {
			if (m_emitterVec.at(i) == NULL) {
				ptr = new SoundEmitter(this, i);
				m_emitterVec.at(i) = ptr;
				break;
			}
		}
		if (!ptr) {
			ptr = new SoundEmitter(this, m_emitterVec.size());
			m_emitterVec.push_back(ptr);
		}
		return ptr;
	}

	SoundEmitter* SoundManager::createEmitter(const std::string& name) {
		SoundEmitter* emitter = createEmitter();
		emitter->setSoundClip(SoundClipManager::instance()->get(name));
		return emitter;
	}

	void SoundManager::releaseEmitter(uint32_t emitterId) {
		SoundEmitter** ptr = &m_emitterVec.at(emitterId);
		if ((*ptr)->isActive()) {
			releaseSource(*ptr);
		}
		delete *ptr;
		*ptr = NULL;
	}

	void SoundManager::deleteEmitter(SoundEmitter* emitter) {
		releaseEmitter(emitter->getId());
	}

	void SoundManager::setEmitterSource(SoundEmitter* emitter) {
		std::pair<std::map<SoundEmitter*, ALuint>::iterator, bool> ret;
		ret = m_activeEmitters.insert(std::pair<SoundEmitter*, ALuint>(emitter, m_freeSources.front()));
		if (ret.second == false) {
			FL_ERR(_log, LMsg() << "SoundEmitter already have an source handler");
		}
		emitter->setSource(m_freeSources.front());
		m_freeSources.pop();
	}

	void SoundManager::releaseSource(SoundEmitter* emitter) {
		if (emitter->isActive()) {
			std::map<SoundEmitter*, ALuint>::iterator it = m_activeEmitters.find(emitter);
			if (it != m_activeEmitters.end()) {
				m_freeSources.push(it->second);
				m_activeEmitters.erase(it);
				emitter->setSource(0);
			} else {
				FL_ERR(_log, LMsg() << "SoundEmitter can not release source handler");
			}
		}
	}

	void SoundManager::addToGroup(SoundEmitter* emitter) {
		if (emitter->getGroup() != "") {
			m_groups[emitter->getGroup()].push_back(emitter);
		}
	}

	void SoundManager::removeFromGroup(SoundEmitter* emitter) {
		std::string group = emitter->getGroup();
		if (group == "") {
			return;
		}
		std::map<std::string, std::vector<SoundEmitter*>>::iterator groupIt = m_groups.find(group);
		if (groupIt == m_groups.end()) {
			FL_ERR(_log, LMsg() << "SoundEmitter can not removed from unknown group");
			return;
		}
		std::vector<SoundEmitter*>::iterator emitterIt = groupIt->second.begin();
		bool found = false;
		while (emitterIt != groupIt->second.end()) {
			if ((*emitterIt) == emitter) {
				groupIt->second.erase(emitterIt++);
				found = true;
			} else {
				++emitterIt;
			}
		}
		if (!found) {
			FL_ERR(_log, LMsg() << "SoundEmitter could not be found in the given group.");
			return;
		}
	}

	void SoundManager::removeGroup(const std::string& group) {
		if (group == "") {
			return;
		}
		std::map<std::string, std::vector<SoundEmitter*>>::iterator groupIt = m_groups.find(group);
		if (groupIt == m_groups.end()) {
			FL_ERR(_log, LMsg() << "SoundEmitter can not remove unknown group");
			return;
		}
		std::vector<SoundEmitter*> emitters = groupIt->second;
		for (std::vector<SoundEmitter*>::iterator it = emitters.begin(); it != emitters.end(); ++it) {
			(*it)->setGroup("");
		}
		m_groups.erase(group);
	}

	void SoundManager::removeAllGroups() {
		std::vector<std::string> groups;
		for (std::map<std::string, std::vector<SoundEmitter*>>::iterator it = m_groups.begin(); it != m_groups.end(); ++it) {
			groups.push_back(it->first);
		}
		for (std::vector<std::string>::iterator it = groups.begin(); it != groups.end(); ++it) {
			removeGroup(*it);
		}
		m_groups.clear();
	}

	void SoundManager::play(const std::string& group) {
		std::map<std::string, std::vector<SoundEmitter*>>::iterator groupIt = m_groups.find(group);
		if (groupIt == m_groups.end()) {
			FL_ERR(_log, LMsg() << "Unknown group can not played");
			return;
		}
		std::vector<SoundEmitter*>::iterator emitterIt = groupIt->second.begin();
		for (; emitterIt != groupIt->second.end(); ++emitterIt) {
			(*emitterIt)->play();
		}
	}

	void SoundManager::pause(const std::string& group) {
		std::map<std::string, std::vector<SoundEmitter*>>::iterator groupIt = m_groups.find(group);
		if (groupIt == m_groups.end()) {
			FL_ERR(_log, LMsg() << "Unknown group can not paused");
			return;
		}
		std::vector<SoundEmitter*>::iterator emitterIt = groupIt->second.begin();
		for (; emitterIt != groupIt->second.end(); ++emitterIt) {
			(*emitterIt)->pause();
		}
	}

	void SoundManager::stop(const std::string& group) {
		std::map<std::string, std::vector<SoundEmitter*>>::iterator groupIt = m_groups.find(group);
		if (groupIt == m_groups.end()) {
			FL_ERR(_log, LMsg() << "Unknown group can not stopped");
			return;
		}
		std::vector<SoundEmitter*>::iterator emitterIt = groupIt->second.begin();
		for (; emitterIt != groupIt->second.end(); ++emitterIt) {
			(*emitterIt)->stop();
		}
	}

	void SoundManager::rewind(const std::string& group) {
		std::map<std::string, std::vector<SoundEmitter*>>::iterator groupIt = m_groups.find(group);
		if (groupIt == m_groups.end()) {
			FL_ERR(_log, LMsg() << "Unknown group can not rewinded");
			return;
		}
		std::vector<SoundEmitter*>::iterator emitterIt = groupIt->second.begin();
		for (; emitterIt != groupIt->second.end(); ++emitterIt) {
			(*emitterIt)->rewind();
		}
	}

	void SoundManager::setGain(const std::string& group, float gain) {
		std::map<std::string, std::vector<SoundEmitter*>>::iterator groupIt = m_groups.find(group);
		if (groupIt == m_groups.end()) {
			FL_ERR(_log, LMsg() << "Unknown group can not set gain");
			return;
		}
		std::vector<SoundEmitter*>::iterator emitterIt = groupIt->second.begin();
		for (; emitterIt != groupIt->second.end(); ++emitterIt) {
			(*emitterIt)->setGain(gain);
		}
	}

	void SoundManager::setMaxGain(const std::string& group, float gain) {
		std::map<std::string, std::vector<SoundEmitter*>>::iterator groupIt = m_groups.find(group);
		if (groupIt == m_groups.end()) {
			FL_ERR(_log, LMsg() << "Unknown group can not set max gain");
			return;
		}
		std::vector<SoundEmitter*>::iterator emitterIt = groupIt->second.begin();
		for (; emitterIt != groupIt->second.end(); ++emitterIt) {
			(*emitterIt)->setMaxGain(gain);
		}
	}

	void SoundManager::setMinGain(const std::string& group, float gain) {
		std::map<std::string, std::vector<SoundEmitter*>>::iterator groupIt = m_groups.find(group);
		if (groupIt == m_groups.end()) {
			FL_ERR(_log, LMsg() << "Unknown group can not set min gain");
			return;
		}
		std::vector<SoundEmitter*>::iterator emitterIt = groupIt->second.begin();
		for (; emitterIt != groupIt->second.end(); ++emitterIt) {
			(*emitterIt)->setMinGain(gain);
		}
	}

} //FIFE
