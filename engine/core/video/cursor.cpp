/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/rect.h"
#include "util/time/timemanager.h"
#include "util/log/logger.h"
#include "video/imagemanager.h"

#include "animation.h"
#include "image.h"
#include "renderbackend.h"
#include "cursor.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_GUI); //@todo We should have a log module for cursor

	Cursor::Cursor(RenderBackend* renderbackend):
		m_cursor_id(NC_ARROW),
		m_cursor_type(CURSOR_NATIVE),
		m_drag_type(CURSOR_NONE),
		m_native_cursor(NULL),
		m_renderbackend(renderbackend),
		m_animtime(0),
		m_drag_animtime(0),
		m_drag_offset_x(0),
		m_drag_offset_y(0),
		m_mx(0),
		m_my(0),
		m_timemanager(TimeManager::instance()),
		m_invalidated(false) {
		assert(m_timemanager);
		set(m_cursor_id);
	}

	void Cursor::set(uint32_t cursor_id) {
		m_cursor_type = CURSOR_NATIVE;

		if (!SDL_ShowCursor(1)) {
			SDL_PumpEvents();
		}
		setNativeCursor(cursor_id);

		m_cursor_image.reset();
		m_cursor_animation.reset();
	}

	void Cursor::set(ImagePtr image) {
		assert(image != 0);

		m_cursor_image = image;
		m_cursor_type = CURSOR_IMAGE;

		if (SDL_ShowCursor(0)) {
			SDL_PumpEvents();
		}

		m_cursor_id = NC_ARROW;
		m_cursor_animation.reset();
	}

	void Cursor::set(AnimationPtr anim) {
		assert(anim != 0);

		m_cursor_animation = anim;
		m_cursor_type = CURSOR_ANIMATION;

		if (SDL_ShowCursor(0)) {
			SDL_PumpEvents();
		}
		m_animtime = m_timemanager->getTime();

		m_cursor_id = NC_ARROW;
		m_cursor_image.reset();
	}

	void Cursor::setDrag(ImagePtr image, int32_t drag_offset_x, int32_t drag_offset_y) {
		assert(image != 0);

		m_cursor_drag_image = image;
		m_drag_type = CURSOR_IMAGE;
		m_drag_offset_x = drag_offset_x;
		m_drag_offset_y = drag_offset_y;

		m_cursor_drag_animation.reset();
	}

	void Cursor::setDrag(AnimationPtr anim, int32_t drag_offset_x, int32_t drag_offset_y) {
		assert(anim != 0);

		m_cursor_drag_animation = anim;
		m_drag_type = CURSOR_ANIMATION;
		m_drag_offset_x = drag_offset_x;
		m_drag_offset_y = drag_offset_y;

		m_drag_animtime = m_timemanager->getTime();

		m_cursor_drag_image.reset();
	}

	void Cursor::resetDrag() {
		m_drag_type = CURSOR_NONE;

		m_drag_animtime = 0;
		m_drag_offset_x = 0;
		m_drag_offset_y = 0;

		m_cursor_drag_animation.reset();
		m_cursor_drag_image.reset();
	}

    void Cursor::setPosition(uint32_t x, uint32_t y) {
		m_mx = x;
		m_my = y;
		SDL_WarpMouseInWindow(RenderBackend::instance()->getWindow(), m_mx, m_my);
	}

    void Cursor::getPosition(int32_t* x, int32_t* y) {
        *x = m_mx;
        *y = m_my;
    }

	void Cursor::invalidate() {
		if (m_native_cursor != NULL) {
			SDL_FreeCursor(m_native_cursor);
			m_native_cursor = NULL;

			m_invalidated = true;
		}
	}

	void Cursor::draw() {
		if (m_invalidated) {
			if (m_cursor_type != CURSOR_ANIMATION || m_cursor_type == CURSOR_IMAGE ) {
				set(m_cursor_id);
			}

			m_invalidated = false;
		}

		SDL_GetMouseState(&m_mx, &m_my);
		if ((m_cursor_type == CURSOR_NATIVE) && (m_drag_type == CURSOR_NONE)) {
			return;
		}

		// render possible drag image
		ImagePtr img;
		if (m_drag_type == CURSOR_IMAGE) {
			img = m_cursor_drag_image;
		}
		else if (m_drag_type == CURSOR_ANIMATION) {
			int32_t animtime = (m_timemanager->getTime() - m_drag_animtime) % m_cursor_drag_animation->getDuration();
			img = m_cursor_drag_animation->getFrameByTimestamp(animtime);
		}

		if (img != 0) {
			Rect area(m_mx + m_drag_offset_x + img->getXShift(), m_my + m_drag_offset_y + img->getYShift(), img->getWidth(), img->getHeight());
			m_renderbackend->pushClipArea(area, false);
			img->render(area);
			m_renderbackend->renderVertexArrays();
			m_renderbackend->popClipArea();
		}

		ImagePtr img2;
		// render possible cursor image
		if (m_cursor_type == CURSOR_IMAGE) {
			img2 = m_cursor_image;
		}
		else if (m_cursor_type == CURSOR_ANIMATION) {
			int32_t animtime = (m_timemanager->getTime() - m_animtime) % m_cursor_animation->getDuration();
			img2 = m_cursor_animation->getFrameByTimestamp(animtime);
		}

		if (img2 != 0) {
			Rect area(m_mx + img2->getXShift(), m_my + img2->getYShift(), img2->getWidth(), img2->getHeight());
			m_renderbackend->pushClipArea(area, false);
			img2->render(area);
			m_renderbackend->renderVertexArrays();
			m_renderbackend->popClipArea();
		}
	}

	uint32_t Cursor::getNativeId(uint32_t cursor_id) {
		switch (cursor_id) {
			case NC_ARROW:
				return SDL_SYSTEM_CURSOR_ARROW;
			case NC_IBEAM:
				return SDL_SYSTEM_CURSOR_IBEAM;
			case NC_WAIT:
				return SDL_SYSTEM_CURSOR_WAIT;
			case NC_CROSS:
				return SDL_SYSTEM_CURSOR_CROSSHAIR;
			case NC_WAITARROW:
				return SDL_SYSTEM_CURSOR_WAITARROW;
			case NC_RESIZENWSE:
				return SDL_SYSTEM_CURSOR_SIZENWSE;
			case NC_RESIZENESW:
				return SDL_SYSTEM_CURSOR_SIZENESW;
			case NC_RESIZEWE:
				return SDL_SYSTEM_CURSOR_SIZEWE;
			case NC_RESIZENS:
				return SDL_SYSTEM_CURSOR_SIZENS;
			case NC_RESIZEALL:
				return SDL_SYSTEM_CURSOR_SIZEALL;
			case NC_NO:
				return SDL_SYSTEM_CURSOR_NO;
			case NC_HAND:
				return SDL_SYSTEM_CURSOR_HAND;
		}
		return cursor_id;
	}

	void Cursor::setNativeCursor(uint32_t cursor_id) {
		cursor_id = getNativeId(cursor_id);
		SDL_Cursor* cursor = SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(cursor_id));
		if (!cursor) {
			FL_WARN(_log, "Cursor: No cursor matching cursor_id was found.");
			return;
		}
		m_native_cursor = cursor;
		SDL_SetCursor(cursor);
	}
}
