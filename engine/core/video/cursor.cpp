/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE Team                              *
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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/rect.h"

#include "imagepool.h"
#include "animationpool.h"
#include "animation.h"
#include "image.h"
#include "renderbackend.h"
#include "cursor.h"

namespace FIFE {
	
	Cursor::Cursor(ImagePool* imgpool, AnimationPool* animpool, RenderBackend* renderbackend):
		m_cursor_id(0),
		m_cursor_type(CURSOR_NATIVE),
		m_renderbackend(renderbackend),
		m_imgpool(imgpool),
		m_animpool(animpool),
		m_animtime(0) {
		
	}
	
	void Cursor::set(MouseCursorType ctype, unsigned int cursor_id) {
		m_cursor_id = cursor_id;
		m_cursor_type = ctype;
		if (ctype == CURSOR_NATIVE) {
			SDL_ShowCursor(1);
		} else {
			SDL_ShowCursor(0);
			if (ctype == CURSOR_ANIMATION) {
				m_animtime = SDL_GetTicks();
			}
		}
	}
	
	void Cursor::draw() {
		
		if (m_cursor_type == CURSOR_NATIVE) {
			return;
		}
		int mx, my;
		SDL_GetMouseState(&mx, &my);
		
		Image* cur = NULL;
		if (m_cursor_type == CURSOR_IMAGE) {
			cur = &m_imgpool->getImage(m_cursor_id);
		} else {
			Animation& anim = m_animpool->getAnimation(m_cursor_id);
			int animtime = (SDL_GetTicks() - m_animtime) % anim.getDuration();
			cur = anim.getFrameByTimestamp(animtime);
		}
		m_renderbackend->pushClipArea(Rect(mx, my, cur->getWidth(), cur->getHeight()));
 		cur->render(Rect(mx, my, cur->getWidth(), cur->getHeight()));
 		std::cout << mx << ", " << my << ", " << cur->getWidth() << ", " << cur->getHeight() << "\n";
 		m_renderbackend->popClipArea();
	}
}
