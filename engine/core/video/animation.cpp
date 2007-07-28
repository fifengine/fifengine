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
#include "util/debugutils.h"
#include "util/exception.h"
#include "util/time/timemanager.h"

#include "animation.h"
#include "image.h"
#include "util/rect.h"

namespace FIFE {

	Animation::AnimationUpdater::AnimationUpdater(int period, Animation* anim): 
		TimeEvent(period),
		m_animation(anim) {
		TimeManager::instance()->registerEvent(this);
	}

	Animation::AnimationUpdater::~AnimationUpdater() {
		TimeManager::instance()->unregisterEvent(this);
	}

	void Animation::AnimationUpdater::updateEvent(unsigned long time) {
		m_animation->setNextFrame();
	}

	Animation::Animation(): 
		Renderable(),
		m_updater(NULL),
		m_listener(NULL),
		m_frames(),
		m_xshift(0),
		m_yshift(0),
		m_action_frame(0),
		m_currentFrame(0),
		m_frameDuration(0),
		m_animateForward(true),
		m_isActive(true) {
		}
	
	Animation::~Animation() {
		setActive(false);
		std::vector<Image*>::iterator i = m_frames.begin();
		while (i != m_frames.end()) {
			delete (*i);
			i++;
		}
	}
	
	void Animation::addFrame(Image* image) {
		m_frames.push_back(image);
	}

	void Animation::setFrameDuration(int length) {
		m_frameDuration = length;
	}

	void Animation::checkIndexOverflow(unsigned int index) {
		if (index >= m_frames.size()) {
			throw IndexOverflow(boost::lexical_cast<std::string>(index) + std::string(" >= animation size ") +
					boost::lexical_cast<std::string>(m_frames.size()));
		}
	}

	void Animation::setCurrentFrame(unsigned int num) {
		checkIndexOverflow(num);
		m_currentFrame = num;
	}

	void Animation::setActionFrame(unsigned int num) {
		checkIndexOverflow(num);
		m_action_frame = num;
	};

	void Animation::render(const Rect& target, Screen *screen, unsigned char alpha) {
		Renderable* img = m_frames[m_currentFrame];
		Rect tmp(target);
		tmp.x += img->getXShift();
		tmp.y += img->getYShift();
		img->render(tmp, screen, alpha);
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
		return m_frames[m_currentFrame]->getWidth();
	}
	
	unsigned int Animation::getHeight() const {
		return m_frames[m_currentFrame]->getHeight();
	}
	
	void Animation::setActive(bool active) {
		m_isActive = active;
		setTimerActive(active);
	}
	
	void Animation::setDirection(bool forward) {
		m_animateForward = forward;
		if (forward) {
			m_currentFrame = 0;
		} else {
			m_currentFrame = m_frames.size() - 1;
		}
	}

	void Animation::setNextFrame() {
		if (!m_isActive) {
 			return;
		}

		bool on_last_frame = true;
		if (m_animateForward) {
			if (m_currentFrame < m_frames.size() - 1) {
				++m_currentFrame;
				on_last_frame = false;
			}
		} else {
			if (m_currentFrame > 0) {
				--m_currentFrame;
				on_last_frame = false;
			}
		}
		if (on_last_frame && m_listener) {
			setActive(false);
			m_listener->onAnimationEnd(this);
		}
	}

	void Animation::setTimerActive(bool active) {
		if (active) {
			if (!m_updater) {
				m_updater = new AnimationUpdater(m_frameDuration, this);
			}
		} else {
			if (m_updater) {
				delete m_updater;
				m_updater = NULL;
			}
		}
	}

	int Animation::getFrameDuration() const {
		return m_frameDuration;
	}

	unsigned int Animation::getNumFrames() const {
		return m_frames.size();
	}

	void Animation::setAnimationListener(AnimationListener* listener) {
		m_listener = listener;
	}

	bool Animation::getActive() {
		return m_isActive;
	}

	bool Animation::isDirectionForward() {
		return m_animateForward;
	}

	SDL_Surface* Animation::getSurface() {
		return m_frames[m_currentFrame]->getSurface();
	}
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
