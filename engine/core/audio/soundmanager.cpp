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

	SoundManager::SoundManager() : m_context(0),
				       m_device(0),
				       m_mutevol(0),
					   m_volume(1.0) {
	}

	SoundManager::~SoundManager() {

		// free all soundemitters

		for (std::vector<SoundEmitter*>::iterator it = m_emittervec.begin(), it_end = m_emittervec.end(); it != it_end;  ++it) {
			if ((*it) != NULL) {
				delete (*it);
			}
		}

		m_emittervec.clear();

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

	SoundEmitter* SoundManager::getEmitter(uint32_t emitterid) const{
		return m_emittervec.at(emitterid);
	}

	SoundEmitter* SoundManager::createEmitter() {
		SoundEmitter* ptr = new SoundEmitter(this, m_emittervec.size());
		m_emittervec.push_back(ptr);
		return ptr;
	}

	void SoundManager::releaseEmitter(uint32_t emitterid) {
		SoundEmitter** ptr = &m_emittervec.at(emitterid);
		delete *ptr;
		*ptr = NULL;
	}
} //FIFE
