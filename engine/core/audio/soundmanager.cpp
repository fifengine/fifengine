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
		m_volume(1.0) {
	}

	SoundManager::~SoundManager() {
		// free all soundemitters
		for (std::vector<SoundEmitter*>::iterator it = m_emitterVec.begin(), it_end = m_emitterVec.end(); it != it_end;  ++it) {
			if ((*it) != NULL) {
				delete (*it);
			}
		}

		m_emitterVec.clear();

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
	}

	void SoundManager::update() {
		for (std::vector<SoundEmitter*>::iterator it = m_emitterVec.begin(), it_end = m_emitterVec.end(); it != it_end;  ++it) {
			if ((*it) != NULL) {
				(*it)->update();
			}
		}
	}

	SoundEmitter* SoundManager::getEmitter(uint32_t emitterId) const{
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
		//SoundEmitter* ptr = new SoundEmitter(this, m_emitterVec.size());
		//m_emitterVec.push_back(ptr);
		return ptr;
	}

	void SoundManager::releaseEmitter(uint32_t emitterId) {
		SoundEmitter** ptr = &m_emitterVec.at(emitterId);
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

	void SoundManager::setListenerPosition(float x, float y, float z) {
		alListener3f(AL_POSITION, x, y, z);
	}

	void SoundManager::setListenerOrientation(float x, float y, float z) {
		ALfloat vec[6] = { x, y, z, 0.0, 0.0, 1.0};
		alListenerfv(AL_ORIENTATION, vec);
	}

	void SoundManager::setListenerVelocity(float x, float y, float z) {
		alListener3f(AL_VELOCITY, x, y, z);
	}

	bool SoundManager::isActive() const {
		return m_device != NULL;
	}
} //FIFE
