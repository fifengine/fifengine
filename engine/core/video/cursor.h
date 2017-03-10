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

#ifndef FIFE_CURSOR_H
#define FIFE_CURSOR_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "animation.h"

struct SDL_Cursor;

namespace FIFE {

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

	enum NativeCursor {
		// Start on 1000000 to avoid id-clashes with SDL
		NC_ARROW = 1000000,	// Standard arrow
		NC_IBEAM,			// I-beam for text selection
		NC_WAIT,			// Hourglass
		NC_CROSS,			// Crosshair
		NC_WAITARROW,		// Small wait cursor (or Wait if not available)
		NC_RESIZENWSE,		// Double arrow pointing northwest and southeast
		NC_RESIZENESW,		// Double arrow pointing northeast and southwest
		NC_RESIZEWE,		// Double arrow pointing west and east
		NC_RESIZENS,		// Double arrow pointing north and south
		NC_RESIZEALL,		// Four-pointed arrow pointing north, south, east, and west
		NC_NO,				// Slashed circle
		NC_HAND				// Hand. Common for links, etc.
	};

	/**  Cursor class manages mouse cursor handling
	 */
	class Cursor {
	public:
		/** Constructor.
		 */
		Cursor(RenderBackend* renderbackend);

		/** Destructor.
		 */
		virtual ~Cursor() { invalidate(); }

		void invalidate();

		/** draws cursor on screen
		 */
		virtual void draw();

		/** Sets the current mouse cursor
		 * @param cursor_id For native cursors, this is the resource id to native cursor, or one of the values in NativeCursor
		 */
		void set(uint32_t cursor_id=0);

		/** Sets the current mouse cursor type to image
		 * @param image ImagePtr to a image used for the cursor
		 */
		void set(ImagePtr image);

		/** Sets the current mouse cursor type to animation
		 * @param anim AnimationPtr to a loaded animation used for the cursor
		 */
		void set(AnimationPtr anim);

		/** Sets the current drag image cursor
		 * @param image ImagePtr to a image used for the drag
		 * @param drag_offset_x X Offset to display image when dragging.
		 * @param drag_offset_y Y Offset to display image when dragging.
		 * @note to reset the cursors drag call cursor.setDrag(Cursor::CURSOR_NONE, 0, 0)
		 */
		void setDrag(ImagePtr image, int32_t drag_offset_x=0, int32_t drag_offset_y=0);

		/** Sets the current drag animated cursor
		 * @param anim AnimationPtr to a loaded animation used for the drag
		 * @param drag_offset_x X Offset to display animation when dragging.
		 * @param drag_offset_y Y Offset to display animation when dragging.
		 * @note to reset the cursors drag call cursor.setDrag(Cursor::CURSOR_NONE, 0, 0)
		 */
		void setDrag(AnimationPtr anim, int32_t drag_offset_x=0, int32_t drag_offset_y=0);

		/** Resets the cursor drag type to CURSOR_NONE
		 */
		void resetDrag();

		/** Gets the current mouse cursor type
		 */
		MouseCursorType getType() const { return m_cursor_type; }

		/** Gets the current mouse cursor handle
		 */
		uint32_t getId() const { return m_cursor_id; }

		/** Gets the current mouse image
		 */
		ImagePtr getImage() { return m_cursor_image; }

		/** Gets the current mouse animation
		 */
		AnimationPtr getAnimation() { return m_cursor_animation; }

		/** Gets the current mouse cursor type
		 */
		MouseCursorType getDragType() const { return m_drag_type; }

		/** Gets the current mouse drag image
		 */
		ImagePtr getDragImage() { return m_cursor_drag_image; }

		/** Gets the current mouse drag animation
		 */
		AnimationPtr getDragAnimation() { return m_cursor_drag_animation; }

		/** Gets the current mouse x position
		 */
		uint32_t getX() const { return m_mx; }

		/** Gets the current mouse y position
		 */
		uint32_t getY() const { return m_my; }

		/** Set the mouse position
		 * @param x,y: The new position in screen coordinates
		 */
		void setPosition(uint32_t x, uint32_t y);

		/** Get the current mouse position
		*/
		void getPosition(int32_t* x, int32_t* y);

	protected:
		/** Sets the cursor to a native type.
		  * @param cursor_id One of the values in NativeCursor
		  */
		void setNativeCursor(uint32_t cursor_id);

		/** To get some consistancy between platforms, this function checks
		  * if cursor_id matches any of the values in NativeCursor, and
		  * returns the SDL_SystemCursor.
		  * If no match is found, cursor_id is returned.
		  *
		  * @param cursor_id One of the values in NativeCursor
		  */
		uint32_t getNativeId(uint32_t cursor_id);

	private:
		uint32_t m_cursor_id;
		MouseCursorType m_cursor_type;
		MouseCursorType m_drag_type;

		SDL_Cursor* m_native_cursor;

		ImagePtr m_cursor_image;
		ImagePtr m_cursor_drag_image;

		AnimationPtr m_cursor_animation;
		AnimationPtr m_cursor_drag_animation;

		RenderBackend* m_renderbackend;

		uint32_t m_animtime;
		uint32_t m_drag_animtime;

		int32_t m_drag_offset_x;
		int32_t m_drag_offset_y;
		int32_t m_mx;
		int32_t m_my;
		TimeManager* m_timemanager;

		bool m_invalidated;
	};

} //FIFE

#endif
