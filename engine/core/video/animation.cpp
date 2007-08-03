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

	Animation::Animation(): 
		m_action_frame(-1),
		m_animation_endtime(-1) {
		}
	
	Animation::~Animation() {
		std::cout << ">>> in Animation destructor\n";
		std::vector<FrameInfo>::const_iterator i(m_frames.begin());
		while (i != m_frames.end()) {
			i->img->decRef();
			i++;
		}
	}
	
	void Animation::addFrame(Image* image, unsigned int duration) {
		image->addRef();

		FrameInfo info;
		info.index = m_frames.size();
		info.duration = duration;
		info.img = image;
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
		if ((static_cast<int>(timestamp) < m_animation_endtime) && (m_animation_endtime > 0)) {
			std::map<unsigned int, FrameInfo>::const_iterator i(m_framemap.lower_bound(timestamp));
			val = i->second.index;
		}
		return val;
	}

	bool Animation::isValidIndex(int index) {
		int size = m_frames.size();
		if ((size == 0) || (index > (size - 1))) {
			return false;
		}
		return true;
	}

	Image* Animation::getFrame(int index) {
		if (isValidIndex(index)) {
			return m_frames[index].img;
		} else {
			return NULL;
		}
	}

	Image* Animation::getFrameByTimestamp(unsigned int timestamp) {
		return getFrame(getFrameIndex(timestamp));
	}

	int Animation::getFrameDuration(int index) {
		if (isValidIndex(index)) {
			return m_frames[index].duration;
		} else {
			return -1;
		}
	}

	unsigned int Animation::getNumFrames() const {
		return m_frames.size();
	}

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
