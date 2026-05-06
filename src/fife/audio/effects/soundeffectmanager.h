// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_SOUNDEFFECTMANAGER_H
#define FIFE_SOUNDEFFECTMANAGER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <map>
#include <queue>
#include <vector>
// 3rd party library includes

// FIFE includes
#include "audio/fife_openal.h"
#include "audio/soundconfig.h"

namespace FIFE
{

    class SoundEffect;
    class SoundFilter;
    class SoundEmitter;

    class /*FIFE_API*/ SoundEffectManager
    {
        public:
            /** Constructor
             */
            SoundEffectManager();

            /** Destructor
             */
            ~SoundEffectManager();

            /** Initializes the effect system
             */
            void init(ALCdevice* device);

            /** Returns true if sound effect module is active
             */
            bool isActive() const;

            /** Creates SoundEffect of the specific type.
             * @param type See SoundEffectType
             */
            SoundEffect* createSoundEffect(SoundEffectType type);

            /** Creates EaxReverb SoundEffect and loads the specific preset type.
             * @param type See SoundEffectPreset
             */
            SoundEffect* createSoundEffectPreset(SoundEffectPreset type);

            /** Deletes given SoundEffect.
             * Deactivates for SoundEmitters are automatically done.
             */
            void deleteSoundEffect(SoundEffect* effect);

            /** Enables given SoundEffect.
             */
            void enableSoundEffect(SoundEffect* effect);

            /** Disables given SoundEffect.
             */
            void disableSoundEffect(SoundEffect* effect);

            /** Adds given SoundEmitter to the specific SoundEffect.
             * Note: A SoundEmitter can only have a limited number of effects, usually 4.
             */
            void addEmitterToSoundEffect(SoundEffect* effect, SoundEmitter* emitter);

            /** Removes given SoundEmitter from the specific SoundEffect.
             */
            void removeEmitterFromSoundEffect(SoundEffect* effect, SoundEmitter* emitter);

            /** Adds given SoundFilter to the SoundEffect.
             * Note: A SoundEffect can only have one SoundFilter.
             */
            void addSoundFilterToSoundEffect(SoundEffect* effect, SoundFilter* filter);

            /** Removes given SoundFilter from the SoundEffect.
             */
            void removeSoundFilterFromSoundEffect(SoundEffect* effect, SoundFilter* filter);

            /** Internal function to do the OpenAL calls to activate the SoundEffect for the SoundEmitter.
             */
            void activateEffect(SoundEffect* effect, SoundEmitter* emitter);

            /** Internal function to do the OpenAL calls to deactivate the SoundEffect for the SoundEmitter.
             */
            void deactivateEffect(SoundEffect* effect, SoundEmitter* emitter);

            /** Creates SoundFilter of the specific type.
             * @param type See SoundFilterType
             */
            SoundFilter* createSoundFilter(SoundFilterType type);

            /** Deletes given SoundFilter.
             * Deactivates for SoundEmitters and Effects are automatically done.
             */
            void deleteSoundFilter(SoundFilter* filter);

            /** Enables given direct SoundFilter. Only SoundFilters for SoundEmitters are meant.
             */
            void enableDirectSoundFilter(SoundFilter* filter);

            /** Disables given SoundFilter. Only SoundFilters for SoundEmitters are meant.
             */
            void disableDirectSoundFilter(SoundFilter* filter);

            /** Adds given SoundEmitter to the specific direct SoundFilter
             * Note: A SoundEmitter can only have one direct filter.
             */
            void addEmitterToDirectSoundFilter(SoundFilter* filter, SoundEmitter* emitter);

            /** Removes given SoundEmitter from the specific direct SoundFilter.
             */
            void removeEmitterFromDirectSoundFilter(SoundFilter* filter, SoundEmitter* emitter);

            /** Internal function to do the OpenAL calls to activate the SoundFilter for the SoundEmitter.
             */
            void activateFilter(SoundFilter const * filter, SoundEmitter const * emitter);

            /** Internal function to do the OpenAL calls to deactivate the SoundFilter for the SoundEmitter.
             */
            void deactivateFilter(SoundFilter const * filter, SoundEmitter const * emitter);

        private:
            //! Inital the presets
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
            //! Holds all SoundEffects
            std::vector<SoundEffect*> m_effects;
            //! Holds SoundEffects together with the added SoundEmitters
            using SoundEffectEmitterMap = std::map<SoundEffect*, std::vector<SoundEmitter*>>;
            SoundEffectEmitterMap m_effectEmitters;
            //! Holds all SoundFilters
            std::vector<SoundFilter*> m_filters;
            //! Holds SoundFilters together with the added SoundEmitters
            using SoundFilterEmitterMap = std::map<SoundFilter*, std::vector<SoundEmitter*>>;
            SoundFilterEmitterMap m_filterdEmitters;
            //! Holds SoundFilters together with the added SoundEffects
            using SoundFilterEffectMap = std::map<SoundFilter*, std::vector<SoundEffect*>>;
            SoundFilterEffectMap m_filterdEffects;
            //! Establishes the relationship between SoundEffectPreset and EFXEAXREVERBPROPERTIES
            std::map<SoundEffectPreset, EFXEAXREVERBPROPERTIES> m_presets;
    };
} // namespace FIFE
#endif
