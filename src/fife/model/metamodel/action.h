// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MODEL_METAMODEL_ACTION_H
#define FIFE_MODEL_METAMODEL_ACTION_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <list>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "ivisual.h"
#include "util/base/fifeclass.h"
#include "util/math/angles.h"

namespace FIFE
{
    class ActionAudio;

    class FIFE_API Action : public FifeClass
    {
        public:
            /** Constructor
             * Actions are created by calling addAction from object, thus
             * this method should really be called only by object or test code
             */
            explicit Action(std::string identifier);

            /** Destructor
             */
            virtual ~Action();

            /** Get the identifier for this action.
             */
            std::string const & getId()
            {
                return m_id;
            }

            /** Sets the duration for this action
             */
            void setDuration(uint32_t duration)
            {
                m_duration = duration;
            }

            /** Gets the duration of this action
             */
            uint32_t getDuration() const
            {
                return m_duration;
            }

            /** Sets visualization to be used. Transfers ownership.
             */
            void adoptVisual(IVisual* visual)
            {
                m_visual = visual;
            }

            /** Gets used visualization
             */
            template <typename T>
            T* getVisual() const
            {
                // Use dynamic_cast to safely downcast across polymorphic types.
                // This avoids unsafe reinterpret_cast and preserves runtime checks.
                return dynamic_cast<T*>(m_visual);
            }

            /** Sets audio to be used. Transfers ownership.
             */
            void adoptAudio(ActionAudio* audio)
            {
                m_audio = audio;
            }

            /** Gets used audio
             */
            ActionAudio* getAudio() const
            {
                return m_audio;
            }

        private:
            std::string m_id;

            // duration of the action
            uint32_t m_duration;
            // visualization for action
            IVisual* m_visual;
            // audio for action
            ActionAudio* m_audio;
    };

} // namespace FIFE

#endif
