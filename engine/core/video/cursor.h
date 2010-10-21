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

#ifndef FIFE_CURSOR_H
#define FIFE_CURSOR_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

struct SDL_Cursor;

namespace FIFE {

	class ImagePool;
	class AnimationPool;
	class RenderBackend;
	class TimeManager;

	/** Defines the type of shown cursor
	 * native -> default cursor
	 * image -> cursor from image pool
	 * animation -> cursor from animation pool
	 */
	enum MouseCursorType {
		CURSOR_NONE,
		CURSOR_NATIVE,
		CURSOR_IMAGE,
		CURSOR_ANIMATION
	};

	/** Defines some common native cursors between platforms.
	  * In addition to these, you can use the values in:
	  * Windows: http://msdn.microsoft.com/en-us/library/ms648391(VS.85).aspx
	  * X11: http://fife.pastebin.com/f5b89dd6b
	  */
	enum NativeCursor {
		// Start on 1000000 to avoid id-clashes with X11 and windows
		NC_ARROW = 1000000, // Standard arrow
		NC_IBEAM,			// I-beam for text selection
		NC_WAIT,			// Hourglass
		NC_CROSS,			// Crosshair
		NC_UPARROW,			// Vertical arrow
		NC_RESIZENW,		// Cursor for resize in northwest corner
		NC_RESIZESE,		//
		NC_RESIZESW,		//
		NC_RESIZENE,		//
		NC_RESIZEE,			//
		NC_RESIZEW,			//
		NC_RESIZEN,			//
		NC_RESIZES,			//
		NC_RESIZEALL,		// Four-pointed arrow pointing north, south, east, and west
		NC_NO,				// Slashed circle
		NC_HAND,			// Hand. Common for links, etc.
		NC_APPSTARTING,		// Standard arrow and small hourglass
		NC_HELP				// Arrow and question mark
	};

	/**  Cursor class manages mouse cursor handling
	 */
	class Cursor {
	public:
		/** Constructor.
		 */
		Cursor(ImagePool* imgpool, AnimationPool* animpool, RenderBackend* renderbackend);

		/** Destructor.
		 */
		virtual ~Cursor() { invalidate(); }

		void invalidate();

		/** draws cursor on screen
		 */
		virtual void draw();

		/** Sets the current mouse cursor type and possible pool value
		 * @param ctype cursor type
		 * @param cursor_id Pool id to image or animation. For native cursors, this is the resource id to native cursor, or one of the values in NativeCursor
		 */
		void set(MouseCursorType ctype, unsigned int cursor_id=0);

		/** Sets the current drag cursor type and pool value
		 * @param ctype drag cursor type
		 * @param drag_id pool id for the drag cursor (either image or animation)
		 * @param drag_offset offset of drag image shown with cursor
		 */
		void setDrag(MouseCursorType ctype, unsigned int drag_id=0, int drag_offset_x=0, int drag_offset_y=0);

		/** Gets the current mouse cursor type
		 */
		MouseCursorType getType() const { return m_cursor_type; }

		/** Gets the current mouse cursor pool id
		 */
		unsigned int getId() const { return m_cursor_id; }

		/** Gets the current mouse cursor type
		 */
		MouseCursorType getDragType() const { return m_drag_type; }

		/** Gets the current mouse cursor pool id
		 */
		unsigned int getDragId() const { return m_drag_id; }

		/** Gets the current mouse x position
		 */
		unsigned int getX() const {return m_mx;}

		/** Gets the current mouse y position
		 */
		unsigned int getY() const {return m_my;}

	protected:
		/** Sets the cursor to a native type.
		  * @param cursor_id Resource id to native cursor, or one of the values in NativeCursor
		  */
		void setNativeCursor(unsigned int cursor_id);

		/** To get some consistancy between platforms, this function checks
		  * if cursor_id matches any of the values in NativeCursor, and
		  * returns the resource ID specific to the running platform.
		  * If no match is found, cursor_id is returned.
		  *
		  * @param One of the values in NativeCursor
		  */
		unsigned int getNativeId(unsigned int cursor_id);

	private:
		unsigned int m_cursor_id;
		unsigned int m_drag_id;
		MouseCursorType m_cursor_type;
		MouseCursorType m_drag_type;

		SDL_Cursor* m_native_cursor;

		RenderBackend* m_renderbackend;
		ImagePool* m_imgpool;
		AnimationPool* m_animpool;

		unsigned int m_animtime;
		unsigned int m_drag_animtime;

		int m_drag_offset_x;
		int m_drag_offset_y;
		int m_mx;
		int m_my;
		TimeManager* m_timemanager;

		bool m_invalidated;
	};

} //FIFE

#endif
