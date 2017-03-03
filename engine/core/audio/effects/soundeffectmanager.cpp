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

		alcGetIntegerv(m_device, ALC_MAX_AUXILIARY_SENDS, 1, &m_maxSlots);

		createPresets();
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
		}
		if (effect) {
			m_effects.push_back(effect);
		}
		return effect;
	}

	SoundEffect* SoundEffectManager::createSoundEffectPreset(SoundEffectPreset type) {
		std::map<SoundEffectPreset, EFXEAXREVERBPROPERTIES>::iterator it = m_presets.find(type);
		if (it == m_presets.end()) {
			return NULL;
		}
		SoundEffect* effect = new EaxReverb();
		m_effects.push_back(effect);

		EaxReverb* reverb = static_cast<EaxReverb*>(effect);
		reverb->loadPreset(it->second);
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

	EFXEAXREVERBPROPERTIES SoundEffectManager::presetCreater(EFXEAXREVERBPROPERTIES prop) {
		EFXEAXREVERBPROPERTIES pro = prop;
		return pro;
	}

	void SoundEffectManager::createPresets() {
		m_presets[SE_PRESET_GENERIC] = presetCreater(EFX_REVERB_PRESET_GENERIC);
		m_presets[SE_PRESET_PADDEDCELL] = presetCreater(EFX_REVERB_PRESET_PADDEDCELL);
		m_presets[SE_PRESET_ROOM] = presetCreater(EFX_REVERB_PRESET_ROOM);
		m_presets[SE_PRESET_BATHROOM] = presetCreater(EFX_REVERB_PRESET_BATHROOM);
		m_presets[SE_PRESET_LIVINGROOM] = presetCreater(EFX_REVERB_PRESET_LIVINGROOM);
		m_presets[SE_PRESET_STONEROOM] = presetCreater(EFX_REVERB_PRESET_STONEROOM);
		m_presets[SE_PRESET_AUDITORIUM] = presetCreater(EFX_REVERB_PRESET_AUDITORIUM);
		m_presets[SE_PRESET_CONCERTHALL] = presetCreater(EFX_REVERB_PRESET_CONCERTHALL);
		m_presets[SE_PRESET_CAVE] = presetCreater(EFX_REVERB_PRESET_CAVE);
		m_presets[SE_PRESET_ARENA] = presetCreater(EFX_REVERB_PRESET_ARENA);
		m_presets[SE_PRESET_HANGAR] = presetCreater(EFX_REVERB_PRESET_HANGAR);
		m_presets[SE_PRESET_CARPETEDHALLWAY] = presetCreater(EFX_REVERB_PRESET_CARPETEDHALLWAY);
		m_presets[SE_PRESET_HALLWAY] = presetCreater(EFX_REVERB_PRESET_HALLWAY);
		m_presets[SE_PRESET_STONECORRIDOR] = presetCreater(EFX_REVERB_PRESET_STONECORRIDOR);
		m_presets[SE_PRESET_ALLEY] = presetCreater(EFX_REVERB_PRESET_ALLEY);
		m_presets[SE_PRESET_FOREST] = presetCreater(EFX_REVERB_PRESET_FOREST);
		m_presets[SE_PRESET_CITY] = presetCreater(EFX_REVERB_PRESET_CITY);
		m_presets[SE_PRESET_MOUNTAINS] = presetCreater(EFX_REVERB_PRESET_MOUNTAINS);
		m_presets[SE_PRESET_QUARRY] = presetCreater(EFX_REVERB_PRESET_QUARRY);
		m_presets[SE_PRESET_PLAIN] = presetCreater(EFX_REVERB_PRESET_PLAIN);
		m_presets[SE_PRESET_PARKINGLOT] = presetCreater(EFX_REVERB_PRESET_PARKINGLOT);
		m_presets[SE_PRESET_SEWERPIPE] = presetCreater(EFX_REVERB_PRESET_SEWERPIPE);
		m_presets[SE_PRESET_UNDERWATER] = presetCreater(EFX_REVERB_PRESET_UNDERWATER);
		m_presets[SE_PRESET_DRUGGED] = presetCreater(EFX_REVERB_PRESET_DRUGGED);
		m_presets[SE_PRESET_DIZZY] = presetCreater(EFX_REVERB_PRESET_DIZZY);
		m_presets[SE_PRESET_PSYCHOTIC] = presetCreater(EFX_REVERB_PRESET_PSYCHOTIC);
		m_presets[SE_PRESET_CASTLE_SMALLROOM] = presetCreater(EFX_REVERB_PRESET_CASTLE_SMALLROOM);
		m_presets[SE_PRESET_CASTLE_SHORTPASSAGE] = presetCreater(EFX_REVERB_PRESET_CASTLE_SHORTPASSAGE);
		m_presets[SE_PRESET_CASTLE_MEDIUMROOM] = presetCreater(EFX_REVERB_PRESET_CASTLE_MEDIUMROOM);
		m_presets[SE_PRESET_CASTLE_LARGEROOM] = presetCreater(EFX_REVERB_PRESET_CASTLE_LARGEROOM);
		m_presets[SE_PRESET_CASTLE_LONGPASSAGE] = presetCreater(EFX_REVERB_PRESET_CASTLE_LONGPASSAGE);
		m_presets[SE_PRESET_CASTLE_HALL] = presetCreater(EFX_REVERB_PRESET_CASTLE_HALL);
		m_presets[SE_PRESET_CASTLE_CUPBOARD] = presetCreater(EFX_REVERB_PRESET_CASTLE_CUPBOARD);
		m_presets[SE_PRESET_CASTLE_COURTYARD] = presetCreater(EFX_REVERB_PRESET_CASTLE_COURTYARD);
		m_presets[SE_PRESET_CASTLE_ALCOVE] = presetCreater(EFX_REVERB_PRESET_CASTLE_ALCOVE);
		m_presets[SE_PRESET_FACTORY_SMALLROOM] = presetCreater(EFX_REVERB_PRESET_FACTORY_SMALLROOM);
		m_presets[SE_PRESET_FACTORY_SHORTPASSAGE] = presetCreater(EFX_REVERB_PRESET_FACTORY_SHORTPASSAGE);
		m_presets[SE_PRESET_FACTORY_MEDIUMROOM] = presetCreater(EFX_REVERB_PRESET_FACTORY_MEDIUMROOM);
		m_presets[SE_PRESET_FACTORY_LARGEROOM] = presetCreater(EFX_REVERB_PRESET_FACTORY_LARGEROOM);
		m_presets[SE_PRESET_FACTORY_LONGPASSAGE] = presetCreater(EFX_REVERB_PRESET_FACTORY_LONGPASSAGE);
		m_presets[SE_PRESET_FACTORY_HALL] = presetCreater(EFX_REVERB_PRESET_FACTORY_HALL);
		m_presets[SE_PRESET_FACTORY_CUPBOARD] = presetCreater(EFX_REVERB_PRESET_FACTORY_CUPBOARD);
		m_presets[SE_PRESET_FACTORY_COURTYARD] = presetCreater(EFX_REVERB_PRESET_FACTORY_COURTYARD);
		m_presets[SE_PRESET_FACTORY_ALCOVE] = presetCreater(EFX_REVERB_PRESET_FACTORY_ALCOVE);
		m_presets[SE_PRESET_ICEPALACE_SMALLROOM] = presetCreater(EFX_REVERB_PRESET_ICEPALACE_SMALLROOM);
		m_presets[SE_PRESET_ICEPALACE_SHORTPASSAGE] = presetCreater(EFX_REVERB_PRESET_ICEPALACE_SHORTPASSAGE);
		m_presets[SE_PRESET_ICEPALACE_MEDIUMROOM] = presetCreater(EFX_REVERB_PRESET_ICEPALACE_MEDIUMROOM);
		m_presets[SE_PRESET_ICEPALACE_LARGEROOM] = presetCreater(EFX_REVERB_PRESET_ICEPALACE_LARGEROOM);
		m_presets[SE_PRESET_ICEPALACE_LONGPASSAGE] = presetCreater(EFX_REVERB_PRESET_ICEPALACE_LONGPASSAGE);
		m_presets[SE_PRESET_ICEPALACE_HALL] = presetCreater(EFX_REVERB_PRESET_ICEPALACE_HALL);
		m_presets[SE_PRESET_ICEPALACE_CUPBOARD] = presetCreater(EFX_REVERB_PRESET_ICEPALACE_CUPBOARD);
		m_presets[SE_PRESET_ICEPALACE_COURTYARD] = presetCreater(EFX_REVERB_PRESET_ICEPALACE_COURTYARD);
		m_presets[SE_PRESET_ICEPALACE_ALCOVE] = presetCreater(EFX_REVERB_PRESET_ICEPALACE_ALCOVE);
		m_presets[SE_PRESET_SPACESTATION_SMALLROOM] = presetCreater(EFX_REVERB_PRESET_SPACESTATION_SMALLROOM);
		m_presets[SE_PRESET_SPACESTATION_SHORTPASSAGE] = presetCreater(EFX_REVERB_PRESET_SPACESTATION_SHORTPASSAGE);
		m_presets[SE_PRESET_SPACESTATION_MEDIUMROOM] = presetCreater(EFX_REVERB_PRESET_SPACESTATION_MEDIUMROOM);
		m_presets[SE_PRESET_SPACESTATION_LARGEROOM] = presetCreater(EFX_REVERB_PRESET_SPACESTATION_LARGEROOM);
		m_presets[SE_PRESET_SPACESTATION_LONGPASSAGE] = presetCreater(EFX_REVERB_PRESET_SPACESTATION_LONGPASSAGE);
		m_presets[SE_PRESET_SPACESTATION_HALL] = presetCreater(EFX_REVERB_PRESET_SPACESTATION_HALL);
		m_presets[SE_PRESET_SPACESTATION_CUPBOARD] = presetCreater(EFX_REVERB_PRESET_SPACESTATION_CUPBOARD);
		m_presets[SE_PRESET_SPACESTATION_ALCOVE] = presetCreater(EFX_REVERB_PRESET_SPACESTATION_ALCOVE);
		m_presets[SE_PRESET_WOODEN_SMALLROOM] = presetCreater(EFX_REVERB_PRESET_WOODEN_SMALLROOM);
		m_presets[SE_PRESET_WOODEN_SHORTPASSAGE] = presetCreater(EFX_REVERB_PRESET_WOODEN_SHORTPASSAGE);
		m_presets[SE_PRESET_WOODEN_MEDIUMROOM] = presetCreater(EFX_REVERB_PRESET_WOODEN_MEDIUMROOM);
		m_presets[SE_PRESET_WOODEN_LARGEROOM] = presetCreater(EFX_REVERB_PRESET_WOODEN_LARGEROOM);
		m_presets[SE_PRESET_WOODEN_LONGPASSAGE] = presetCreater(EFX_REVERB_PRESET_WOODEN_LONGPASSAGE);
		m_presets[SE_PRESET_WOODEN_HALL] = presetCreater(EFX_REVERB_PRESET_WOODEN_HALL);
		m_presets[SE_PRESET_WOODEN_CUPBOARD] = presetCreater(EFX_REVERB_PRESET_WOODEN_CUPBOARD);
		m_presets[SE_PRESET_WOODEN_COURTYARD] = presetCreater(EFX_REVERB_PRESET_WOODEN_COURTYARD);
		m_presets[SE_PRESET_WOODEN_ALCOVE] = presetCreater(EFX_REVERB_PRESET_WOODEN_ALCOVE);
		m_presets[SE_PRESET_SPORT_EMPTYSTADIUM] = presetCreater(EFX_REVERB_PRESET_SPORT_EMPTYSTADIUM);
		m_presets[SE_PRESET_SPORT_SQUASHCOURT] = presetCreater(EFX_REVERB_PRESET_SPORT_SQUASHCOURT);
		m_presets[SE_PRESET_SPORT_SMALLSWIMMINGPOOL] = presetCreater(EFX_REVERB_PRESET_SPORT_SMALLSWIMMINGPOOL);
		m_presets[SE_PRESET_SPORT_LARGESWIMMINGPOOL] = presetCreater(EFX_REVERB_PRESET_SPORT_LARGESWIMMINGPOOL);
		m_presets[SE_PRESET_SPORT_GYMNASIUM] = presetCreater(EFX_REVERB_PRESET_SPORT_GYMNASIUM);
		m_presets[SE_PRESET_SPORT_FULLSTADIUM] = presetCreater(EFX_REVERB_PRESET_SPORT_FULLSTADIUM);
		m_presets[SE_PRESET_SPORT_STADIUMTANNOY] = presetCreater(EFX_REVERB_PRESET_SPORT_STADIUMTANNOY);
		m_presets[SE_PRESET_PREFAB_WORKSHOP] = presetCreater(EFX_REVERB_PRESET_PREFAB_WORKSHOP);
		m_presets[SE_PRESET_PREFAB_SCHOOLROOM] = presetCreater(EFX_REVERB_PRESET_PREFAB_SCHOOLROOM);
		m_presets[SE_PRESET_PREFAB_PRACTISEROOM] = presetCreater(EFX_REVERB_PRESET_PREFAB_PRACTISEROOM);
		m_presets[SE_PRESET_PREFAB_OUTHOUSE] = presetCreater(EFX_REVERB_PRESET_PREFAB_OUTHOUSE);
		m_presets[SE_PRESET_PREFAB_CARAVAN] = presetCreater(EFX_REVERB_PRESET_PREFAB_CARAVAN);
		m_presets[SE_PRESET_DOME_TOMB] = presetCreater(EFX_REVERB_PRESET_DOME_TOMB);
		m_presets[SE_PRESET_PIPE_SMALL] = presetCreater(EFX_REVERB_PRESET_PIPE_SMALL);
		m_presets[SE_PRESET_DOME_SAINTPAULS] = presetCreater(EFX_REVERB_PRESET_DOME_SAINTPAULS);
		m_presets[SE_PRESET_PIPE_LONGTHIN] = presetCreater(EFX_REVERB_PRESET_PIPE_LONGTHIN);
		m_presets[SE_PRESET_PIPE_LARGE] = presetCreater(EFX_REVERB_PRESET_PIPE_LARGE);
		m_presets[SE_PRESET_PIPE_RESONANT] = presetCreater(EFX_REVERB_PRESET_PIPE_RESONANT);
		m_presets[SE_PRESET_OUTDOORS_BACKYARD] = presetCreater(EFX_REVERB_PRESET_OUTDOORS_BACKYARD);
		m_presets[SE_PRESET_OUTDOORS_ROLLINGPLAINS] = presetCreater(EFX_REVERB_PRESET_OUTDOORS_ROLLINGPLAINS);
		m_presets[SE_PRESET_OUTDOORS_DEEPCANYON] = presetCreater(EFX_REVERB_PRESET_OUTDOORS_DEEPCANYON);
		m_presets[SE_PRESET_OUTDOORS_CREEK] = presetCreater(EFX_REVERB_PRESET_OUTDOORS_CREEK);
		m_presets[SE_PRESET_OUTDOORS_VALLEY] = presetCreater(EFX_REVERB_PRESET_OUTDOORS_VALLEY);
		m_presets[SE_PRESET_MOOD_HEAVEN] = presetCreater(EFX_REVERB_PRESET_MOOD_HEAVEN);
		m_presets[SE_PRESET_MOOD_HELL] = presetCreater(EFX_REVERB_PRESET_MOOD_HELL);
		m_presets[SE_PRESET_MOOD_MEMORY] = presetCreater(EFX_REVERB_PRESET_MOOD_MEMORY);
		m_presets[SE_PRESET_DRIVING_COMMENTATOR] = presetCreater(EFX_REVERB_PRESET_DRIVING_COMMENTATOR);
		m_presets[SE_PRESET_DRIVING_PITGARAGE] = presetCreater(EFX_REVERB_PRESET_DRIVING_PITGARAGE);
		m_presets[SE_PRESET_DRIVING_INCAR_RACER] = presetCreater(EFX_REVERB_PRESET_DRIVING_INCAR_RACER);
		m_presets[SE_PRESET_DRIVING_INCAR_SPORTS] = presetCreater(EFX_REVERB_PRESET_DRIVING_INCAR_SPORTS);
		m_presets[SE_PRESET_DRIVING_INCAR_LUXURY] = presetCreater(EFX_REVERB_PRESET_DRIVING_INCAR_LUXURY);
		m_presets[SE_PRESET_DRIVING_FULLGRANDSTAND] = presetCreater(EFX_REVERB_PRESET_DRIVING_FULLGRANDSTAND);
		m_presets[SE_PRESET_DRIVING_EMPTYGRANDSTAND] = presetCreater(EFX_REVERB_PRESET_DRIVING_EMPTYGRANDSTAND);
		m_presets[SE_PRESET_DRIVING_TUNNEL] = presetCreater(EFX_REVERB_PRESET_DRIVING_TUNNEL);
		m_presets[SE_PRESET_CITY_STREETS] = presetCreater(EFX_REVERB_PRESET_CITY_STREETS);
		m_presets[SE_PRESET_CITY_SUBWAY] = presetCreater(EFX_REVERB_PRESET_CITY_SUBWAY);
		m_presets[SE_PRESET_CITY_MUSEUM] = presetCreater(EFX_REVERB_PRESET_CITY_MUSEUM);
		m_presets[SE_PRESET_CITY_LIBRARY] = presetCreater(EFX_REVERB_PRESET_CITY_LIBRARY);
		m_presets[SE_PRESET_CITY_UNDERPASS] = presetCreater(EFX_REVERB_PRESET_CITY_UNDERPASS);
		m_presets[SE_PRESET_CITY_ABANDONED] = presetCreater(EFX_REVERB_PRESET_CITY_ABANDONED);
		m_presets[SE_PRESET_DUSTYROOM] = presetCreater(EFX_REVERB_PRESET_DUSTYROOM);
		m_presets[SE_PRESET_CHAPEL] = presetCreater(EFX_REVERB_PRESET_CHAPEL);
		m_presets[SE_PRESET_SMALLWATERROOM] = presetCreater(EFX_REVERB_PRESET_SMALLWATERROOM);
	}

} //FIFE
