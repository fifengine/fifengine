/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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

	void Animation::addFrame(ImagePtr image, uint32_t duration) {
		FrameInfo info;
		info.index = m_frames.size();
		info.duration = duration;
		info.image = image;
		m_frames.push_back(info);

		std::map<uint32_t, FrameInfo>::const_iterator i(m_framemap.end());
		if (i == m_framemap.begin()) {
			m_framemap[0] = info;
			m_animation_endtime = duration;
		} else {
			--i;
			uint32_t frametime = i->first + i->second.duration;
			m_framemap[frametime] = info;
			m_animation_endtime = frametime + duration;
		}

	}

	int32_t Animation::getFrameIndex(uint32_t timestamp) {
		int32_t val = -1;
		if ((static_cast<int32_t>(timestamp) <= m_animation_endtime) && (m_animation_endtime > 0)) {
			std::map<uint32_t, FrameInfo>::const_iterator i(m_framemap.upper_bound(timestamp));
			--i;
			val = i->second.index;
		}
		return val;
	}

	bool Animation::isValidIndex(int32_t index) const{
		int32_t size = m_frames.size();
		return size > 0 && index >= 0 && index < size;
	}

	ImagePtr Animation::getFrame(int32_t index) {
		if (isValidIndex(index)) {
			ImagePtr image =  m_frames[index].image;
			if(image->getState() == IResource::RES_NOT_LOADED) {
				image->load();
			}
			return image;
		} else {
			return ImagePtr();  //return an invalid image .
		}
	}

	ImagePtr Animation::getFrameByTimestamp(uint32_t timestamp) {
		ImagePtr val;
		if ((static_cast<int32_t>(timestamp) <= m_animation_endtime) && (m_animation_endtime > 0)) {
			std::map<uint32_t, FrameInfo>::const_iterator i(m_framemap.upper_bound(timestamp));
			--i;
			val = i->second.image;
		}
		if(val && val->getState() == IResource::RES_NOT_LOADED) {
			val->load();
		}
		return val;
	}

	int32_t Animation::getFrameDuration(int32_t index) const{
		if (isValidIndex(index)) {
			return m_frames[index].duration;
		} else {
			return -1;
		}
	}

	uint32_t Animation::getFrameCount() const {
		return m_frames.size();
	}

	void Animation::setDirection(uint32_t direction) {
		m_direction %= 360;
	}
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
