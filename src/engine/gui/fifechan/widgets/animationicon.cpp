// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "animationicon.h"

// Standard C++ library includes
#include <cassert>

// 3rd party library includes

// FIFE includes
#include "gui/fifechan/base/gui_image.h"
#include "util/base/exception.h"
#include "util/time/timemanager.h"

namespace fcn
{
    AnimationIcon::AnimationIcon() :
        mTimemanager(FIFE::TimeManager::instance()),

        mCurrentImage(nullptr),
        mAnimtime(0),
        mFrameIndex(-1),
        mRepeat(true),
        mPlay(true)
    {

        setScaling(false);
        setTiling(false);
        setOpaque(true);
        adjustSize();
    }

    AnimationIcon::AnimationIcon(FIFE::AnimationPtr const & animation) :
        mTimemanager(FIFE::TimeManager::instance()),
        mAnimation(animation),
        mCurrentImage(nullptr),
        mAnimtime(0),
        mFrameIndex(-1),
        mRepeat(true),
        mPlay(true)
    {

        // set first frame as new image
        if (mAnimation->getFrameCount() > 0) {
            mFrameIndex   = 0;
            mCurrentImage = new FIFE::GuiImage(mAnimation->getFrame(mFrameIndex));
            setImage(mCurrentImage);
        }
        setScaling(false);
        setTiling(false);
        setOpaque(true);
        adjustSize();
    }

    AnimationIcon::~AnimationIcon()
    {
        delete mCurrentImage;
    }

    void AnimationIcon::setAnimation(FIFE::AnimationPtr const & animation)
    {
        mAnimation = animation;
        if (mPlay) {
            mAnimtime = mTimemanager->getTime();
        } else {
            mAnimtime = 0;
        }
        // set first frame as new image
        if (mAnimation->getFrameCount() > 0) {
            mFrameIndex = 0;
            if (mCurrentImage != nullptr) {
                delete mCurrentImage;
                mCurrentImage = nullptr;
            }
            mCurrentImage = new FIFE::GuiImage(mAnimation->getFrame(mFrameIndex));
            setImage(mCurrentImage);
        }
        adjustSize();
    }

    FIFE::AnimationPtr AnimationIcon::getAnimation() const
    {
        return mAnimation;
    }

    void AnimationIcon::setRepeating(bool repeat)
    {
        mRepeat = repeat;
    }

    bool AnimationIcon::isRepeating() const
    {
        return mRepeat;
    }

    void AnimationIcon::play()
    {
        mPlay     = true;
        mAnimtime = mTimemanager->getTime();
    }

    bool AnimationIcon::isPlaying() const
    {
        return mPlay;
    }

    void AnimationIcon::pause()
    {
        mPlay = false;
    }

    void AnimationIcon::stop()
    {
        mPlay = false;
        // set first frame as new image
        if (mAnimation->getFrameCount() > 0) {
            mFrameIndex   = 0;
            mCurrentImage = new FIFE::GuiImage(mAnimation->getFrame(mFrameIndex));
            setImage(mCurrentImage);
        }
    }

    void AnimationIcon::logic()
    {
        if (isPlaying()) {
            int32_t index = mFrameIndex;
            if (isRepeating()) {
                index = mAnimation->getFrameIndex((mTimemanager->getTime() - mAnimtime) % mAnimation->getDuration());
            } else {
                index = mAnimation->getFrameIndex(mTimemanager->getTime() - mAnimtime);
            }
            if (index != mFrameIndex) {
                mFrameIndex = index;
                if (mCurrentImage != nullptr) {
                    delete mCurrentImage;
                    mCurrentImage = nullptr;
                }
                if (mFrameIndex >= 0) {
                    mCurrentImage = new FIFE::GuiImage(mAnimation->getFrame(mFrameIndex));
                }
                setImage(mCurrentImage);
            }
        }
    }
} // namespace fcn
