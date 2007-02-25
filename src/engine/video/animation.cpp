/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <boost/lexical_cast.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "debugutils.h"
#include "exception.h"
#include "imagecache.h"
#include "timemanager.h"

#include "animation.h"
#include "image.h"
#include "rect.h"

namespace FIFE {

	Animation* Animation::m_first_animation = 0;
	Animation::Updater* Animation::m_updater = 0;

	Animation::Updater::Updater() : TimeEvent(0) {
		TimeManager::instance()->registerEvent(this);
	}

	Animation::Updater::~Updater() {
		TimeManager::instance()->unregisterEvent(this);
	}

	void Animation::Updater::updateEvent(unsigned long time) {
		Animation::m_first_animation->updateList(time);
	}

	Animation::Animation(size_t numFrames) : RenderAble(RT_ANIMATION), frames(numFrames) {
		m_currentFrame = 0;
		m_frameChangedAt = 0;
		m_frameDuration = 0;
		m_canSwitchDirection = false;
		m_animateForward = true;
		m_isActive = true;

		// If this is the first animation created, create Updater.
		if (!m_first_animation) {
			m_updater = new Updater();
		}

		// Insert self at the beginning of linked list of animations.
		m_next_animation = m_first_animation;
		if (m_next_animation) {
			m_next_animation->m_previous_animation = this;
		}
		m_previous_animation = 0;
		m_first_animation = this;
	}
	
	Animation::~Animation() {
		// I moved the ownership of animation images to the
		// Animation Class. This is an ugly solution, it works
		// right, if the loader does not give the animation
		// Images, that itself are put in the ImageCache
		// with a location. The collect() might then delete
		// The Image without the Animation's knowledge!
		// If someone has a better solution ... :-) -phoku
		for(unsigned int i=0; i!=frames.size(); ++i) {
			delete frames[i];
		}

		// Remove self from the linked list of animations.
		if (m_previous_animation) {
			m_previous_animation->m_next_animation = m_next_animation;
		}
		else {
			m_first_animation = m_next_animation;
		}
		if (m_next_animation) {
			m_next_animation->m_previous_animation = m_previous_animation;
		}
		if (!m_first_animation) { 
			//If no animations left then remove updater.
			if (m_updater) { // test to safe against multiple deletion
				delete m_updater;
				m_updater = 0;
			}
		}
	}

	void Animation::setOnEndCallback(boost::function0<void> f) {
		m_onAnimationEnd = f;
	}
	
	void Animation::setFrame(size_t num, Image* image) {
		if (num >= frames.size()) {
			throw IndexOverflow(boost::lexical_cast<std::string>(num) + std::string(" >= animation size ") +
					boost::lexical_cast<std::string>(frames.size()));
		}
		frames[num] = image;
	}
	void Animation::setFrameDuration(uint32_t length) {
		m_frameDuration = length;
	}
	void Animation::setCurrentFrame(size_t num) {
		if (num >= frames.size()) {
			throw IndexOverflow(boost::lexical_cast<std::string>(num) + std::string(" >= animation size ") +
					boost::lexical_cast<std::string>(frames.size()));
		}
		m_currentFrame = num;
	}
	void Animation::setActionFrame(size_t num) {
		m_action_frame = num;
	};

	void Animation::update(uint32_t nowTicks) {
		if (!m_isActive)
			return;

		if (nowTicks - m_frameChangedAt >= m_frameDuration) {
			flipFrame();
			m_frameChangedAt = nowTicks;
		}
	}
	void Animation::updateList(unsigned long time) {
		update(time);
		if (m_next_animation) { //update next animation in the list
			m_next_animation->updateList(time);
		}
	}

	void Animation::render(const Rect& target, Screen *screen, unsigned char alpha) {
		RenderAble* img = frames[m_currentFrame];
		Rect tmp(target);
		tmp.x += img->getXShift();
		tmp.y += img->getYShift();
		img->render(tmp, screen, alpha);
	}
	/*
		 void Animation::render(SDL_Rect *target, SDL_Surface *screen, Uint8 alpha) {
		 Image* img = ImageCache::instance()->getImage(m_currentFrame);
		 img->render(target, screen, alpha);
		 }*/
	Image* Animation::getCurrentFrame() {
		return frames[m_currentFrame];
	}

	RenderAble* Animation::getFrame(int frame) {
		return frames[frame];
	}
	int Animation::getXShift() const {
		return m_xshift;
	}
	int Animation::getYShift() const {
		return m_yshift;
	}
	void Animation::setXShift(int xshift) {
		m_xshift = xshift;
	}
	void Animation::setYShift(int yshift) {
		m_yshift = yshift;
	}

	unsigned int Animation::getWidth() const {
		RenderAble* img = frames[m_currentFrame];
		return img->getWidth();
	}
	
	unsigned int Animation::getHeight() const {
		RenderAble* img = frames[m_currentFrame];
		return img->getHeight();
	}
	
	void Animation::setIsActive(bool active) {
		m_isActive = active;
	}
	
	void Animation::setDirection(bool forward) {
		m_canSwitchDirection = true;
		switch(forward) {
			case true:
				m_animateForward = true;
				m_currentFrame = 0;
				break;
			case false:
				m_animateForward = false;
				m_currentFrame = frames.size() - 1;
				break;
		}
	}

	void Animation::flipFrame() {
		if (!m_canSwitchDirection) {
			if (m_currentFrame < frames.size() - 1)
				++m_currentFrame;
			else {
				if (m_onAnimationEnd)
					m_onAnimationEnd();
				m_currentFrame = 0;
			}
		}
		else
			switch(m_animateForward) {
				case true:
					if (m_currentFrame < frames.size() - 1)
						++m_currentFrame;
					else {
						//m_currentFrame=0;
						if (m_onAnimationEnd)
							m_onAnimationEnd();
						m_animateForward = false;
					}
					break;
				case false:
					if (m_currentFrame > 0)
						--m_currentFrame;
					else {
						if (m_onAnimationEnd)
							m_onAnimationEnd();
						m_animateForward = true;
					}
					break;
			}

	}

	uint32_t Animation::getFrameDuration() const {
		return m_frameDuration;
	}

	size_t Animation::getNumFrames() const {
		return frames.size();
	}

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
