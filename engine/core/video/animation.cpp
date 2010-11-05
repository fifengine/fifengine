/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
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
#include <string>

// 3rd party library includes
#include <boost/lexical_cast.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/time/timemanager.h"

#include "animation.h"
#include "image.h"
#include "util/structures/rect.h"

namespace FIFE {

	Animation::Animation(): 
		m_action_frame(-1),
		m_animation_endtime(-1),
		m_direction(0) {
		}
	
	Animation::~Animation() {
		// note: we don't need to free the images, as they are handled via
		// smart references.
	}

	void Animation::addFrame(ResourcePtr image, unsigned int duration) {
		FrameInfo info;
		info.index = m_frames.size();
		info.duration = duration;
		info.image = image;
		m_frames.push_back(info);

		std::map<unsigned int, FrameInfo>::const_iterator i(m_framemap.end());
		if (i == m_framemap.begin()) {
			m_framemap[0] = info;
			m_animation_endtime = duration;
		} else {
			--i;
			unsigned int frametime = i->first + i->second.duration;
			m_framemap[frametime] = info;
			m_animation_endtime = frametime + duration;
		}
		
	}

	int Animation::getFrameIndex(unsigned int timestamp) {
		int val = -1;
		if ((static_cast<int>(timestamp) <= m_animation_endtime) && (m_animation_endtime > 0)) {
			std::map<unsigned int, FrameInfo>::const_iterator i(m_framemap.upper_bound(timestamp));
			--i;
			val = i->second.index;
		}
		return val;
	}

	bool Animation::isValidIndex(int index) const{
		int size = m_frames.size();
		return size > 0 && index >= 0 && index < size; 
	}

	Image* Animation::getFrame(int index) {
		if (isValidIndex(index)) {
			return m_frames[index].image.get<Image>();
		} else {
			return NULL;
		}
	}

	Image* Animation::getFrameByTimestamp(unsigned int timestamp) {
		return getFrame(getFrameIndex(timestamp));
	}

	int Animation::getFrameDuration(int index) const{
		if (isValidIndex(index)) {
			return m_frames[index].duration;
		} else {
			return -1;
		}
	}

	unsigned int Animation::getNumFrames() const {
		return m_frames.size();
	}

	void Animation::setDirection(unsigned int direction) {
		m_direction %= 360;
	}
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
