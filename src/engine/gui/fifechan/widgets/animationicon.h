// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GUI_WIDGETS_ANIMATIONICON_H
#define FIFE_GUI_WIDGETS_ANIMATIONICON_H

// Standard C++ library includes

// 3rd party library includes
#include <fifechan.hpp>
#include <fifechan/image.hpp>
#include <fifechan/platform.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fife_stdint.h"
#include "video/animation.h"

namespace FIFE
{
    class TimeManager;
}

namespace fcn
{

    /**
     * Implements an icon capable of displaying an animation.
     */
    class AnimationIcon : public Icon
    {
    public:
        /**
         * Default constructor.
         */
        AnimationIcon();

        /**
         * Constructor.
         *
         * @param animation The animation to display.
         */
        explicit AnimationIcon(const FIFE::AnimationPtr& animation);

        /**
         * Destructor.
         */
        virtual ~AnimationIcon();

        AnimationIcon(const AnimationIcon&)            = delete;
        AnimationIcon& operator=(const AnimationIcon&) = delete;

        /**
         * Sets the animation to display.
         *
         * @param animation The animation to display.
         */
        void setAnimation(const FIFE::AnimationPtr& animation);

        /**
         * Gets the current animation.
         *
         * @return The current animation.
         */
        FIFE::AnimationPtr getAnimation() const;

        /**
         * Sets repeating of the animation.
         *
         * @param repeat True if the animation repeating is enabled, otherwise false.
         */
        void setRepeating(bool repeat);

        /**
         * Gets repeating of the animation.
         *
         * @return True if the animation repeating is enabled, otherwise false.
         */
        bool isRepeating() const;

        /**
         * Starts the animation from beginning.
         */
        void play();

        /**
         * Gets if the animation is playing.
         *
         * @return True if the animation is playing, otherwise false.
         */
        bool isPlaying() const;

        /**
         * Stops the animation at the current frame.
         */
        void pause();

        /**
         * Stops the animation and sets the current frame to the first frame.
         */
        void stop();

        // Inherited from Widget

        virtual void logic();

    protected:
        /**
         * Holds pointer to Fifes TimeManager.
         */
        FIFE::TimeManager* mTimemanager;

        /**
         * The animation to display.
         */
        FIFE::AnimationPtr mAnimation;

        /**
         * Currently used image. It's the current frame from the animation
         * encapsulated in a GuiImage.
         */
        const Image* mCurrentImage;

        /**
         * The time as the animation was started.
         */
        uint32_t mAnimtime;

        /**
         * The last used frame index from the animation.
         */
        int32_t mFrameIndex;

        /**
         * True if the animation should be repeating, otherwise false.
         */
        bool mRepeat;

        /**
         * True if the animation was started, otherwise false.
         */
        bool mPlay;
    };
} // namespace fcn

#endif
