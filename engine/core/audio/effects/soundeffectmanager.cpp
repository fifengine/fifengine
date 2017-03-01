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
#include "audio/soundemitter.h"
#include "util/log/logger.h"
#include "util/base/exception.h"

#include "soundeffect.h"
#include "soundfilter.h"
#include "soundeffectmanager.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_AUDIO);
	

	// Effect Slots
	LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots = NULL;
	LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots = NULL;
	LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot = NULL;
	LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti = NULL;
	LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv = NULL;
	LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf = NULL;
	LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv = NULL;
	LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti = NULL;
	LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv = NULL;
	LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf = NULL;
	LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv = NULL;

	// Effects
	LPALGENEFFECTS alGenEffects = NULL;
	LPALDELETEEFFECTS alDeleteEffects = NULL;
	LPALISEFFECT alIsEffect = NULL;
	LPALEFFECTI alEffecti = NULL;
	LPALEFFECTIV alEffectiv = NULL;
	LPALEFFECTF alEffectf = NULL;
	LPALEFFECTFV alEffectfv = NULL;
	LPALGETEFFECTI alGetEffecti = NULL;
	LPALGETEFFECTIV alGetEffectiv = NULL;
	LPALGETEFFECTF alGetEffectf = NULL;
	LPALGETEFFECTFV alGetEffectfv = NULL;

	// Filters
	LPALGENFILTERS alGenFilters = NULL;
	LPALDELETEFILTERS alDeleteFilters = NULL;
	LPALISFILTER alIsFilter = NULL;
	LPALFILTERI alFilteri = NULL;
	LPALFILTERIV alFilteriv = NULL;
	LPALFILTERF alFilterf = NULL;
	LPALFILTERFV alFilterfv = NULL;
	LPALGETFILTERI alGetFilteri = NULL;
	LPALGETFILTERIV alGetFilteriv = NULL;
	LPALGETFILTERF alGetFilterf = NULL;
	LPALGETFILTERFV alGetFilterfv = NULL;


	SoundEffectManager::SoundEffectManager() :
		m_device(NULL),
		m_active(false),
		m_createdSlots(0),
		m_maxSlots(0) {

	}

	SoundEffectManager::~SoundEffectManager() {
		// SoundEmitters are destroyed beforehand
		for (std::vector<SoundFilter*>::iterator it = m_filters.begin(); it != m_filters.end(); ++it) {
			delete *it;
		}
		for (std::vector<SoundEffect*>::iterator it = m_effects.begin(); it != m_effects.end(); ++it) {
			delete *it;
		}
	}

	void SoundEffectManager::init(ALCdevice* device) {
		m_device = device;

		if (alcIsExtensionPresent(m_device, "ALC_EXT_EFX") == AL_FALSE) {
			FL_WARN(_log, LMsg() << "ALC_EXT_EFX not supported!\n");
			return;
		}

		// Slot functions
		alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
		alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
		alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot");
		alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
		alAuxiliaryEffectSlotiv = (LPALAUXILIARYEFFECTSLOTIV)alGetProcAddress("alAuxiliaryEffectSlotiv");
		alAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf");
		alAuxiliaryEffectSlotfv = (LPALAUXILIARYEFFECTSLOTFV)alGetProcAddress("alAuxiliaryEffectSlotfv");
		alGetAuxiliaryEffectSloti = (LPALGETAUXILIARYEFFECTSLOTI)alGetProcAddress("alGetAuxiliaryEffectSloti");
		alGetAuxiliaryEffectSlotiv = (LPALGETAUXILIARYEFFECTSLOTIV)alGetProcAddress("alGetAuxiliaryEffectSlotiv");
		alGetAuxiliaryEffectSlotf = (LPALGETAUXILIARYEFFECTSLOTF)alGetProcAddress("alGetAuxiliaryEffectSlotf");
		alGetAuxiliaryEffectSlotfv = (LPALGETAUXILIARYEFFECTSLOTFV)alGetProcAddress("alGetAuxiliaryEffectSlotfv");
		if (!(alGenAuxiliaryEffectSlots && alDeleteAuxiliaryEffectSlots && alIsAuxiliaryEffectSlot &&
			alAuxiliaryEffectSloti && alAuxiliaryEffectSlotiv && alAuxiliaryEffectSlotf &&
			alAuxiliaryEffectSlotfv && alGetAuxiliaryEffectSloti && alGetAuxiliaryEffectSlotiv &&
			alGetAuxiliaryEffectSlotf && alGetAuxiliaryEffectSlotfv)) {
			FL_WARN(_log, LMsg() << "Failed initializing slot function pointers\n");
			return;
		}

		// Effect functions
		alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
		alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
		alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");
		alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
		alEffectiv = (LPALEFFECTIV)alGetProcAddress("alEffectiv");
		alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
		alEffectfv = (LPALEFFECTFV)alGetProcAddress("alEffectfv");
		alGetEffecti = (LPALGETEFFECTI)alGetProcAddress("alGetEffecti");
		alGetEffectiv = (LPALGETEFFECTIV)alGetProcAddress("alGetEffectiv");
		alGetEffectf = (LPALGETEFFECTF)alGetProcAddress("alGetEffectf");
		alGetEffectfv = (LPALGETEFFECTFV)alGetProcAddress("alGetEffectfv");
		if (!(alGenEffects && alDeleteEffects && alIsEffect && alEffecti && alEffectiv && alEffectf &&
			alEffectfv && alGetEffecti && alGetEffectiv && alGetEffectf && alGetEffectfv)) {
			FL_WARN(_log, LMsg() << "Failed initializing effect function pointers\n");
			return;
		}

		// Filter functions
		alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
		alDeleteFilters = (LPALDELETEFILTERS)alGetProcAddress("alDeleteFilters");
		alIsFilter = (LPALISFILTER)alGetProcAddress("alIsFilter");
		alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri");
		alFilteriv = (LPALFILTERIV)alGetProcAddress("alFilteriv");
		alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf");
		alFilterfv = (LPALFILTERFV)alGetProcAddress("alFilterfv");
		alGetFilteri = (LPALGETFILTERI)alGetProcAddress("alGetFilteri");
		alGetFilteriv = (LPALGETFILTERIV)alGetProcAddress("alGetFilteriv");
		alGetFilterf = (LPALGETFILTERF)alGetProcAddress("alGetFilterf");
		alGetFilterfv = (LPALGETFILTERFV)alGetProcAddress("alGetFilterfv");
		if (!(alGenFilters && alDeleteFilters && alIsFilter && alFilteri && alFilteriv && alFilterf &&
			alFilterfv && alGetFilteri && alGetFilteriv && alGetFilterf && alGetFilterfv)) {
			FL_WARN(_log, LMsg() << "Failed initializing filter function pointers\n");
			return;
		}

		m_active = true;
		
		// create max effect slots
		for (uint16_t i = 0; i < MAX_EFFECT_SLOTS; i++) {
			alGenAuxiliaryEffectSlots(1, &m_effectSlots[i]);
			if (alGetError() != AL_NO_ERROR) {
				break;
			}

			m_freeSlots.push(m_effectSlots[i]);
			m_createdSlots++;
		}
		std::cout << "created slots " << m_createdSlots << "\n";
		alcGetIntegerv(m_device, ALC_MAX_AUXILIARY_SENDS, 1, &m_maxSlots);
		std::cout << "max slots per source" << m_maxSlots << "\n";
	}

	bool SoundEffectManager::isActive() const {
		return m_active;
	}

	SoundEffect* SoundEffectManager::createSoundEffect(SoundEffectType type) {
		SoundEffect* effect = NULL;
		if (type == SE_EFFECT_REVERB) {
			effect = new Reverb();
		} else if (type == SE_EFFECT_CHORUS) {
			effect = new Chorus();
		} else if (type == SE_EFFECT_DISTORTION) {
			effect = new Distortion();
		} else if (type == SE_EFFECT_ECHO) {
			effect = new Echo();
		} else if (type == SE_EFFECT_FLANGER) {
			effect = new Flanger();
		} else if (type == SE_EFFECT_FREQUENCY_SHIFTER) {
			effect = new FrequencyShifter();
		} else if (type == SE_EFFECT_VOCAL_MORPHER) {
			effect = new VocalMorpher();
		} else if (type == SE_EFFECT_PITCH_SHIFTER) {
			effect = new PitchShifter();
		} else if (type == SE_EFFECT_RING_MODULATOR) {
			effect = new RingModulator();
		} else if (type == SE_EFFECT_AUTOWAH) {
			effect = new Autowah();
		} else if (type == SE_EFFECT_COMPRESSOR) {
			effect = new Compressor();
		} else if (type == SE_EFFECT_EQUALIZER) {
			effect = new Equalizer();
		} else if (type == SE_EFFECT_EAXREVERB) {
			effect = new EaxReverb();
		} else if (type == SE_EFFECT_FADE) {
			effect = new Fade();
		}
		if (effect) {
			m_effects.push_back(effect);
		}
		return effect;
	}

	void SoundEffectManager::deleteSoundEffect(SoundEffect* effect) {
		disableSoundEffect(effect);
		for (std::vector<SoundEffect*>::iterator it = m_effects.begin(); it != m_effects.end(); ++it) {
			if (effect == *it) {
				std::map<SoundEffect*, std::vector<SoundEmitter*> >::iterator effectIt = m_effectEmitters.find(effect);
				if (effectIt != m_effectEmitters.end()) {
					std::vector<SoundEmitter*>::iterator emitterIt = effectIt->second.begin();
					for (; emitterIt != effectIt->second.end(); ++emitterIt) {
						(*emitterIt)->removeEffect(effect);
					}
				}
				m_effectEmitters.erase(effectIt);
				delete *it;
				m_effects.erase(it);
				break;
			}
		}
	}

	void SoundEffectManager::enableSoundEffect(SoundEffect* effect) {
		if (m_freeSlots.empty() || effect->isEnabled()) {
			if (m_freeSlots.empty()) {
				FL_WARN(_log, LMsg() << "No free auxiliary slot available");
			}
			return;
		}

		ALuint slot = m_freeSlots.front();
		ALuint filter = effect->getFilter() ? effect->getFilter()->getFilterId() : AL_FILTER_NULL;
		m_freeSlots.pop();
		alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, effect->getEffectId());
		effect->setSlotId(slot);
		effect->setEnabled(true);
		std::map<SoundEffect*, std::vector<SoundEmitter*> >::iterator effectIt = m_effectEmitters.find(effect);
		if (effectIt != m_effectEmitters.end()) {
			std::vector<SoundEmitter*>::iterator emitterIt = effectIt->second.begin();
			for (; emitterIt != effectIt->second.end(); ++emitterIt) {
				if (!(*emitterIt)->isActive()) {
					continue;
				}
				activateEffect(effect, *emitterIt);
			}
		}
	}

	void SoundEffectManager::disableSoundEffect(SoundEffect* effect) {
		if (!effect->isEnabled()) {
			return;
		}
		alAuxiliaryEffectSloti(effect->getSlotId(), AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
		m_freeSlots.push(effect->getSlotId());
		effect->setSlotId(0);

		std::map<SoundEffect*, std::vector<SoundEmitter*> >::iterator effectIt = m_effectEmitters.find(effect);
		if (effectIt != m_effectEmitters.end()) {
			std::vector<SoundEmitter*>::iterator emitterIt = effectIt->second.begin();
			for (; emitterIt != effectIt->second.end(); ++emitterIt) {
				deactivateEffect(effect, *emitterIt);
			}
		}
		effect->setEnabled(false);
	}

	void SoundEffectManager::addEmitterToSoundEffect(SoundEffect* effect, SoundEmitter* emitter) {
		if (emitter->getEffectCount() == m_maxSlots) {
			FL_WARN(_log, LMsg() << "Maximal effect number for SoundEmitter reached");
			return;
		}
		m_effectEmitters[effect].push_back(emitter);
		emitter->addEffect(effect);
		if (emitter-isActive()) {
			activateEffect(effect, emitter);
		}
	}

	void SoundEffectManager::removeEmitterFromSoundEffect(SoundEffect* effect, SoundEmitter* emitter) {
		std::map<SoundEffect*, std::vector<SoundEmitter*> >::iterator effectIt = m_effectEmitters.find(effect);
		if (effectIt == m_effectEmitters.end()) {
			FL_WARN(_log, LMsg() << "SoundEmitter can not removed from unknown effect");
			return;
		}
		bool found = false;
		std::vector<SoundEmitter*>::iterator emitterIt = effectIt->second.begin();
		std::vector<SoundEmitter*>::iterator emitterEnd = effectIt->second.end();
		while (emitterIt != emitterEnd) {
			if ((*emitterIt) == emitter) {
				if (emitter->isActive()) {
					deactivateEffect(effect, emitter);
				}
				emitter->removeEffect(effect);
				effectIt->second.erase(emitterIt++);
				found = true;
			} else {
				++emitterIt;
			}
		}
		if (!found) {
			FL_WARN(_log, LMsg() << "SoundEmitter could not be found for the given effect.");
			return;
		}
	}

	void SoundEffectManager::activateEffect(SoundEffect* effect, SoundEmitter* emitter) {
		if (!effect->isEnabled()) {
			return;
		}
		ALuint number = static_cast<ALuint>(emitter->getEffectNumber(effect));
		ALuint filter = effect->getFilter() ? effect->getFilter()->getFilterId() : AL_FILTER_NULL;
		alSource3i(emitter->getSource(), AL_AUXILIARY_SEND_FILTER, effect->getSlotId(), number, filter);
	}

	void SoundEffectManager::deactivateEffect(SoundEffect* effect, SoundEmitter* emitter) {
		if (!effect->isEnabled()) {
			return;
		}
		ALuint number = static_cast<ALuint>(emitter->getEffectNumber(effect));
		alSource3i(emitter->getSource(), AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, number, AL_FILTER_NULL);
	}

	SoundFilter* SoundEffectManager::createSoundFilter(SoundFilterType type) {
		SoundFilter* filter = new SoundFilter(type);
		m_filters.push_back(filter);
		return filter;
	}

	void SoundEffectManager::deleteSoundFilter(SoundFilter* filter) {
		disableSoundFilter(filter);
		for (std::vector<SoundFilter*>::iterator it = m_filters.begin(); it != m_filters.end(); ++it) {
			if (filter == *it) {
				std::map<SoundFilter*, std::vector<SoundEmitter*> >::iterator filterIt = m_filterdEmitters.find(filter);
				if (filterIt != m_filterdEmitters.end()) {
					std::vector<SoundEmitter*>::iterator emitterIt = filterIt->second.begin();
					for (; emitterIt != filterIt->second.end(); ++emitterIt) {
						(*emitterIt)->setDirectFilter(NULL);
					}
				}
				m_filterdEmitters.erase(filterIt);
				delete *it;
				m_filters.erase(it);
				break;
			}
		}
	}

	void SoundEffectManager::enableSoundFilter(SoundFilter* filter) {
		if (filter->isEnabled()) {
			return;
		}
		filter->setEnabled(true);
		std::map<SoundFilter*, std::vector<SoundEmitter*> >::iterator filterIt = m_filterdEmitters.find(filter);
		if (filterIt != m_filterdEmitters.end()) {
			std::vector<SoundEmitter*>::iterator emitterIt = filterIt->second.begin();
			for (; emitterIt != filterIt->second.end(); ++emitterIt) {
				if ((*emitterIt)->isActive()) {
					activateFilter(filter, *emitterIt);
				}
			}
		}
	}

	void SoundEffectManager::disableSoundFilter(SoundFilter* filter) {
		if (!filter->isEnabled()) {
			return;
		}
		std::map<SoundFilter*, std::vector<SoundEmitter*> >::iterator filterIt = m_filterdEmitters.find(filter);
		if (filterIt != m_filterdEmitters.end()) {
			std::vector<SoundEmitter*>::iterator emitterIt = filterIt->second.begin();
			for (; emitterIt != filterIt->second.end(); ++emitterIt) {
				if ((*emitterIt)->isActive()) {
					deactivateFilter(filter, *emitterIt);
				}
			}
		}
		filter->setEnabled(false);
	}

	void SoundEffectManager::addEmitterToSoundFilter(SoundFilter* filter, SoundEmitter* emitter) {
		if (emitter->getDirectFilter()) {
			FL_WARN(_log, LMsg() << "SoundEmitter already has a direct filter");
			return;
		}
		emitter->setDirectFilter(filter);
		m_filterdEmitters[filter].push_back(emitter);
		if (emitter->isActive()) {
			activateFilter(filter, emitter);
		}
	}

	void SoundEffectManager::removeEmitterFromSoundFilter(SoundFilter* filter, SoundEmitter* emitter) {
		std::map<SoundFilter*, std::vector<SoundEmitter*> >::iterator filterIt = m_filterdEmitters.find(filter);
		if (filterIt == m_filterdEmitters.end()) {
			FL_WARN(_log, LMsg() << "SoundEmitter can not removed from unknown filter");
			return;
		}
		bool found = false;
		std::vector<SoundEmitter*>::iterator emitterIt = filterIt->second.begin();
		std::vector<SoundEmitter*>::iterator emitterEnd = filterIt->second.end();
		while (emitterIt != emitterEnd) {
			if ((*emitterIt) == emitter) {
				if (emitter-isActive()) {
					deactivateFilter(filter, emitter);
				}
				emitter->setDirectFilter(NULL);
				filterIt->second.erase(emitterIt++);
				found = true;
			}
			else {
				++emitterIt;
			}
		}
		if (!found) {
			FL_WARN(_log, LMsg() << "SoundEmitter could not be found for the given filter.");
			return;
		}
	}

	void SoundEffectManager::activateFilter(SoundFilter* filter, SoundEmitter* emitter) {
		if (filter->isEnabled()) {
			alSourcei(emitter->getSource(), AL_DIRECT_FILTER, filter->getFilterId());
		}
	}

	void SoundEffectManager::deactivateFilter(SoundFilter* filter, SoundEmitter* emitter) {
		if (filter->isEnabled()) {
			alSourcei(emitter->getSource(), AL_DIRECT_FILTER, AL_FILTER_NULL);
		}
	}

} //FIFE
