// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_SOUNDSOURCE_H
#define FIFE_SOUNDSOURCE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
// 3rd party library includes

// FIFE includes

namespace FIFE
{

    class ActionAudio;
    class Instance;
    class SoundChangeListener;
    class SoundEmitter;

    /** Interface class between Instance / ActionAudio and SoundEmitter.
     */
    class /*FIFE_API*/ SoundSource
    {
        public:
            explicit SoundSource(Instance* instance);
            ~SoundSource();

            SoundSource(SoundSource const &)            = delete;
            SoundSource& operator=(SoundSource const &) = delete;

            /** Sets the ActionAudio. Owned by Object.
             */
            void setActionAudio(ActionAudio* audio);

            /** Return ActionAudio. Owned by Object.
             */
            ActionAudio* getActionAudio() const;

            /** Sets the positon of the SoundEmitter, called from Instance.
             */
            void setPosition();

            /** Sets the direction of the SoundEmitter, called from Instance.
             */
            void setDirection();

        private:
            /** Moves data from ActionAudio to SoundEmitter.
             */
            void updateSoundEmitter();

            //! Associated Instance
            Instance* m_instance;
            //! Actual ActionAudio
            ActionAudio* m_audio;
            //! Related SoundEmitter
            SoundEmitter* m_emitter;
            //! InstanceChangeListener for position and direction
            SoundChangeListener* m_listener;
    };
} // namespace FIFE

#endif
