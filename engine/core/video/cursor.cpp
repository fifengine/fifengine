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
#if defined( WIN32 )
#include <windows.h>
#include <sdl.h>
#endif

#if defined( __unix__ )
#include <X11/Xcursor/Xcursor.h>
#endif

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/rect.h"
#include "util/time/timemanager.h"
#include "util/log/logger.h"

#include "imagepool.h"
#include "animationpool.h"
#include "animation.h"
#include "image.h"
#include "renderbackend.h"
#include "cursor.h"

#if defined( WIN32 )

// From SDL_sysmouse.c
struct WMcursor {
	HCURSOR curs;
#ifndef _WIN32_WCE
	Uint8 *ands;
	Uint8 *xors;
#endif
};

#endif

#if defined( __unix__ )

// Stops the compiler from confusing it with FIFE:Cursor
typedef Cursor XCursor;

// From SDL_x11mouse.c
struct WMcursor {
	Cursor x_cursor;
};

#endif

namespace FIFE {
	static Logger _log(LM_GUI); // We should have a log module for cursor

	Cursor::Cursor(ImagePool* imgpool, AnimationPool* animpool, RenderBackend* renderbackend):
		m_cursor_id(NC_ARROW),
		m_drag_id(0),
		m_cursor_type(CURSOR_NATIVE),
		m_drag_type(CURSOR_NONE),
		m_native_cursor(NULL),
		m_renderbackend(renderbackend),
		m_imgpool(imgpool),
		m_animpool(animpool),
		m_animtime(0),
		m_drag_animtime(0),
		m_drag_offset_x(0),
		m_drag_offset_y(0),
		m_mx(0),
		m_my(0),
		m_timemanager(TimeManager::instance()),
		m_invalidated(false) {
		assert(m_timemanager);
		set(m_cursor_type, m_cursor_id);
	}

	void Cursor::set(MouseCursorType ctype, unsigned int cursor_id) {
		m_cursor_id = cursor_id;
		m_cursor_type = ctype;
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (ctype == CURSOR_NATIVE) {
			if (!SDL_ShowCursor(1)) {
				SDL_PumpEvents();
				SDL_WarpMouse(mx, my);
			}
			setNativeCursor(cursor_id);
		} else {
			if (SDL_ShowCursor(0)) {
				SDL_PumpEvents();
				SDL_WarpMouse(mx, my);
			}
			if (ctype == CURSOR_ANIMATION) {
				m_animtime = m_timemanager->getTime();
			}
		}
		m_invalidated = false;
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

	void Cursor::invalidate() {
		if (m_native_cursor != NULL) {
			SDL_free(m_native_cursor->wm_cursor);
			m_native_cursor->wm_cursor = NULL;
			SDL_FreeCursor(m_native_cursor);
			m_native_cursor = NULL;

			m_invalidated = true;
		}
	}

	void Cursor::draw() {
		if (m_invalidated) {
			set(m_cursor_type, m_cursor_id);
		}

		SDL_GetMouseState(&m_mx, &m_my);
		if ((m_cursor_type == CURSOR_NATIVE) && (m_drag_type == CURSOR_NONE)) {
			return;
		}

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
			Rect area(m_mx + m_drag_offset_x + img->getXShift(), m_my + m_drag_offset_y + img->getYShift(), img->getWidth(), img->getHeight());
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
			Rect area(m_mx + img->getXShift(), m_my + img->getYShift(), img->getWidth(), img->getHeight());
			m_renderbackend->pushClipArea(area, false);
			img->render(area);
			m_renderbackend->popClipArea();
		}
	}

	unsigned int Cursor::getNativeId(unsigned int cursor_id) {
#if defined( WIN32 )
		switch (cursor_id) {
			case NC_ARROW:
				return 32512; // IDC_ARROW;
			case NC_IBEAM:
				return 32513; // IDC_IBEAM;
			case NC_WAIT:
				return 32514; // IDC_WAIT;
			case NC_CROSS:
				return 32515; // IDC_CROSS;
			case NC_UPARROW:
				return 32516; // IDC_UPARROW;
			case NC_RESIZESE:
				return 32642; // IDC_SIZENWSE;
			case NC_RESIZESW:
				return 32643; // IDC_SIZENESW;
			case NC_RESIZEE:
				return 32644; // IDC_SIZEWE;
			case NC_RESIZES:
				return 32645; // IDC_SIZENS;
			case NC_RESIZENW:
				return 32642; // IDC_SIZENWSE;
			case NC_RESIZENE:
				return 32643; // IDC_SIZENESW;
			case NC_RESIZEW:
				return 32644; // IDC_SIZEWE;
			case NC_RESIZEN:
				return 32645; // IDC_SIZENS;
			case NC_RESIZEALL:
				return 32646; // IDC_SIZEALL;
			case NC_NO:
				return 32648; // IDC_NO;
			case NC_HAND:
				return 32649; // IDC_HAND;
			case NC_APPSTARTING:
				return 32650; // IDC_APPSTARTING;
			case NC_HELP:
				return 32651; // IDC_HELP;
			default:
				break;
		}

#elif defined( __unix__ )
		switch (cursor_id) {
			case NC_ARROW:
				return 68;
			case NC_IBEAM:
				return 152;
			case NC_WAIT:
				return 150;
			case NC_CROSS:
				return 130;
			case NC_UPARROW:
				return 22;
			case NC_RESIZESE:
				return 14;
			case NC_RESIZESW:
				return 12;
			case NC_RESIZEE:
				return 96;
			case NC_RESIZES:
				return 16;
			case NC_RESIZENW:
				return 134;
			case NC_RESIZENE:
				return 136;
			case NC_RESIZEW:
				return 70;
			case NC_RESIZEN:
				return 138;
			case NC_RESIZEALL:
				return 52;
			case NC_NO:
				return 0;
			case NC_HAND:
				return 60;
			case NC_APPSTARTING:
				return 150;
			case NC_HELP:
				return 92;
			default:
				break;
		}
#endif
		return cursor_id;
	}

	void Cursor::setNativeCursor(unsigned int cursor_id) {
#if defined( WIN32 ) || defined(__unix__)
		// Check if a value in NativeCursors is requested
		cursor_id = getNativeId(cursor_id);

		// Load cursor
#if defined( __unix__ )
		static Display* dsp = XOpenDisplay(NULL);
		XCursor xCursor = XcursorShapeLoadCursor(dsp, cursor_id);
		if (xCursor == 0) {
			if (m_native_cursor != NULL) {
				SDL_FreeCursor(m_native_cursor);
				m_native_cursor = NULL;
			}
			FL_WARN(_log, "Cursor: No cursor matching cursor_id was found.");
			return;
		}
#elif defined( WIN32 )
		// Load native cursor
		HCURSOR hIcon = LoadCursor(NULL, MAKEINTRESOURCE(cursor_id));
		if (hIcon == static_cast<HCURSOR>(0)) {
			if (m_native_cursor != NULL) {
				SDL_FreeCursor(m_native_cursor);
				m_native_cursor = NULL;
			}
			FL_WARN(_log, "Cursor: No cursor matching cursor_id was found.");
			return;
		}
#endif

		WMcursor *cursor;
		SDL_Cursor *curs2;

		// Allocate memory. Use SDL_FreeCursor to free cursor memory
		cursor = (WMcursor *)SDL_malloc(sizeof(*cursor));
		curs2 = (SDL_Cursor *)SDL_malloc(sizeof *curs2);

		//-- Set up some default values --
		curs2->wm_cursor = cursor;
		curs2->data = NULL;
		curs2->mask = NULL;
		curs2->save[0] = NULL;
		curs2->save[1] = NULL;
		curs2->area.x = 0;
		curs2->area.y = 0;
		curs2->area.w = 32;
		curs2->area.h = 32;
		curs2->hot_x = 0;
		curs2->hot_y = 0;

#if defined(WIN32)
		cursor->curs = hIcon;
#ifndef _WIN32_WCE
		cursor->ands = NULL;
		cursor->xors = NULL;
#endif

		// Get hot spot
		ICONINFO iconinfo;
		if (GetIconInfo(hIcon, &iconinfo)) {
			curs2->hot_x = static_cast<Sint16>(iconinfo.xHotspot);
			curs2->hot_y = static_cast<Sint16>(iconinfo.yHotspot);
		}

#elif defined(__unix__)
		cursor->x_cursor = xCursor;
		XSync(dsp, false);
#endif

		m_native_cursor = curs2;
		SDL_SetCursor(curs2);

#endif // WIN32 || __unix__
	}
}
