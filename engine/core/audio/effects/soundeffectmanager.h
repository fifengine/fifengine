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

#ifndef FIFE_SOUNDEFFECTMANAGER_H
#define FIFE_SOUNDEFFECTMANAGER_H

// Standard C++ library includes
#include <queue>
#include <map>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "audio/fife_openal.h"
#include "audio/soundconfig.h"

namespace FIFE {

	class SoundEffect;
	class SoundFilter;
	class SoundEmitter;

	class SoundEffectManager {
	public:

		SoundEffectManager();

		~SoundEffectManager();

		/** Initializes the effect system
		 */
		void init(ALCdevice* device);

		/** Returns true if sound effect module is active
		 */
		bool isActive() const;

		uint16_t getFreeSlots();

		SoundEffect* createSoundEffect(SoundEffectType type);
		SoundEffect* createSoundEffectPreset(SoundEffectPreset type);
		void deleteSoundEffect(SoundEffect* effect);
		void enableSoundEffect(SoundEffect* effect);
		void disableSoundEffect(SoundEffect* effect);
		void addEmitterToSoundEffect(SoundEffect* effect, SoundEmitter* emitter);
		void removeEmitterFromSoundEffect(SoundEffect* effect, SoundEmitter* emitter);
		void activateEffect(SoundEffect* effect, SoundEmitter* emitter);
		void deactivateEffect(SoundEffect* effect, SoundEmitter* emitter);

		SoundFilter* createSoundFilter(SoundFilterType type);
		void deleteSoundFilter(SoundFilter* filter);
		void enableSoundFilter(SoundFilter* filter);
		void disableSoundFilter(SoundFilter* filter);
		void addEmitterToSoundFilter(SoundFilter* filter, SoundEmitter* emitter);
		void removeEmitterFromSoundFilter(SoundFilter* filter, SoundEmitter* emitter);
		void activateFilter(SoundFilter* filter, SoundEmitter* emitter);
		void deactivateFilter(SoundFilter* filter, SoundEmitter* emitter);

	private:
		void createPresets();
		//! OpenAL device
		ALCdevice* m_device;

		//! If sound effect module is active
		bool m_active;

		//! Holds handles for effects
		ALuint m_effectSlots[MAX_EFFECT_SLOTS];
		//! Maximal created effect slots, can be different to MAX_EFFECT_SLOTS
		uint16_t m_createdSlots;
		//! Holds free handles for effect slots
		std::queue<ALuint> m_freeSlots;
		//! Maximal effect slots per Source
		ALint m_maxSlots;

		std::vector<SoundEffect*> m_effects;
		std::map<SoundEffect*, std::vector<SoundEmitter*> > m_effectEmitters;
		std::vector<SoundFilter*> m_filters;
		std::map<SoundFilter*, std::vector<SoundEmitter*> > m_filterdEmitters;

		std::map<SoundEffectPreset, EFXEAXREVERBPROPERTIES> m_presets;
	};
}
#endif
