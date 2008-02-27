/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE Team                              *
 *   http://www.fifengine.de                                               *
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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/rect.h"
#include "util/time/timemanager.h"

#include "imagepool.h"
#include "animationpool.h"
#include "animation.h"
#include "image.h"
#include "renderbackend.h"
#include "cursor.h"

namespace FIFE {
	
	Cursor::Cursor(ImagePool* imgpool, AnimationPool* animpool, RenderBackend* renderbackend):
		m_cursor_id(0),
		m_drag_id(0),
		m_cursor_type(CURSOR_NATIVE),
		m_drag_type(CURSOR_NONE),
		m_renderbackend(renderbackend),
		m_imgpool(imgpool),
		m_animpool(animpool),
		m_animtime(0),
		m_drag_animtime(0),
		m_drag_offset_x(0),
		m_drag_offset_y(0),
		m_timemanager(TimeManager::instance()) {
		assert(m_timemanager);
	}
	
	void Cursor::set(MouseCursorType ctype, unsigned int cursor_id) {
		m_cursor_id = cursor_id;
		m_cursor_type = ctype;
		if (ctype == CURSOR_NATIVE) {
			SDL_ShowCursor(1);
		} else {
			SDL_ShowCursor(0);
			if (ctype == CURSOR_ANIMATION) {
				m_animtime = m_timemanager->getTime();
			}
		}
	}
	
	void Cursor::setDrag(MouseCursorType ctype, unsigned int drag_id, int drag_offset_x, int drag_offset_y) {
		m_drag_type = ctype;
		m_drag_id = drag_id;
		m_drag_offset_x = drag_offset_x;
		m_drag_offset_y = drag_offset_y;
		if (ctype != CURSOR_NONE) {
			if (ctype == CURSOR_ANIMATION) {
				m_drag_animtime = m_timemanager->getTime();
			}
		}
	}
	
	void Cursor::draw() {
		int mx = 0;
		int my = 0;
		if ((m_cursor_type == CURSOR_NATIVE) && (m_drag_type == CURSOR_NONE)) {
			return;
		}
		SDL_GetMouseState(&mx, &my);
		
		// render possible drag image
		Image* img = NULL;
		if (m_drag_type == CURSOR_IMAGE) {
			img = &m_imgpool->getImage(m_drag_id);
		} else if (m_drag_type == CURSOR_ANIMATION) {
			Animation& anim = m_animpool->getAnimation(m_drag_id);
			int animtime = (m_timemanager->getTime() - m_drag_animtime) % anim.getDuration();
 			img = anim.getFrameByTimestamp(animtime);
		}
		if (img) {
			Rect area(mx + m_drag_offset_x, my + m_drag_offset_y, img->getWidth(), img->getHeight());
			m_renderbackend->pushClipArea(area, false);
			img->render(area);
			m_renderbackend->popClipArea();
		}
		
		// render possible cursor image
		img = NULL;
		if (m_cursor_type == CURSOR_IMAGE) {
			img = &m_imgpool->getImage(m_cursor_id);
		} else if (m_cursor_type == CURSOR_ANIMATION) {
			Animation& anim = m_animpool->getAnimation(m_cursor_id);
			int animtime = (m_timemanager->getTime() - m_animtime) % anim.getDuration();
			img = anim.getFrameByTimestamp(animtime);
		}
		if (img) {
			Rect area(mx, my, img->getWidth(), img->getHeight());
			m_renderbackend->pushClipArea(area, false);
			img->render(area);
			m_renderbackend->popClipArea();
		}
	}
}
