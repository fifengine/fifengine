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
#include "util/log/logger.h"
#include "util/base/exception.h"

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
			FL_ERR(_log, LMsg() << "Failed initializing slot function pointers\n");
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
			FL_ERR(_log, LMsg() << "Failed initializing effect function pointers\n");
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
			FL_ERR(_log, LMsg() << "Failed initializing filter function pointers\n");
			return;
		}

		/*alGenAuxiliaryEffectSlots(1, &uiEffectSlot);
		alGenEffects(1, &uiEffect);

		alEffecti(uiEffect, AL_EFFECT_TYPE, AL_EFFECT_COMPRESSOR);
		alEffecti(uiEffect, AL_COMPRESSOR_ONOFF, 1);
		alAuxiliaryEffectSloti(uiEffectSlot, AL_EFFECTSLOT_EFFECT, uiEffect);*/

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

} //FIFE
