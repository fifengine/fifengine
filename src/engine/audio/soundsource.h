// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_SOUNDSOURCE_H
#define FIFE_SOUNDSOURCE_H

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE
{

    class ActionAudio;
    class Instance;
    class SoundChangeListener;
    class SoundEmitter;

    /** Interface class between Instance / ActionAudio and SoundEmitter.
     */
    class SoundSource
    {
    public:
        SoundSource(Instance* instance);
        ~SoundSource();

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