// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <map>
#include <utility>
#include <vector>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "audio/soundemitter.h"
#include "util/base/exception.h"
#include "util/log/logger.h"

#include "soundeffect.h"
#include "soundeffectmanager.h"
#include "soundfilter.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_AUDIO);

    // Effect Slots
    static LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots       = nullptr;
    static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots = nullptr;
    static LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot           = nullptr;
    static LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti             = nullptr;
    static LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv           = nullptr;
    static LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf             = nullptr;
    static LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv           = nullptr;
    static LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti       = nullptr;
    static LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv     = nullptr;
    static LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf       = nullptr;
    static LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv     = nullptr;

    // Effects
    static LPALGENEFFECTS alGenEffects       = nullptr;
    static LPALDELETEEFFECTS alDeleteEffects = nullptr;
    static LPALISEFFECT alIsEffect           = nullptr;
    static LPALEFFECTI alEffecti             = nullptr;
    static LPALEFFECTIV alEffectiv           = nullptr;
    static LPALEFFECTF alEffectf             = nullptr;
    static LPALEFFECTFV alEffectfv           = nullptr;
    static LPALGETEFFECTI alGetEffecti       = nullptr;
    static LPALGETEFFECTIV alGetEffectiv     = nullptr;
    static LPALGETEFFECTF alGetEffectf       = nullptr;
    static LPALGETEFFECTFV alGetEffectfv     = nullptr;

    // Filters
    static LPALGENFILTERS alGenFilters       = nullptr;
    static LPALDELETEFILTERS alDeleteFilters = nullptr;
    static LPALISFILTER alIsFilter           = nullptr;
    static LPALFILTERI alFilteri             = nullptr;
    static LPALFILTERIV alFilteriv           = nullptr;
    static LPALFILTERF alFilterf             = nullptr;
    static LPALFILTERFV alFilterfv           = nullptr;
    static LPALGETFILTERI alGetFilteri       = nullptr;
    static LPALGETFILTERIV alGetFilteriv     = nullptr;
    static LPALGETFILTERF alGetFilterf       = nullptr;
    static LPALGETFILTERFV alGetFilterfv     = nullptr;

    SoundEffectManager::SoundEffectManager() :
        m_device(nullptr), m_active(false), m_effectSlots{}, m_createdSlots(0), m_maxSlots(0)
    {
    }

    SoundEffectManager::~SoundEffectManager()
    {
        // SoundEmitters are destroyed beforehand
        for (auto& m_filter : m_filters) {
            delete m_filter;
        }
        for (auto& m_effect : m_effects) {
            delete m_effect;
        }
    }

    void SoundEffectManager::init(ALCdevice* device)
    {
        m_device = device;

        if (alcIsExtensionPresent(m_device, "ALC_EXT_EFX") == AL_FALSE) {
            LMsg msg;
            msg << "ALC_EXT_EFX not supported!\n";
            FL_WARN(_log, msg);
            return;
        }

        // Slot functions
        alGenAuxiliaryEffectSlots    = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
        alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
        alIsAuxiliaryEffectSlot      = (LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot");
        alAuxiliaryEffectSloti       = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
        alAuxiliaryEffectSlotiv      = (LPALAUXILIARYEFFECTSLOTIV)alGetProcAddress("alAuxiliaryEffectSlotiv");
        alAuxiliaryEffectSlotf       = (LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf");
        alAuxiliaryEffectSlotfv      = (LPALAUXILIARYEFFECTSLOTFV)alGetProcAddress("alAuxiliaryEffectSlotfv");
        alGetAuxiliaryEffectSloti    = (LPALGETAUXILIARYEFFECTSLOTI)alGetProcAddress("alGetAuxiliaryEffectSloti");
        alGetAuxiliaryEffectSlotiv   = (LPALGETAUXILIARYEFFECTSLOTIV)alGetProcAddress("alGetAuxiliaryEffectSlotiv");
        alGetAuxiliaryEffectSlotf    = (LPALGETAUXILIARYEFFECTSLOTF)alGetProcAddress("alGetAuxiliaryEffectSlotf");
        alGetAuxiliaryEffectSlotfv   = (LPALGETAUXILIARYEFFECTSLOTFV)alGetProcAddress("alGetAuxiliaryEffectSlotfv");
        if (alGenAuxiliaryEffectSlots == nullptr || alDeleteAuxiliaryEffectSlots == nullptr ||
            alIsAuxiliaryEffectSlot == nullptr || alAuxiliaryEffectSloti == nullptr ||
            alAuxiliaryEffectSlotiv == nullptr || alAuxiliaryEffectSlotf == nullptr ||
            alAuxiliaryEffectSlotfv == nullptr || alGetAuxiliaryEffectSloti == nullptr ||
            alGetAuxiliaryEffectSlotiv == nullptr || alGetAuxiliaryEffectSlotf == nullptr ||
            alGetAuxiliaryEffectSlotfv == nullptr) {
            LMsg msg;
            msg << "Failed initializing slot function pointers\n";
            FL_WARN(_log, msg);
            return;
        }

        // Effect functions
        alGenEffects    = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
        alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
        alIsEffect      = (LPALISEFFECT)alGetProcAddress("alIsEffect");
        alEffecti       = (LPALEFFECTI)alGetProcAddress("alEffecti");
        alEffectiv      = (LPALEFFECTIV)alGetProcAddress("alEffectiv");
        alEffectf       = (LPALEFFECTF)alGetProcAddress("alEffectf");
        alEffectfv      = (LPALEFFECTFV)alGetProcAddress("alEffectfv");
        alGetEffecti    = (LPALGETEFFECTI)alGetProcAddress("alGetEffecti");
        alGetEffectiv   = (LPALGETEFFECTIV)alGetProcAddress("alGetEffectiv");
        alGetEffectf    = (LPALGETEFFECTF)alGetProcAddress("alGetEffectf");
        alGetEffectfv   = (LPALGETEFFECTFV)alGetProcAddress("alGetEffectfv");
        if (alGenEffects == nullptr || alDeleteEffects == nullptr || alIsEffect == nullptr || alEffecti == nullptr ||
            alEffectiv == nullptr || alEffectf == nullptr || alEffectfv == nullptr || alGetEffecti == nullptr ||
            alGetEffectiv == nullptr || alGetEffectf == nullptr || alGetEffectfv == nullptr) {
            LMsg msg;
            msg << "Failed initializing effect function pointers\n";
            FL_WARN(_log, msg);
            return;
        }

        // Filter functions
        alGenFilters    = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
        alDeleteFilters = (LPALDELETEFILTERS)alGetProcAddress("alDeleteFilters");
        alIsFilter      = (LPALISFILTER)alGetProcAddress("alIsFilter");
        alFilteri       = (LPALFILTERI)alGetProcAddress("alFilteri");
        alFilteriv      = (LPALFILTERIV)alGetProcAddress("alFilteriv");
        alFilterf       = (LPALFILTERF)alGetProcAddress("alFilterf");
        alFilterfv      = (LPALFILTERFV)alGetProcAddress("alFilterfv");
        alGetFilteri    = (LPALGETFILTERI)alGetProcAddress("alGetFilteri");
        alGetFilteriv   = (LPALGETFILTERIV)alGetProcAddress("alGetFilteriv");
        alGetFilterf    = (LPALGETFILTERF)alGetProcAddress("alGetFilterf");
        alGetFilterfv   = (LPALGETFILTERFV)alGetProcAddress("alGetFilterfv");
        if (alGenFilters == nullptr || alDeleteFilters == nullptr || alIsFilter == nullptr || alFilteri == nullptr ||
            alFilteriv == nullptr || alFilterf == nullptr || alFilterfv == nullptr || alGetFilteri == nullptr ||
            alGetFilteriv == nullptr || alGetFilterf == nullptr || alGetFilterfv == nullptr) {
            LMsg msg;
            msg << "Failed initializing filter function pointers\n";
            FL_WARN(_log, msg);
            return;
        }

        m_active = true;

        // create max effect slots
        for (unsigned int& m_effectSlot : m_effectSlots) {
            alGenAuxiliaryEffectSlots(1, &m_effectSlot);
            if (alGetError() != AL_NO_ERROR) {
                break;
            }

            m_freeSlots.push(m_effectSlot);
            m_createdSlots++;
        }
        // fetch maximal slots per source
        alcGetIntegerv(m_device, ALC_MAX_AUXILIARY_SENDS, 1, &m_maxSlots);
        // prepares the presets
        createPresets();
    }

    bool SoundEffectManager::isActive() const
    {
        return m_active;
    }

    SoundEffect* SoundEffectManager::createSoundEffect(SoundEffectType type)
    {
        SoundEffect* effect = nullptr;
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
        if (effect != nullptr) {
            m_effects.push_back(effect);
        }
        return effect;
    }

    SoundEffect* SoundEffectManager::createSoundEffectPreset(SoundEffectPreset type)
    {
        auto it = m_presets.find(type);
        if (it == m_presets.end()) {
            return nullptr;
        }
        SoundEffect* effect = new EaxReverb();
        m_effects.push_back(effect);

        auto* reverb = dynamic_cast<EaxReverb*>(effect);
        reverb->loadPreset(it->second);
        return effect;
    }

    void SoundEffectManager::deleteSoundEffect(SoundEffect* effect)
    {
        disableSoundEffect(effect);

        if (effect->getFilter() != nullptr) {
            removeSoundFilterFromSoundEffect(effect, effect->getFilter());
        }

        auto it = std::ranges::find(m_effects, effect);
        if (it != m_effects.end()) {
            auto effectIt = m_effectEmitters.find(effect);
            if (effectIt != m_effectEmitters.end()) {
                for (auto* emitter : effectIt->second) {
                    emitter->removeEffect(effect);
                }
                m_effectEmitters.erase(effectIt);
            }

            delete *it;
            m_effects.erase(it);
        }
    }

    void SoundEffectManager::enableSoundEffect(SoundEffect* effect)
    {
        if (m_freeSlots.empty() || effect->isEnabled()) {
            if (m_freeSlots.empty()) {
                LMsg msg;
                msg << "No free auxiliary slot available";
                FL_WARN(_log, msg);
            }
            return;
        }

        ALuint const slot = m_freeSlots.front();
        m_freeSlots.pop();
        alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, effect->getEffectId());
        effect->setSlotId(slot);
        effect->setEnabled(true);
        auto effectIt = m_effectEmitters.find(effect);
        if (effectIt != m_effectEmitters.end()) {
            auto emitterIt = effectIt->second.begin();
            for (; emitterIt != effectIt->second.end(); ++emitterIt) {
                if (!(*emitterIt)->isActive()) {
                    continue;
                }
                activateEffect(effect, *emitterIt);
            }
        }
    }

    void SoundEffectManager::disableSoundEffect(SoundEffect* effect)
    {
        if (!effect->isEnabled()) {
            return;
        }
        alAuxiliaryEffectSloti(effect->getSlotId(), AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
        m_freeSlots.push(effect->getSlotId());
        effect->setSlotId(0);

        auto effectIt = m_effectEmitters.find(effect);
        if (effectIt != m_effectEmitters.end()) {
            auto emitterIt = effectIt->second.begin();
            for (; emitterIt != effectIt->second.end(); ++emitterIt) {
                deactivateEffect(effect, *emitterIt);
            }
        }
        effect->setEnabled(false);
    }

    void SoundEffectManager::addEmitterToSoundEffect(SoundEffect* effect, SoundEmitter* emitter)
    {
        if (std::cmp_equal(emitter->getEffectCount(), m_maxSlots)) {
            LMsg msg;
            msg << "Maximal effect number for SoundEmitter reached";
            FL_WARN(_log, msg);
            return;
        }
        m_effectEmitters[effect].push_back(emitter);
        emitter->addEffect(effect);
        if ((emitter - static_cast<int>(isActive())) != nullptr) {
            activateEffect(effect, emitter);
        }
    }

    void SoundEffectManager::removeEmitterFromSoundEffect(SoundEffect* effect, SoundEmitter* emitter)
    {
        auto effectIt = m_effectEmitters.find(effect);
        if (effectIt == m_effectEmitters.end()) {
            LMsg msg;
            msg << "SoundEmitter can not removed from unknown effect";
            FL_WARN(_log, msg);
            return;
        }
        bool found      = false;
        auto emitterIt  = effectIt->second.begin();
        auto emitterEnd = effectIt->second.end();
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
            LMsg msg;
            msg << "SoundEmitter could not be found for the given effect.";
            FL_WARN(_log, msg);
            return;
        }
    }

    void SoundEffectManager::addSoundFilterToSoundEffect(SoundEffect* effect, SoundFilter* filter)
    {
        if (effect->getFilter() != nullptr) {
            LMsg msg;
            msg << "SoundEffect already has a filter";
            FL_WARN(_log, msg);
            return;
        }
        effect->setFilter(filter);
        m_filterdEffects[filter].push_back(effect);
        if (effect->isEnabled()) {
            disableSoundEffect(effect);
            enableSoundEffect(effect);
        }
    }

    void SoundEffectManager::removeSoundFilterFromSoundEffect(SoundEffect* effect, SoundFilter* filter)
    {
        auto filterIt = m_filterdEffects.find(filter);
        if (filterIt == m_filterdEffects.end()) {
            LMsg msg;
            msg << "SoundEffect can not removed from unknown filter";
            FL_WARN(_log, msg);
            return;
        }
        bool found     = false;
        auto effectIt  = filterIt->second.begin();
        auto effectEnd = filterIt->second.end();
        while (effectIt != effectEnd) {
            if ((*effectIt) == effect) {
                effect->setFilter(nullptr);
                if (effect->isEnabled()) {
                    disableSoundEffect(effect);
                    enableSoundEffect(effect);
                }
                filterIt->second.erase(effectIt++);
                found = true;
            } else {
                ++effectIt;
            }
        }
        if (!found) {
            LMsg msg;
            msg << "SoundEffect could not be found for the given filter.";
            FL_WARN(_log, msg);
            return;
        }
    }

    void SoundEffectManager::activateEffect(SoundEffect* effect, SoundEmitter* emitter)
    {
        if (!effect->isEnabled()) {
            return;
        }
        auto number         = static_cast<ALuint>(emitter->getEffectNumber(effect));
        ALuint const filter = (effect->getFilter() != nullptr) ? effect->getFilter()->getFilterId() : AL_FILTER_NULL;
        alSource3i(emitter->getSource(), AL_AUXILIARY_SEND_FILTER, effect->getSlotId(), number, filter);
    }

    void SoundEffectManager::deactivateEffect(SoundEffect* effect, SoundEmitter* emitter)
    {
        if (!effect->isEnabled()) {
            return;
        }
        auto number = static_cast<ALuint>(emitter->getEffectNumber(effect));
        alSource3i(emitter->getSource(), AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, number, AL_FILTER_NULL);
    }

    SoundFilter* SoundEffectManager::createSoundFilter(SoundFilterType type)
    {
        auto* filter = new SoundFilter(type);
        m_filters.push_back(filter);
        return filter;
    }

    void SoundEffectManager::deleteSoundFilter(SoundFilter* filter)
    {
        disableDirectSoundFilter(filter);

        auto it = std::ranges::find(m_filters, filter);
        if (it != m_filters.end()) {
            auto filterIt = m_filterdEmitters.find(filter);
            if (filterIt != m_filterdEmitters.end()) {
                for (auto* emitter : filterIt->second) {
                    emitter->setDirectFilter(nullptr);
                }
                m_filterdEmitters.erase(filterIt);
            }

            auto filterItt = m_filterdEffects.find(filter);
            if (filterItt != m_filterdEffects.end()) {
                for (auto* effect : filterItt->second) {
                    effect->setFilter(nullptr);
                    if (effect->isEnabled()) {
                        disableSoundEffect(effect);
                        enableSoundEffect(effect);
                    }
                }
                m_filterdEffects.erase(filterItt);
            }

            delete *it;
            m_filters.erase(it);
        }
    }

    void SoundEffectManager::enableDirectSoundFilter(SoundFilter* filter)
    {
        if (filter->isEnabled()) {
            return;
        }
        filter->setEnabled(true);
        auto filterIt = m_filterdEmitters.find(filter);
        if (filterIt != m_filterdEmitters.end()) {
            auto emitterIt = filterIt->second.begin();
            for (; emitterIt != filterIt->second.end(); ++emitterIt) {
                if ((*emitterIt)->isActive()) {
                    activateFilter(filter, *emitterIt);
                }
            }
        }
    }

    void SoundEffectManager::disableDirectSoundFilter(SoundFilter* filter)
    {
        if (!filter->isEnabled()) {
            return;
        }
        auto filterIt = m_filterdEmitters.find(filter);
        if (filterIt != m_filterdEmitters.end()) {
            auto emitterIt = filterIt->second.begin();
            for (; emitterIt != filterIt->second.end(); ++emitterIt) {
                if ((*emitterIt)->isActive()) {
                    deactivateFilter(filter, *emitterIt);
                }
            }
        }
        filter->setEnabled(false);
    }

    void SoundEffectManager::addEmitterToDirectSoundFilter(SoundFilter* filter, SoundEmitter* emitter)
    {
        if (emitter->getDirectFilter() != nullptr) {
            LMsg msg;
            msg << "SoundEmitter already has a direct filter";
            FL_WARN(_log, msg);
            return;
        }
        emitter->setDirectFilter(filter);
        m_filterdEmitters[filter].push_back(emitter);
        if (emitter->isActive()) {
            activateFilter(filter, emitter);
        }
    }

    void SoundEffectManager::removeEmitterFromDirectSoundFilter(SoundFilter* filter, SoundEmitter* emitter)
    {
        auto filterIt = m_filterdEmitters.find(filter);
        if (filterIt == m_filterdEmitters.end()) {
            LMsg msg;
            msg << "SoundEmitter can not removed from unknown filter";
            FL_WARN(_log, msg);
            return;
        }
        bool found      = false;
        auto emitterIt  = filterIt->second.begin();
        auto emitterEnd = filterIt->second.end();
        while (emitterIt != emitterEnd) {
            if ((*emitterIt) == emitter) {
                if (emitter->isActive()) {
                    deactivateFilter(filter, emitter);
                }
                emitter->setDirectFilter(nullptr);
                filterIt->second.erase(emitterIt++);
                found = true;
            } else {
                ++emitterIt;
            }
        }
        if (!found) {
            LMsg msg;
            msg << "SoundEmitter could not be found for the given filter.";
            FL_WARN(_log, msg);
            return;
        }
    }

    void SoundEffectManager::activateFilter(const SoundFilter* filter, const SoundEmitter* emitter)
    {
        if (filter->isEnabled()) {
            alSourcei(emitter->getSource(), AL_DIRECT_FILTER, filter->getFilterId());
        }
    }

    void SoundEffectManager::deactivateFilter(const SoundFilter* filter, const SoundEmitter* emitter)
    {
        if (filter->isEnabled()) {
            alSourcei(emitter->getSource(), AL_DIRECT_FILTER, AL_FILTER_NULL);
        }
    }

    void SoundEffectManager::createPresets()
    {
        EFXEAXREVERBPROPERTIES const prop1   = EFX_REVERB_PRESET_GENERIC;
        EFXEAXREVERBPROPERTIES const prop2   = EFX_REVERB_PRESET_PADDEDCELL;
        EFXEAXREVERBPROPERTIES const prop3   = EFX_REVERB_PRESET_ROOM;
        EFXEAXREVERBPROPERTIES const prop4   = EFX_REVERB_PRESET_BATHROOM;
        EFXEAXREVERBPROPERTIES const prop5   = EFX_REVERB_PRESET_LIVINGROOM;
        EFXEAXREVERBPROPERTIES const prop6   = EFX_REVERB_PRESET_STONEROOM;
        EFXEAXREVERBPROPERTIES const prop7   = EFX_REVERB_PRESET_AUDITORIUM;
        EFXEAXREVERBPROPERTIES const prop8   = EFX_REVERB_PRESET_CONCERTHALL;
        EFXEAXREVERBPROPERTIES const prop9   = EFX_REVERB_PRESET_CAVE;
        EFXEAXREVERBPROPERTIES const prop10  = EFX_REVERB_PRESET_ARENA;
        EFXEAXREVERBPROPERTIES const prop11  = EFX_REVERB_PRESET_HANGAR;
        EFXEAXREVERBPROPERTIES const prop12  = EFX_REVERB_PRESET_CARPETEDHALLWAY;
        EFXEAXREVERBPROPERTIES const prop13  = EFX_REVERB_PRESET_HALLWAY;
        EFXEAXREVERBPROPERTIES const prop14  = EFX_REVERB_PRESET_STONECORRIDOR;
        EFXEAXREVERBPROPERTIES const prop15  = EFX_REVERB_PRESET_ALLEY;
        EFXEAXREVERBPROPERTIES const prop16  = EFX_REVERB_PRESET_FOREST;
        EFXEAXREVERBPROPERTIES const prop17  = EFX_REVERB_PRESET_CITY;
        EFXEAXREVERBPROPERTIES const prop18  = EFX_REVERB_PRESET_MOUNTAINS;
        EFXEAXREVERBPROPERTIES const prop19  = EFX_REVERB_PRESET_QUARRY;
        EFXEAXREVERBPROPERTIES const prop20  = EFX_REVERB_PRESET_PLAIN;
        EFXEAXREVERBPROPERTIES const prop21  = EFX_REVERB_PRESET_PARKINGLOT;
        EFXEAXREVERBPROPERTIES const prop22  = EFX_REVERB_PRESET_SEWERPIPE;
        EFXEAXREVERBPROPERTIES const prop23  = EFX_REVERB_PRESET_UNDERWATER;
        EFXEAXREVERBPROPERTIES const prop24  = EFX_REVERB_PRESET_DRUGGED;
        EFXEAXREVERBPROPERTIES const prop25  = EFX_REVERB_PRESET_DIZZY;
        EFXEAXREVERBPROPERTIES const prop26  = EFX_REVERB_PRESET_PSYCHOTIC;
        EFXEAXREVERBPROPERTIES const prop27  = EFX_REVERB_PRESET_CASTLE_SMALLROOM;
        EFXEAXREVERBPROPERTIES const prop28  = EFX_REVERB_PRESET_CASTLE_SHORTPASSAGE;
        EFXEAXREVERBPROPERTIES const prop29  = EFX_REVERB_PRESET_CASTLE_MEDIUMROOM;
        EFXEAXREVERBPROPERTIES const prop30  = EFX_REVERB_PRESET_CASTLE_LARGEROOM;
        EFXEAXREVERBPROPERTIES const prop31  = EFX_REVERB_PRESET_CASTLE_LONGPASSAGE;
        EFXEAXREVERBPROPERTIES const prop32  = EFX_REVERB_PRESET_CASTLE_HALL;
        EFXEAXREVERBPROPERTIES const prop33  = EFX_REVERB_PRESET_CASTLE_CUPBOARD;
        EFXEAXREVERBPROPERTIES const prop34  = EFX_REVERB_PRESET_CASTLE_COURTYARD;
        EFXEAXREVERBPROPERTIES const prop35  = EFX_REVERB_PRESET_CASTLE_ALCOVE;
        EFXEAXREVERBPROPERTIES const prop36  = EFX_REVERB_PRESET_FACTORY_SMALLROOM;
        EFXEAXREVERBPROPERTIES const prop37  = EFX_REVERB_PRESET_FACTORY_SHORTPASSAGE;
        EFXEAXREVERBPROPERTIES const prop38  = EFX_REVERB_PRESET_FACTORY_MEDIUMROOM;
        EFXEAXREVERBPROPERTIES const prop39  = EFX_REVERB_PRESET_FACTORY_LARGEROOM;
        EFXEAXREVERBPROPERTIES const prop40  = EFX_REVERB_PRESET_FACTORY_LONGPASSAGE;
        EFXEAXREVERBPROPERTIES const prop41  = EFX_REVERB_PRESET_FACTORY_HALL;
        EFXEAXREVERBPROPERTIES const prop42  = EFX_REVERB_PRESET_FACTORY_CUPBOARD;
        EFXEAXREVERBPROPERTIES const prop43  = EFX_REVERB_PRESET_FACTORY_COURTYARD;
        EFXEAXREVERBPROPERTIES const prop44  = EFX_REVERB_PRESET_FACTORY_ALCOVE;
        EFXEAXREVERBPROPERTIES const prop45  = EFX_REVERB_PRESET_ICEPALACE_SMALLROOM;
        EFXEAXREVERBPROPERTIES const prop46  = EFX_REVERB_PRESET_ICEPALACE_SHORTPASSAGE;
        EFXEAXREVERBPROPERTIES const prop47  = EFX_REVERB_PRESET_ICEPALACE_MEDIUMROOM;
        EFXEAXREVERBPROPERTIES const prop48  = EFX_REVERB_PRESET_ICEPALACE_LARGEROOM;
        EFXEAXREVERBPROPERTIES const prop49  = EFX_REVERB_PRESET_ICEPALACE_LONGPASSAGE;
        EFXEAXREVERBPROPERTIES const prop50  = EFX_REVERB_PRESET_ICEPALACE_HALL;
        EFXEAXREVERBPROPERTIES const prop51  = EFX_REVERB_PRESET_ICEPALACE_CUPBOARD;
        EFXEAXREVERBPROPERTIES const prop52  = EFX_REVERB_PRESET_ICEPALACE_COURTYARD;
        EFXEAXREVERBPROPERTIES const prop53  = EFX_REVERB_PRESET_ICEPALACE_ALCOVE;
        EFXEAXREVERBPROPERTIES const prop54  = EFX_REVERB_PRESET_SPACESTATION_SMALLROOM;
        EFXEAXREVERBPROPERTIES const prop55  = EFX_REVERB_PRESET_SPACESTATION_SHORTPASSAGE;
        EFXEAXREVERBPROPERTIES const prop56  = EFX_REVERB_PRESET_SPACESTATION_MEDIUMROOM;
        EFXEAXREVERBPROPERTIES const prop57  = EFX_REVERB_PRESET_SPACESTATION_LARGEROOM;
        EFXEAXREVERBPROPERTIES const prop58  = EFX_REVERB_PRESET_SPACESTATION_LONGPASSAGE;
        EFXEAXREVERBPROPERTIES const prop59  = EFX_REVERB_PRESET_SPACESTATION_HALL;
        EFXEAXREVERBPROPERTIES const prop60  = EFX_REVERB_PRESET_SPACESTATION_CUPBOARD;
        EFXEAXREVERBPROPERTIES const prop61  = EFX_REVERB_PRESET_SPACESTATION_ALCOVE;
        EFXEAXREVERBPROPERTIES const prop62  = EFX_REVERB_PRESET_WOODEN_SMALLROOM;
        EFXEAXREVERBPROPERTIES const prop63  = EFX_REVERB_PRESET_WOODEN_SHORTPASSAGE;
        EFXEAXREVERBPROPERTIES const prop64  = EFX_REVERB_PRESET_WOODEN_MEDIUMROOM;
        EFXEAXREVERBPROPERTIES const prop65  = EFX_REVERB_PRESET_WOODEN_LARGEROOM;
        EFXEAXREVERBPROPERTIES const prop66  = EFX_REVERB_PRESET_WOODEN_LONGPASSAGE;
        EFXEAXREVERBPROPERTIES const prop67  = EFX_REVERB_PRESET_WOODEN_HALL;
        EFXEAXREVERBPROPERTIES const prop68  = EFX_REVERB_PRESET_WOODEN_CUPBOARD;
        EFXEAXREVERBPROPERTIES const prop69  = EFX_REVERB_PRESET_WOODEN_COURTYARD;
        EFXEAXREVERBPROPERTIES const prop70  = EFX_REVERB_PRESET_WOODEN_ALCOVE;
        EFXEAXREVERBPROPERTIES const prop71  = EFX_REVERB_PRESET_SPORT_EMPTYSTADIUM;
        EFXEAXREVERBPROPERTIES const prop72  = EFX_REVERB_PRESET_SPORT_SQUASHCOURT;
        EFXEAXREVERBPROPERTIES const prop73  = EFX_REVERB_PRESET_SPORT_SMALLSWIMMINGPOOL;
        EFXEAXREVERBPROPERTIES const prop74  = EFX_REVERB_PRESET_SPORT_LARGESWIMMINGPOOL;
        EFXEAXREVERBPROPERTIES const prop75  = EFX_REVERB_PRESET_SPORT_GYMNASIUM;
        EFXEAXREVERBPROPERTIES const prop76  = EFX_REVERB_PRESET_SPORT_FULLSTADIUM;
        EFXEAXREVERBPROPERTIES const prop77  = EFX_REVERB_PRESET_SPORT_STADIUMTANNOY;
        EFXEAXREVERBPROPERTIES const prop78  = EFX_REVERB_PRESET_PREFAB_WORKSHOP;
        EFXEAXREVERBPROPERTIES const prop79  = EFX_REVERB_PRESET_PREFAB_SCHOOLROOM;
        EFXEAXREVERBPROPERTIES const prop80  = EFX_REVERB_PRESET_PREFAB_PRACTISEROOM;
        EFXEAXREVERBPROPERTIES const prop81  = EFX_REVERB_PRESET_PREFAB_OUTHOUSE;
        EFXEAXREVERBPROPERTIES const prop82  = EFX_REVERB_PRESET_PREFAB_CARAVAN;
        EFXEAXREVERBPROPERTIES const prop83  = EFX_REVERB_PRESET_DOME_TOMB;
        EFXEAXREVERBPROPERTIES const prop84  = EFX_REVERB_PRESET_PIPE_SMALL;
        EFXEAXREVERBPROPERTIES const prop85  = EFX_REVERB_PRESET_DOME_SAINTPAULS;
        EFXEAXREVERBPROPERTIES const prop86  = EFX_REVERB_PRESET_PIPE_LONGTHIN;
        EFXEAXREVERBPROPERTIES const prop87  = EFX_REVERB_PRESET_PIPE_LARGE;
        EFXEAXREVERBPROPERTIES const prop88  = EFX_REVERB_PRESET_PIPE_RESONANT;
        EFXEAXREVERBPROPERTIES const prop89  = EFX_REVERB_PRESET_OUTDOORS_BACKYARD;
        EFXEAXREVERBPROPERTIES const prop90  = EFX_REVERB_PRESET_OUTDOORS_ROLLINGPLAINS;
        EFXEAXREVERBPROPERTIES const prop91  = EFX_REVERB_PRESET_OUTDOORS_DEEPCANYON;
        EFXEAXREVERBPROPERTIES const prop92  = EFX_REVERB_PRESET_OUTDOORS_CREEK;
        EFXEAXREVERBPROPERTIES const prop93  = EFX_REVERB_PRESET_OUTDOORS_VALLEY;
        EFXEAXREVERBPROPERTIES const prop94  = EFX_REVERB_PRESET_MOOD_HEAVEN;
        EFXEAXREVERBPROPERTIES const prop95  = EFX_REVERB_PRESET_MOOD_HELL;
        EFXEAXREVERBPROPERTIES const prop96  = EFX_REVERB_PRESET_MOOD_MEMORY;
        EFXEAXREVERBPROPERTIES const prop97  = EFX_REVERB_PRESET_DRIVING_COMMENTATOR;
        EFXEAXREVERBPROPERTIES const prop98  = EFX_REVERB_PRESET_DRIVING_PITGARAGE;
        EFXEAXREVERBPROPERTIES const prop99  = EFX_REVERB_PRESET_DRIVING_INCAR_RACER;
        EFXEAXREVERBPROPERTIES const prop100 = EFX_REVERB_PRESET_DRIVING_INCAR_SPORTS;
        EFXEAXREVERBPROPERTIES const prop101 = EFX_REVERB_PRESET_DRIVING_INCAR_LUXURY;
        EFXEAXREVERBPROPERTIES const prop102 = EFX_REVERB_PRESET_DRIVING_FULLGRANDSTAND;
        EFXEAXREVERBPROPERTIES const prop103 = EFX_REVERB_PRESET_DRIVING_EMPTYGRANDSTAND;
        EFXEAXREVERBPROPERTIES const prop104 = EFX_REVERB_PRESET_DRIVING_TUNNEL;
        EFXEAXREVERBPROPERTIES const prop105 = EFX_REVERB_PRESET_CITY_STREETS;
        EFXEAXREVERBPROPERTIES const prop106 = EFX_REVERB_PRESET_CITY_SUBWAY;
        EFXEAXREVERBPROPERTIES const prop107 = EFX_REVERB_PRESET_CITY_MUSEUM;
        EFXEAXREVERBPROPERTIES const prop108 = EFX_REVERB_PRESET_CITY_LIBRARY;
        EFXEAXREVERBPROPERTIES const prop109 = EFX_REVERB_PRESET_CITY_UNDERPASS;
        EFXEAXREVERBPROPERTIES const prop110 = EFX_REVERB_PRESET_CITY_ABANDONED;
        EFXEAXREVERBPROPERTIES const prop111 = EFX_REVERB_PRESET_DUSTYROOM;
        EFXEAXREVERBPROPERTIES const prop112 = EFX_REVERB_PRESET_CHAPEL;
        EFXEAXREVERBPROPERTIES const prop113 = EFX_REVERB_PRESET_SMALLWATERROOM;

        m_presets[SE_PRESET_GENERIC]                   = prop1;
        m_presets[SE_PRESET_PADDEDCELL]                = prop2;
        m_presets[SE_PRESET_ROOM]                      = prop3;
        m_presets[SE_PRESET_BATHROOM]                  = prop4;
        m_presets[SE_PRESET_LIVINGROOM]                = prop5;
        m_presets[SE_PRESET_STONEROOM]                 = prop6;
        m_presets[SE_PRESET_AUDITORIUM]                = prop7;
        m_presets[SE_PRESET_CONCERTHALL]               = prop8;
        m_presets[SE_PRESET_CAVE]                      = prop9;
        m_presets[SE_PRESET_ARENA]                     = prop10;
        m_presets[SE_PRESET_HANGAR]                    = prop11;
        m_presets[SE_PRESET_CARPETEDHALLWAY]           = prop12;
        m_presets[SE_PRESET_HALLWAY]                   = prop13;
        m_presets[SE_PRESET_STONECORRIDOR]             = prop14;
        m_presets[SE_PRESET_ALLEY]                     = prop15;
        m_presets[SE_PRESET_FOREST]                    = prop16;
        m_presets[SE_PRESET_CITY]                      = prop17;
        m_presets[SE_PRESET_MOUNTAINS]                 = prop18;
        m_presets[SE_PRESET_QUARRY]                    = prop19;
        m_presets[SE_PRESET_PLAIN]                     = prop20;
        m_presets[SE_PRESET_PARKINGLOT]                = prop21;
        m_presets[SE_PRESET_SEWERPIPE]                 = prop22;
        m_presets[SE_PRESET_UNDERWATER]                = prop23;
        m_presets[SE_PRESET_DRUGGED]                   = prop24;
        m_presets[SE_PRESET_DIZZY]                     = prop25;
        m_presets[SE_PRESET_PSYCHOTIC]                 = prop26;
        m_presets[SE_PRESET_CASTLE_SMALLROOM]          = prop27;
        m_presets[SE_PRESET_CASTLE_SHORTPASSAGE]       = prop28;
        m_presets[SE_PRESET_CASTLE_MEDIUMROOM]         = prop29;
        m_presets[SE_PRESET_CASTLE_LARGEROOM]          = prop30;
        m_presets[SE_PRESET_CASTLE_LONGPASSAGE]        = prop31;
        m_presets[SE_PRESET_CASTLE_HALL]               = prop32;
        m_presets[SE_PRESET_CASTLE_CUPBOARD]           = prop33;
        m_presets[SE_PRESET_CASTLE_COURTYARD]          = prop34;
        m_presets[SE_PRESET_CASTLE_ALCOVE]             = prop35;
        m_presets[SE_PRESET_FACTORY_SMALLROOM]         = prop36;
        m_presets[SE_PRESET_FACTORY_SHORTPASSAGE]      = prop37;
        m_presets[SE_PRESET_FACTORY_MEDIUMROOM]        = prop38;
        m_presets[SE_PRESET_FACTORY_LARGEROOM]         = prop39;
        m_presets[SE_PRESET_FACTORY_LONGPASSAGE]       = prop40;
        m_presets[SE_PRESET_FACTORY_HALL]              = prop41;
        m_presets[SE_PRESET_FACTORY_CUPBOARD]          = prop42;
        m_presets[SE_PRESET_FACTORY_COURTYARD]         = prop43;
        m_presets[SE_PRESET_FACTORY_ALCOVE]            = prop44;
        m_presets[SE_PRESET_ICEPALACE_SMALLROOM]       = prop45;
        m_presets[SE_PRESET_ICEPALACE_SHORTPASSAGE]    = prop46;
        m_presets[SE_PRESET_ICEPALACE_MEDIUMROOM]      = prop47;
        m_presets[SE_PRESET_ICEPALACE_LARGEROOM]       = prop48;
        m_presets[SE_PRESET_ICEPALACE_LONGPASSAGE]     = prop49;
        m_presets[SE_PRESET_ICEPALACE_HALL]            = prop50;
        m_presets[SE_PRESET_ICEPALACE_CUPBOARD]        = prop51;
        m_presets[SE_PRESET_ICEPALACE_COURTYARD]       = prop52;
        m_presets[SE_PRESET_ICEPALACE_ALCOVE]          = prop53;
        m_presets[SE_PRESET_SPACESTATION_SMALLROOM]    = prop54;
        m_presets[SE_PRESET_SPACESTATION_SHORTPASSAGE] = prop55;
        m_presets[SE_PRESET_SPACESTATION_MEDIUMROOM]   = prop56;
        m_presets[SE_PRESET_SPACESTATION_LARGEROOM]    = prop57;
        m_presets[SE_PRESET_SPACESTATION_LONGPASSAGE]  = prop58;
        m_presets[SE_PRESET_SPACESTATION_HALL]         = prop59;
        m_presets[SE_PRESET_SPACESTATION_CUPBOARD]     = prop60;
        m_presets[SE_PRESET_SPACESTATION_ALCOVE]       = prop61;
        m_presets[SE_PRESET_WOODEN_SMALLROOM]          = prop62;
        m_presets[SE_PRESET_WOODEN_SHORTPASSAGE]       = prop63;
        m_presets[SE_PRESET_WOODEN_MEDIUMROOM]         = prop64;
        m_presets[SE_PRESET_WOODEN_LARGEROOM]          = prop65;
        m_presets[SE_PRESET_WOODEN_LONGPASSAGE]        = prop66;
        m_presets[SE_PRESET_WOODEN_HALL]               = prop67;
        m_presets[SE_PRESET_WOODEN_CUPBOARD]           = prop68;
        m_presets[SE_PRESET_WOODEN_COURTYARD]          = prop69;
        m_presets[SE_PRESET_WOODEN_ALCOVE]             = prop70;
        m_presets[SE_PRESET_SPORT_EMPTYSTADIUM]        = prop71;
        m_presets[SE_PRESET_SPORT_SQUASHCOURT]         = prop72;
        m_presets[SE_PRESET_SPORT_SMALLSWIMMINGPOOL]   = prop73;
        m_presets[SE_PRESET_SPORT_LARGESWIMMINGPOOL]   = prop74;
        m_presets[SE_PRESET_SPORT_GYMNASIUM]           = prop75;
        m_presets[SE_PRESET_SPORT_FULLSTADIUM]         = prop76;
        m_presets[SE_PRESET_SPORT_STADIUMTANNOY]       = prop77;
        m_presets[SE_PRESET_PREFAB_WORKSHOP]           = prop78;
        m_presets[SE_PRESET_PREFAB_SCHOOLROOM]         = prop79;
        m_presets[SE_PRESET_PREFAB_PRACTISEROOM]       = prop80;
        m_presets[SE_PRESET_PREFAB_OUTHOUSE]           = prop81;
        m_presets[SE_PRESET_PREFAB_CARAVAN]            = prop82;
        m_presets[SE_PRESET_DOME_TOMB]                 = prop83;
        m_presets[SE_PRESET_PIPE_SMALL]                = prop84;
        m_presets[SE_PRESET_DOME_SAINTPAULS]           = prop85;
        m_presets[SE_PRESET_PIPE_LONGTHIN]             = prop86;
        m_presets[SE_PRESET_PIPE_LARGE]                = prop87;
        m_presets[SE_PRESET_PIPE_RESONANT]             = prop88;
        m_presets[SE_PRESET_OUTDOORS_BACKYARD]         = prop89;
        m_presets[SE_PRESET_OUTDOORS_ROLLINGPLAINS]    = prop90;
        m_presets[SE_PRESET_OUTDOORS_DEEPCANYON]       = prop91;
        m_presets[SE_PRESET_OUTDOORS_CREEK]            = prop92;
        m_presets[SE_PRESET_OUTDOORS_VALLEY]           = prop93;
        m_presets[SE_PRESET_MOOD_HEAVEN]               = prop94;
        m_presets[SE_PRESET_MOOD_HELL]                 = prop95;
        m_presets[SE_PRESET_MOOD_MEMORY]               = prop96;
        m_presets[SE_PRESET_DRIVING_COMMENTATOR]       = prop97;
        m_presets[SE_PRESET_DRIVING_PITGARAGE]         = prop98;
        m_presets[SE_PRESET_DRIVING_INCAR_RACER]       = prop99;
        m_presets[SE_PRESET_DRIVING_INCAR_SPORTS]      = prop100;
        m_presets[SE_PRESET_DRIVING_INCAR_LUXURY]      = prop101;
        m_presets[SE_PRESET_DRIVING_FULLGRANDSTAND]    = prop102;
        m_presets[SE_PRESET_DRIVING_EMPTYGRANDSTAND]   = prop103;
        m_presets[SE_PRESET_DRIVING_TUNNEL]            = prop104;
        m_presets[SE_PRESET_CITY_STREETS]              = prop105;
        m_presets[SE_PRESET_CITY_SUBWAY]               = prop106;
        m_presets[SE_PRESET_CITY_MUSEUM]               = prop107;
        m_presets[SE_PRESET_CITY_LIBRARY]              = prop108;
        m_presets[SE_PRESET_CITY_UNDERPASS]            = prop109;
        m_presets[SE_PRESET_CITY_ABANDONED]            = prop110;
        m_presets[SE_PRESET_DUSTYROOM]                 = prop111;
        m_presets[SE_PRESET_CHAPEL]                    = prop112;
        m_presets[SE_PRESET_SMALLWATERROOM]            = prop113;
    }

} // namespace FIFE
