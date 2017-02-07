/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE::                                            *
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
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "gui/fifechan/base/gui_image.h"
#include "util/base/exception.h"
#include "util/time/timemanager.h"

#include "animationicon.h"

namespace fcn {
	AnimationIcon::AnimationIcon() :
		mTimemanager(FIFE::TimeManager::instance()),
		mAnimation(FIFE::AnimationPtr()),
		mCurrentImage(NULL),
		mAnimtime(0),
		mFrameIndex(-1),
		mRepeat(true),
		mPlay(true) {

		setScaling(false);
		setTiling(false);
		setOpaque(true);
		adjustSize();
	}

	AnimationIcon::AnimationIcon(FIFE::AnimationPtr animation) :
		mTimemanager(FIFE::TimeManager::instance()),
		mAnimation(animation),
		mCurrentImage(NULL),
		mAnimtime(0),
		mFrameIndex(-1),
		mRepeat(true),
		mPlay(true) {

		// set first frame as new image
		if (mAnimation->getFrameCount() > 0) {
			mFrameIndex = 0;
			mCurrentImage = new FIFE::GuiImage(mAnimation->getFrame(mFrameIndex));
			setImage(mCurrentImage);
		}
		setScaling(false);
		setTiling(false);
		setOpaque(true);
		adjustSize();
	}

	AnimationIcon::~AnimationIcon() {
		delete mCurrentImage;
	}

	void AnimationIcon::setAnimation(FIFE::AnimationPtr animation) {
		mAnimation = animation;
		if (mPlay) {
			mAnimtime = mTimemanager->getTime();
		} else {
			mAnimtime = 0;
		}
		// set first frame as new image
		if (mAnimation->getFrameCount() > 0) {
			mFrameIndex = 0;
			if (mCurrentImage) {
				delete mCurrentImage;
				mCurrentImage = 0;
			}
			mCurrentImage = new FIFE::GuiImage(mAnimation->getFrame(mFrameIndex));
			setImage(mCurrentImage);
		}
		adjustSize();
	}

	FIFE::AnimationPtr AnimationIcon::getAnimation() const {
		return mAnimation;
	}

	void AnimationIcon::setRepeating(bool repeat) {
		mRepeat = repeat;
	}

	bool AnimationIcon::isRepeating() const {
		return mRepeat;
	}

	void AnimationIcon::play() {
		mPlay = true;
		mAnimtime = mTimemanager->getTime();
	}

	bool AnimationIcon::isPlaying() const {
		return mPlay;
	}

	void AnimationIcon::pause() {
		mPlay = false;
	}

	void AnimationIcon::stop() {
		mPlay = false;
		// set first frame as new image
		if (mAnimation->getFrameCount() > 0) {
			mFrameIndex = 0;
			mCurrentImage = new FIFE::GuiImage(mAnimation->getFrame(mFrameIndex));
			setImage(mCurrentImage);
		}
	}

	void AnimationIcon::logic() {
		if (isPlaying()) {
			int32_t index = mFrameIndex;
			if (isRepeating()) {
				index = mAnimation->getFrameIndex((mTimemanager->getTime() - mAnimtime) % mAnimation->getDuration());
			} else {
				index = mAnimation->getFrameIndex(mTimemanager->getTime() - mAnimtime);
			}
			if (index != mFrameIndex) {
				mFrameIndex = index;
				if (mCurrentImage) {
					delete mCurrentImage;
					mCurrentImage = 0;
				}
				if (mFrameIndex >= 0) {
					mCurrentImage = new FIFE::GuiImage(mAnimation->getFrame(mFrameIndex));
				}
				setImage(mCurrentImage);
			}
		}
	}
}
