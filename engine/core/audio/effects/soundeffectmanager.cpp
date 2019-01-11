/***************************************************************************
 *   Copyright (C) 2005-2019 by the FIFE team                              *
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
		// fetch maximal slots per source
		alcGetIntegerv(m_device, ALC_MAX_AUXILIARY_SENDS, 1, &m_maxSlots);
		// prepares the presets
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
		if (effect->getFilter()) {
			removeSoundFilterFromSoundEffect(effect, effect->getFilter());
		}
		for (std::vector<SoundEffect*>::iterator it = m_effects.begin(); it != m_effects.end(); ++it) {
			if (effect == *it) {
				SoundEffectEmitterMap::iterator effectIt = m_effectEmitters.find(effect);
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
		SoundEffectEmitterMap::iterator effectIt = m_effectEmitters.find(effect);
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

		SoundEffectEmitterMap::iterator effectIt = m_effectEmitters.find(effect);
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
		SoundEffectEmitterMap::iterator effectIt = m_effectEmitters.find(effect);
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

	void SoundEffectManager::addSoundFilterToSoundEffect(SoundEffect* effect, SoundFilter* filter) {
		if (effect->getFilter()) {
			FL_WARN(_log, LMsg() << "SoundEffect already has a filter");
			return;
		}
		effect->setFilter(filter);
		m_filterdEffects[filter].push_back(effect);
		if (effect->isEnabled()) {
			disableSoundEffect(effect);
			enableSoundEffect(effect);
		}
	}

	void SoundEffectManager::removeSoundFilterFromSoundEffect(SoundEffect* effect, SoundFilter* filter) {
		SoundFilterEffectMap::iterator filterIt = m_filterdEffects.find(filter);
		if (filterIt == m_filterdEffects.end()) {
			FL_WARN(_log, LMsg() << "SoundEffect can not removed from unknown filter");
			return;
		}
		bool found = false;
		std::vector<SoundEffect*>::iterator effectIt = filterIt->second.begin();
		std::vector<SoundEffect*>::iterator effectEnd = filterIt->second.end();
		while (effectIt != effectEnd) {
			if ((*effectIt) == effect) {
				effect->setFilter(NULL);
				if (effect->isEnabled()) {
					disableSoundEffect(effect);
					enableSoundEffect(effect);
				}
				filterIt->second.erase(effectIt++);
				found = true;
			}
			else {
				++effectIt;
			}
		}
		if (!found) {
			FL_WARN(_log, LMsg() << "SoundEffect could not be found for the given filter.");
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
		disableDirectSoundFilter(filter);
		for (std::vector<SoundFilter*>::iterator it = m_filters.begin(); it != m_filters.end(); ++it) {
			if (filter == *it) {
				SoundFilterEmitterMap::iterator filterIt = m_filterdEmitters.find(filter);
				if (filterIt != m_filterdEmitters.end()) {
					std::vector<SoundEmitter*>::iterator emitterIt = filterIt->second.begin();
					for (; emitterIt != filterIt->second.end(); ++emitterIt) {
						(*emitterIt)->setDirectFilter(NULL);
					}
				}
				m_filterdEmitters.erase(filterIt);
				SoundFilterEffectMap::iterator filterItt = m_filterdEffects.find(filter);
				if (filterItt != m_filterdEffects.end()) {
					std::vector<SoundEffect*>::iterator effectIt = filterItt->second.begin();
					for (; effectIt != filterItt->second.end(); ++effectIt) {
						(*effectIt)->setFilter(NULL);
						if ((*effectIt)->isEnabled()) {
							disableSoundEffect(*effectIt);
							enableSoundEffect(*effectIt);
						}
					}
				}
				m_filterdEffects.erase(filterItt);
				delete *it;
				m_filters.erase(it);
				break;
			}
		}
	}

	void SoundEffectManager::enableDirectSoundFilter(SoundFilter* filter) {
		if (filter->isEnabled()) {
			return;
		}
		filter->setEnabled(true);
		SoundFilterEmitterMap::iterator filterIt = m_filterdEmitters.find(filter);
		if (filterIt != m_filterdEmitters.end()) {
			std::vector<SoundEmitter*>::iterator emitterIt = filterIt->second.begin();
			for (; emitterIt != filterIt->second.end(); ++emitterIt) {
				if ((*emitterIt)->isActive()) {
					activateFilter(filter, *emitterIt);
				}
			}
		}
	}

	void SoundEffectManager::disableDirectSoundFilter(SoundFilter* filter) {
		if (!filter->isEnabled()) {
			return;
		}
		SoundFilterEmitterMap::iterator filterIt = m_filterdEmitters.find(filter);
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

	void SoundEffectManager::addEmitterToDirectSoundFilter(SoundFilter* filter, SoundEmitter* emitter) {
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

	void SoundEffectManager::removeEmitterFromDirectSoundFilter(SoundFilter* filter, SoundEmitter* emitter) {
		SoundFilterEmitterMap::iterator filterIt = m_filterdEmitters.find(filter);
		if (filterIt == m_filterdEmitters.end()) {
			FL_WARN(_log, LMsg() << "SoundEmitter can not removed from unknown filter");
			return;
		}
		bool found = false;
		std::vector<SoundEmitter*>::iterator emitterIt = filterIt->second.begin();
		std::vector<SoundEmitter*>::iterator emitterEnd = filterIt->second.end();
		while (emitterIt != emitterEnd) {
			if ((*emitterIt) == emitter) {
				if (emitter->isActive()) {
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

	void SoundEffectManager::createPresets() {
		EFXEAXREVERBPROPERTIES prop1 = EFX_REVERB_PRESET_GENERIC;
		EFXEAXREVERBPROPERTIES prop2 = EFX_REVERB_PRESET_PADDEDCELL;
		EFXEAXREVERBPROPERTIES prop3 = EFX_REVERB_PRESET_ROOM;
		EFXEAXREVERBPROPERTIES prop4 = EFX_REVERB_PRESET_BATHROOM;
		EFXEAXREVERBPROPERTIES prop5 = EFX_REVERB_PRESET_LIVINGROOM;
		EFXEAXREVERBPROPERTIES prop6 = EFX_REVERB_PRESET_STONEROOM;
		EFXEAXREVERBPROPERTIES prop7 = EFX_REVERB_PRESET_AUDITORIUM;
		EFXEAXREVERBPROPERTIES prop8 = EFX_REVERB_PRESET_CONCERTHALL;
		EFXEAXREVERBPROPERTIES prop9 = EFX_REVERB_PRESET_CAVE;
		EFXEAXREVERBPROPERTIES prop10 = EFX_REVERB_PRESET_ARENA;
		EFXEAXREVERBPROPERTIES prop11 = EFX_REVERB_PRESET_HANGAR;
		EFXEAXREVERBPROPERTIES prop12 = EFX_REVERB_PRESET_CARPETEDHALLWAY;
		EFXEAXREVERBPROPERTIES prop13 = EFX_REVERB_PRESET_HALLWAY;
		EFXEAXREVERBPROPERTIES prop14 = EFX_REVERB_PRESET_STONECORRIDOR;
		EFXEAXREVERBPROPERTIES prop15 = EFX_REVERB_PRESET_ALLEY;
		EFXEAXREVERBPROPERTIES prop16 = EFX_REVERB_PRESET_FOREST;
		EFXEAXREVERBPROPERTIES prop17 = EFX_REVERB_PRESET_CITY;
		EFXEAXREVERBPROPERTIES prop18 = EFX_REVERB_PRESET_MOUNTAINS;
		EFXEAXREVERBPROPERTIES prop19 = EFX_REVERB_PRESET_QUARRY;
		EFXEAXREVERBPROPERTIES prop20 = EFX_REVERB_PRESET_PLAIN;
		EFXEAXREVERBPROPERTIES prop21 = EFX_REVERB_PRESET_PARKINGLOT;
		EFXEAXREVERBPROPERTIES prop22 = EFX_REVERB_PRESET_SEWERPIPE;
		EFXEAXREVERBPROPERTIES prop23 = EFX_REVERB_PRESET_UNDERWATER;
		EFXEAXREVERBPROPERTIES prop24 = EFX_REVERB_PRESET_DRUGGED;
		EFXEAXREVERBPROPERTIES prop25 = EFX_REVERB_PRESET_DIZZY;
		EFXEAXREVERBPROPERTIES prop26 = EFX_REVERB_PRESET_PSYCHOTIC;
		EFXEAXREVERBPROPERTIES prop27 = EFX_REVERB_PRESET_CASTLE_SMALLROOM;
		EFXEAXREVERBPROPERTIES prop28 = EFX_REVERB_PRESET_CASTLE_SHORTPASSAGE;
		EFXEAXREVERBPROPERTIES prop29 = EFX_REVERB_PRESET_CASTLE_MEDIUMROOM;
		EFXEAXREVERBPROPERTIES prop30 = EFX_REVERB_PRESET_CASTLE_LARGEROOM;
		EFXEAXREVERBPROPERTIES prop31 = EFX_REVERB_PRESET_CASTLE_LONGPASSAGE;
		EFXEAXREVERBPROPERTIES prop32 = EFX_REVERB_PRESET_CASTLE_HALL;
		EFXEAXREVERBPROPERTIES prop33 = EFX_REVERB_PRESET_CASTLE_CUPBOARD;
		EFXEAXREVERBPROPERTIES prop34 = EFX_REVERB_PRESET_CASTLE_COURTYARD;
		EFXEAXREVERBPROPERTIES prop35 = EFX_REVERB_PRESET_CASTLE_ALCOVE;
		EFXEAXREVERBPROPERTIES prop36 = EFX_REVERB_PRESET_FACTORY_SMALLROOM;
		EFXEAXREVERBPROPERTIES prop37 = EFX_REVERB_PRESET_FACTORY_SHORTPASSAGE;
		EFXEAXREVERBPROPERTIES prop38 = EFX_REVERB_PRESET_FACTORY_MEDIUMROOM;
		EFXEAXREVERBPROPERTIES prop39 = EFX_REVERB_PRESET_FACTORY_LARGEROOM;
		EFXEAXREVERBPROPERTIES prop40 = EFX_REVERB_PRESET_FACTORY_LONGPASSAGE;
		EFXEAXREVERBPROPERTIES prop41 = EFX_REVERB_PRESET_FACTORY_HALL;
		EFXEAXREVERBPROPERTIES prop42 = EFX_REVERB_PRESET_FACTORY_CUPBOARD;
		EFXEAXREVERBPROPERTIES prop43 = EFX_REVERB_PRESET_FACTORY_COURTYARD;
		EFXEAXREVERBPROPERTIES prop44 = EFX_REVERB_PRESET_FACTORY_ALCOVE;
		EFXEAXREVERBPROPERTIES prop45 = EFX_REVERB_PRESET_ICEPALACE_SMALLROOM;
		EFXEAXREVERBPROPERTIES prop46 = EFX_REVERB_PRESET_ICEPALACE_SHORTPASSAGE;
		EFXEAXREVERBPROPERTIES prop47 = EFX_REVERB_PRESET_ICEPALACE_MEDIUMROOM;
		EFXEAXREVERBPROPERTIES prop48 = EFX_REVERB_PRESET_ICEPALACE_LARGEROOM;
		EFXEAXREVERBPROPERTIES prop49 = EFX_REVERB_PRESET_ICEPALACE_LONGPASSAGE;
		EFXEAXREVERBPROPERTIES prop50 = EFX_REVERB_PRESET_ICEPALACE_HALL;
		EFXEAXREVERBPROPERTIES prop51 = EFX_REVERB_PRESET_ICEPALACE_CUPBOARD;
		EFXEAXREVERBPROPERTIES prop52 = EFX_REVERB_PRESET_ICEPALACE_COURTYARD;
		EFXEAXREVERBPROPERTIES prop53 = EFX_REVERB_PRESET_ICEPALACE_ALCOVE;
		EFXEAXREVERBPROPERTIES prop54 = EFX_REVERB_PRESET_SPACESTATION_SMALLROOM;
		EFXEAXREVERBPROPERTIES prop55 = EFX_REVERB_PRESET_SPACESTATION_SHORTPASSAGE;
		EFXEAXREVERBPROPERTIES prop56 = EFX_REVERB_PRESET_SPACESTATION_MEDIUMROOM;
		EFXEAXREVERBPROPERTIES prop57 = EFX_REVERB_PRESET_SPACESTATION_LARGEROOM;
		EFXEAXREVERBPROPERTIES prop58 = EFX_REVERB_PRESET_SPACESTATION_LONGPASSAGE;
		EFXEAXREVERBPROPERTIES prop59 = EFX_REVERB_PRESET_SPACESTATION_HALL;
		EFXEAXREVERBPROPERTIES prop60 = EFX_REVERB_PRESET_SPACESTATION_CUPBOARD;
		EFXEAXREVERBPROPERTIES prop61 = EFX_REVERB_PRESET_SPACESTATION_ALCOVE;
		EFXEAXREVERBPROPERTIES prop62 = EFX_REVERB_PRESET_WOODEN_SMALLROOM;
		EFXEAXREVERBPROPERTIES prop63 = EFX_REVERB_PRESET_WOODEN_SHORTPASSAGE;
		EFXEAXREVERBPROPERTIES prop64 = EFX_REVERB_PRESET_WOODEN_MEDIUMROOM;
		EFXEAXREVERBPROPERTIES prop65 = EFX_REVERB_PRESET_WOODEN_LARGEROOM;
		EFXEAXREVERBPROPERTIES prop66 = EFX_REVERB_PRESET_WOODEN_LONGPASSAGE;
		EFXEAXREVERBPROPERTIES prop67 = EFX_REVERB_PRESET_WOODEN_HALL;
		EFXEAXREVERBPROPERTIES prop68 = EFX_REVERB_PRESET_WOODEN_CUPBOARD;
		EFXEAXREVERBPROPERTIES prop69 = EFX_REVERB_PRESET_WOODEN_COURTYARD;
		EFXEAXREVERBPROPERTIES prop70 = EFX_REVERB_PRESET_WOODEN_ALCOVE;
		EFXEAXREVERBPROPERTIES prop71 = EFX_REVERB_PRESET_SPORT_EMPTYSTADIUM;
		EFXEAXREVERBPROPERTIES prop72 = EFX_REVERB_PRESET_SPORT_SQUASHCOURT;
		EFXEAXREVERBPROPERTIES prop73 = EFX_REVERB_PRESET_SPORT_SMALLSWIMMINGPOOL;
		EFXEAXREVERBPROPERTIES prop74 = EFX_REVERB_PRESET_SPORT_LARGESWIMMINGPOOL;
		EFXEAXREVERBPROPERTIES prop75 = EFX_REVERB_PRESET_SPORT_GYMNASIUM;
		EFXEAXREVERBPROPERTIES prop76 = EFX_REVERB_PRESET_SPORT_FULLSTADIUM;
		EFXEAXREVERBPROPERTIES prop77 = EFX_REVERB_PRESET_SPORT_STADIUMTANNOY;
		EFXEAXREVERBPROPERTIES prop78 = EFX_REVERB_PRESET_PREFAB_WORKSHOP;
		EFXEAXREVERBPROPERTIES prop79 = EFX_REVERB_PRESET_PREFAB_SCHOOLROOM;
		EFXEAXREVERBPROPERTIES prop80 = EFX_REVERB_PRESET_PREFAB_PRACTISEROOM;
		EFXEAXREVERBPROPERTIES prop81 = EFX_REVERB_PRESET_PREFAB_OUTHOUSE;
		EFXEAXREVERBPROPERTIES prop82 = EFX_REVERB_PRESET_PREFAB_CARAVAN;
		EFXEAXREVERBPROPERTIES prop83 = EFX_REVERB_PRESET_DOME_TOMB;
		EFXEAXREVERBPROPERTIES prop84 = EFX_REVERB_PRESET_PIPE_SMALL;
		EFXEAXREVERBPROPERTIES prop85 = EFX_REVERB_PRESET_DOME_SAINTPAULS;
		EFXEAXREVERBPROPERTIES prop86 = EFX_REVERB_PRESET_PIPE_LONGTHIN;
		EFXEAXREVERBPROPERTIES prop87 = EFX_REVERB_PRESET_PIPE_LARGE;
		EFXEAXREVERBPROPERTIES prop88 = EFX_REVERB_PRESET_PIPE_RESONANT;
		EFXEAXREVERBPROPERTIES prop89 = EFX_REVERB_PRESET_OUTDOORS_BACKYARD;
		EFXEAXREVERBPROPERTIES prop90 = EFX_REVERB_PRESET_OUTDOORS_ROLLINGPLAINS;
		EFXEAXREVERBPROPERTIES prop91 = EFX_REVERB_PRESET_OUTDOORS_DEEPCANYON;
		EFXEAXREVERBPROPERTIES prop92 = EFX_REVERB_PRESET_OUTDOORS_CREEK;
		EFXEAXREVERBPROPERTIES prop93 = EFX_REVERB_PRESET_OUTDOORS_VALLEY;
		EFXEAXREVERBPROPERTIES prop94 = EFX_REVERB_PRESET_MOOD_HEAVEN;
		EFXEAXREVERBPROPERTIES prop95 = EFX_REVERB_PRESET_MOOD_HELL;
		EFXEAXREVERBPROPERTIES prop96 = EFX_REVERB_PRESET_MOOD_MEMORY;
		EFXEAXREVERBPROPERTIES prop97 = EFX_REVERB_PRESET_DRIVING_COMMENTATOR;
		EFXEAXREVERBPROPERTIES prop98 = EFX_REVERB_PRESET_DRIVING_PITGARAGE;
		EFXEAXREVERBPROPERTIES prop99 = EFX_REVERB_PRESET_DRIVING_INCAR_RACER;
		EFXEAXREVERBPROPERTIES prop100 = EFX_REVERB_PRESET_DRIVING_INCAR_SPORTS;
		EFXEAXREVERBPROPERTIES prop101 = EFX_REVERB_PRESET_DRIVING_INCAR_LUXURY;
		EFXEAXREVERBPROPERTIES prop102 = EFX_REVERB_PRESET_DRIVING_FULLGRANDSTAND;
		EFXEAXREVERBPROPERTIES prop103 = EFX_REVERB_PRESET_DRIVING_EMPTYGRANDSTAND;
		EFXEAXREVERBPROPERTIES prop104 = EFX_REVERB_PRESET_DRIVING_TUNNEL;
		EFXEAXREVERBPROPERTIES prop105 = EFX_REVERB_PRESET_CITY_STREETS;
		EFXEAXREVERBPROPERTIES prop106 = EFX_REVERB_PRESET_CITY_SUBWAY;
		EFXEAXREVERBPROPERTIES prop107 = EFX_REVERB_PRESET_CITY_MUSEUM;
		EFXEAXREVERBPROPERTIES prop108 = EFX_REVERB_PRESET_CITY_LIBRARY;
		EFXEAXREVERBPROPERTIES prop109 = EFX_REVERB_PRESET_CITY_UNDERPASS;
		EFXEAXREVERBPROPERTIES prop110 = EFX_REVERB_PRESET_CITY_ABANDONED;
		EFXEAXREVERBPROPERTIES prop111 = EFX_REVERB_PRESET_DUSTYROOM;
		EFXEAXREVERBPROPERTIES prop112 = EFX_REVERB_PRESET_CHAPEL;
		EFXEAXREVERBPROPERTIES prop113 = EFX_REVERB_PRESET_SMALLWATERROOM;

		m_presets[SE_PRESET_GENERIC] = prop1;
		m_presets[SE_PRESET_PADDEDCELL] = prop2;
		m_presets[SE_PRESET_ROOM] = prop3;
		m_presets[SE_PRESET_BATHROOM] = prop4;
		m_presets[SE_PRESET_LIVINGROOM] = prop5;
		m_presets[SE_PRESET_STONEROOM] = prop6;
		m_presets[SE_PRESET_AUDITORIUM] = prop7;
		m_presets[SE_PRESET_CONCERTHALL] = prop8;
		m_presets[SE_PRESET_CAVE] = prop9;
		m_presets[SE_PRESET_ARENA] = prop10;
		m_presets[SE_PRESET_HANGAR] = prop11;
		m_presets[SE_PRESET_CARPETEDHALLWAY] = prop12;
		m_presets[SE_PRESET_HALLWAY] = prop13;
		m_presets[SE_PRESET_STONECORRIDOR] = prop14;
		m_presets[SE_PRESET_ALLEY] = prop15;
		m_presets[SE_PRESET_FOREST] = prop16;
		m_presets[SE_PRESET_CITY] = prop17;
		m_presets[SE_PRESET_MOUNTAINS] = prop18;
		m_presets[SE_PRESET_QUARRY] = prop19;
		m_presets[SE_PRESET_PLAIN] = prop20;
		m_presets[SE_PRESET_PARKINGLOT] = prop21;
		m_presets[SE_PRESET_SEWERPIPE] = prop22;
		m_presets[SE_PRESET_UNDERWATER] = prop23;
		m_presets[SE_PRESET_DRUGGED] = prop24;
		m_presets[SE_PRESET_DIZZY] = prop25;
		m_presets[SE_PRESET_PSYCHOTIC] = prop26;
		m_presets[SE_PRESET_CASTLE_SMALLROOM] = prop27;
		m_presets[SE_PRESET_CASTLE_SHORTPASSAGE] = prop28;
		m_presets[SE_PRESET_CASTLE_MEDIUMROOM] = prop29;
		m_presets[SE_PRESET_CASTLE_LARGEROOM] = prop30;
		m_presets[SE_PRESET_CASTLE_LONGPASSAGE] = prop31;
		m_presets[SE_PRESET_CASTLE_HALL] = prop32;
		m_presets[SE_PRESET_CASTLE_CUPBOARD] = prop33;
		m_presets[SE_PRESET_CASTLE_COURTYARD] = prop34;
		m_presets[SE_PRESET_CASTLE_ALCOVE] = prop35;
		m_presets[SE_PRESET_FACTORY_SMALLROOM] = prop36;
		m_presets[SE_PRESET_FACTORY_SHORTPASSAGE] = prop37;
		m_presets[SE_PRESET_FACTORY_MEDIUMROOM] = prop38;
		m_presets[SE_PRESET_FACTORY_LARGEROOM] = prop39;
		m_presets[SE_PRESET_FACTORY_LONGPASSAGE] = prop40;
		m_presets[SE_PRESET_FACTORY_HALL] = prop41;
		m_presets[SE_PRESET_FACTORY_CUPBOARD] = prop42;
		m_presets[SE_PRESET_FACTORY_COURTYARD] = prop43;
		m_presets[SE_PRESET_FACTORY_ALCOVE] = prop44;
		m_presets[SE_PRESET_ICEPALACE_SMALLROOM] = prop45;
		m_presets[SE_PRESET_ICEPALACE_SHORTPASSAGE] = prop46;
		m_presets[SE_PRESET_ICEPALACE_MEDIUMROOM] = prop47;
		m_presets[SE_PRESET_ICEPALACE_LARGEROOM] = prop48;
		m_presets[SE_PRESET_ICEPALACE_LONGPASSAGE] = prop49;
		m_presets[SE_PRESET_ICEPALACE_HALL] = prop50;
		m_presets[SE_PRESET_ICEPALACE_CUPBOARD] = prop51;
		m_presets[SE_PRESET_ICEPALACE_COURTYARD] = prop52;
		m_presets[SE_PRESET_ICEPALACE_ALCOVE] = prop53;
		m_presets[SE_PRESET_SPACESTATION_SMALLROOM] = prop54;
		m_presets[SE_PRESET_SPACESTATION_SHORTPASSAGE] = prop55;
		m_presets[SE_PRESET_SPACESTATION_MEDIUMROOM] = prop56;
		m_presets[SE_PRESET_SPACESTATION_LARGEROOM] = prop57;
		m_presets[SE_PRESET_SPACESTATION_LONGPASSAGE] = prop58;
		m_presets[SE_PRESET_SPACESTATION_HALL] = prop59;
		m_presets[SE_PRESET_SPACESTATION_CUPBOARD] = prop60;
		m_presets[SE_PRESET_SPACESTATION_ALCOVE] = prop61;
		m_presets[SE_PRESET_WOODEN_SMALLROOM] = prop62;
		m_presets[SE_PRESET_WOODEN_SHORTPASSAGE] = prop63;
		m_presets[SE_PRESET_WOODEN_MEDIUMROOM] = prop64;
		m_presets[SE_PRESET_WOODEN_LARGEROOM] = prop65;
		m_presets[SE_PRESET_WOODEN_LONGPASSAGE] = prop66;
		m_presets[SE_PRESET_WOODEN_HALL] = prop67;
		m_presets[SE_PRESET_WOODEN_CUPBOARD] = prop68;
		m_presets[SE_PRESET_WOODEN_COURTYARD] = prop69;
		m_presets[SE_PRESET_WOODEN_ALCOVE] = prop70;
		m_presets[SE_PRESET_SPORT_EMPTYSTADIUM] = prop71;
		m_presets[SE_PRESET_SPORT_SQUASHCOURT] = prop72;
		m_presets[SE_PRESET_SPORT_SMALLSWIMMINGPOOL] = prop73;
		m_presets[SE_PRESET_SPORT_LARGESWIMMINGPOOL] = prop74;
		m_presets[SE_PRESET_SPORT_GYMNASIUM] = prop75;
		m_presets[SE_PRESET_SPORT_FULLSTADIUM] = prop76;
		m_presets[SE_PRESET_SPORT_STADIUMTANNOY] = prop77;
		m_presets[SE_PRESET_PREFAB_WORKSHOP] = prop78;
		m_presets[SE_PRESET_PREFAB_SCHOOLROOM] = prop79;
		m_presets[SE_PRESET_PREFAB_PRACTISEROOM] = prop80;
		m_presets[SE_PRESET_PREFAB_OUTHOUSE] = prop81;
		m_presets[SE_PRESET_PREFAB_CARAVAN] = prop82;
		m_presets[SE_PRESET_DOME_TOMB] = prop83;
		m_presets[SE_PRESET_PIPE_SMALL] = prop84;
		m_presets[SE_PRESET_DOME_SAINTPAULS] = prop85;
		m_presets[SE_PRESET_PIPE_LONGTHIN] = prop86;
		m_presets[SE_PRESET_PIPE_LARGE] = prop87;
		m_presets[SE_PRESET_PIPE_RESONANT] = prop88;
		m_presets[SE_PRESET_OUTDOORS_BACKYARD] = prop89;
		m_presets[SE_PRESET_OUTDOORS_ROLLINGPLAINS] = prop90;
		m_presets[SE_PRESET_OUTDOORS_DEEPCANYON] = prop91;
		m_presets[SE_PRESET_OUTDOORS_CREEK] = prop92;
		m_presets[SE_PRESET_OUTDOORS_VALLEY] = prop93;
		m_presets[SE_PRESET_MOOD_HEAVEN] = prop94;
		m_presets[SE_PRESET_MOOD_HELL] = prop95;
		m_presets[SE_PRESET_MOOD_MEMORY] = prop96;
		m_presets[SE_PRESET_DRIVING_COMMENTATOR] = prop97;
		m_presets[SE_PRESET_DRIVING_PITGARAGE] = prop98;
		m_presets[SE_PRESET_DRIVING_INCAR_RACER] = prop99;
		m_presets[SE_PRESET_DRIVING_INCAR_SPORTS] = prop100;
		m_presets[SE_PRESET_DRIVING_INCAR_LUXURY] = prop101;
		m_presets[SE_PRESET_DRIVING_FULLGRANDSTAND] = prop102;
		m_presets[SE_PRESET_DRIVING_EMPTYGRANDSTAND] = prop103;
		m_presets[SE_PRESET_DRIVING_TUNNEL] = prop104;
		m_presets[SE_PRESET_CITY_STREETS] = prop105;
		m_presets[SE_PRESET_CITY_SUBWAY] = prop106;
		m_presets[SE_PRESET_CITY_MUSEUM] = prop107;
		m_presets[SE_PRESET_CITY_LIBRARY] = prop108;
		m_presets[SE_PRESET_CITY_UNDERPASS] = prop109;
		m_presets[SE_PRESET_CITY_ABANDONED] = prop110;
		m_presets[SE_PRESET_DUSTYROOM] = prop111;
		m_presets[SE_PRESET_CHAPEL] = prop112;
		m_presets[SE_PRESET_SMALLWATERROOM] = prop113;
	}

} //FIFE
