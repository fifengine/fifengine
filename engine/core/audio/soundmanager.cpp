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
		m_sources(),
		m_createdSources(0) {
	}

	SoundManager::~SoundManager() {
		// free all soundemitters
		for (std::vector<SoundEmitter*>::iterator it = m_emitterVec.begin(), it_end = m_emitterVec.end(); it != it_end;  ++it) {
			if ((*it) != NULL) {
				delete (*it);
			}
		}

		m_emitterVec.clear();

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
	}

	bool SoundManager::isActive() const {
		return m_device != NULL;
	}

	void SoundManager::update() {
		ALfloat vec[3];
		alGetListenerfv(AL_POSITION, vec);
		AudioSpaceCoordinate listenerPos = AudioSpaceCoordinate(vec[0], vec[1], vec[2]);
		double maxDistance = static_cast<double>(m_maxDistance);

		// first try to remove
		for (std::vector<SoundEmitter*>::iterator it = m_emitterVec.begin(), it_end = m_emitterVec.end(); it != it_end;  ++it) {
			SoundEmitter* emitter = (*it);
			if (!emitter) {
				continue;
			}

			bool active = emitter->isActive();
			// remove active without clip
			if (!emitter->getSoundClip() && active) {
				releaseSource(emitter);
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
			if (emitter->getSoundClip()) {
				// remove active not in range
				if (active && !inRange) {
					releaseSource(emitter);
				// remove active with stopped sound
				} else if (active && emitter->getState() == SD_STOPPED_STATE) {
					releaseSource(emitter);
				// emitter that came into range
				} else if (!active && inRange && emitter->getState() == SD_PLAYING_STATE) {
					if (emitter->isLooping() || !emitter->isEndTimestamp()) {
						m_waitingEmitters.push_back(emitter);
					}
				}
			}
		}

		m_waitingEmitters.sort();
		m_waitingEmitters.unique();
		if (!m_freeSources.empty() && !m_waitingEmitters.empty()) {
			std::list<SoundEmitter*>::iterator it = m_waitingEmitters.begin();
			while (it != m_waitingEmitters.end()) {
				if (m_freeSources.empty()) {
					break;
				}
				requestSource(*it);
				m_waitingEmitters.erase(it++);
			}
		}
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
		// instant request to play the sound
		requestSource(emitter);
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

	ALCcontext* SoundManager::getContext() const {
		return m_context;
	}

	void SoundManager::setVolume(float vol) {
		if (m_device == NULL) {
			m_volume = vol;
		}
		alListenerf(AL_GAIN, vol);
	}

	float SoundManager::getVolume() const {
		return m_volume;
	}

	void SoundManager::mute() {
		alGetListenerf(AL_GAIN, &m_muteVol);
		alListenerf(AL_GAIN, 0);
	}

	void SoundManager::unmute() {
		alListenerf(AL_GAIN, m_muteVol);
	}

	void SoundManager::setDistanceModel(SoundDistanceModelType model) {
		m_distanceModel = model;

		switch(m_distanceModel) {
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
		alListener3f(AL_POSITION, static_cast<ALfloat>(position.x), static_cast<ALfloat>(position.y), static_cast<ALfloat>(position.z));
	}

	AudioSpaceCoordinate SoundManager::getListenerPosition() const {
		ALfloat vec[3];
		alGetListenerfv(AL_POSITION, vec);
		return AudioSpaceCoordinate(vec[0], vec[1], vec[2]);
	}

	void SoundManager::setListenerOrientation(const AudioSpaceCoordinate& orientation) {
		ALfloat vec[6] = { static_cast<ALfloat>(orientation.x), static_cast<ALfloat>(orientation.y), static_cast<ALfloat>(orientation.z),
			0.0, 0.0, 1.0};
		alListenerfv(AL_ORIENTATION, vec);
	}

	AudioSpaceCoordinate SoundManager::getListenerOrientation() const {
		ALfloat vec[6];
		alGetListenerfv(AL_ORIENTATION, vec);
		return AudioSpaceCoordinate(vec[0], vec[1], vec[2]);
	}

	void SoundManager::setListenerVelocity(const AudioSpaceCoordinate& velocity) {
		alListener3f(AL_VELOCITY, static_cast<ALfloat>(velocity.x), static_cast<ALfloat>(velocity.y), static_cast<ALfloat>(velocity.z));
	}

	AudioSpaceCoordinate SoundManager::getListenerVelocity() const {
		ALfloat vec[3];
		alGetListenerfv(AL_VELOCITY, vec);
		return AudioSpaceCoordinate(vec[0], vec[1], vec[2]);
	}

	void SoundManager::setDopplerFactor(float factor) {
		if (factor >= 0.0) {
			alDopplerFactor(factor);
		}
	}

	float SoundManager::getDopplerFactor() const {
		return alGetFloat(AL_DOPPLER_FACTOR);
	}

	void SoundManager::setListenerMaxDistance(float distance) {
		m_maxDistance = distance;
	}

	float SoundManager::getListenerMaxDistance() const {
		return m_maxDistance;
	}

	void SoundManager::requestSource(SoundEmitter* emitter) {
		if (!emitter->isActive() && !m_freeSources.empty()) {
			std::pair<std::map<SoundEmitter*, ALuint>::iterator, bool> ret;
			ret = m_activeEmitters.insert(std::pair<SoundEmitter*, ALuint>(emitter, m_freeSources.front()));
			if (ret.second == false) {
				FL_ERR(_log, LMsg() << "SoundEmitter already have an source handler");
			}
			emitter->setSource(m_freeSources.front());
			m_freeSources.pop();
			return;
		}
		m_waitingEmitters.push_back(emitter);
	}

	void SoundManager::releaseSource(SoundEmitter* emitter) {
		if (emitter->isActive()) {
			std::map<SoundEmitter*, ALuint>::iterator it = m_activeEmitters.find(emitter);
			if (it != m_activeEmitters.end()) {
				m_activeEmitters.erase(it);
				m_freeSources.push(emitter->getSource());
				emitter->setSource(0);
			} else {
				FL_ERR(_log, LMsg() << "SoundEmitter can not release source handler");
			}
		}
	}

	bool SoundManager::isInRange(SoundEmitter* emitter) const {
		ALfloat vec[3];
		alGetListenerfv(AL_POSITION, vec);

		AudioSpaceCoordinate listenerPos = AudioSpaceCoordinate(vec[0], vec[1], vec[2]);
		AudioSpaceCoordinate emitterPos = emitter->getPosition();

		double maxDistance = static_cast<double>(m_maxDistance);

		double rx = listenerPos.x - emitterPos.x;
		double ry = listenerPos.y - emitterPos.y;
		double rz = listenerPos.z - emitterPos.z;

		return (maxDistance >= Mathd::Sqrt(rx*rx + ry*ry + rz*rz));
	}

} //FIFE
