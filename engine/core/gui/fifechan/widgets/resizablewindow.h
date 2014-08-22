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

#ifndef FIFE_GUI_WIDGETS_RESIZABLEWINDOW_H
#define FIFE_GUI_WIDGETS_RESIZABLEWINDOW_H

// Standard C++ library includes

// 3rd party library includes
#include <fifechan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/cursor.h"


namespace fcn {
	class ResizableWindow : public Window, public KeyListener, public FocusListener {
	public:
		enum CursorDirections {
			// left
			CURSOR_DIRECTION_L = 0,
			// right
			CURSOR_DIRECTION_R = 1,
			// top
			CURSOR_DIRECTION_T = 2,
			// bottom
			CURSOR_DIRECTION_B = 5,
			// left and top
			CURSOR_DIRECTION_LT = 3,
			// right and top
			CURSOR_DIRECTION_RT = 4,
			// left and bottom
			CURSOR_DIRECTION_LB = 6,
			// right and bottom
			CURSOR_DIRECTION_RB = 7
		};

		ResizableWindow();
		ResizableWindow(bool resizable);
		ResizableWindow(const std::string& caption, bool resizable=true);
		virtual ~ResizableWindow();
        
		/** Sets the size of the area that is active for resize events.
		 * @param border The distance from the edge that is active for resize events.
		 */
		void setResizableBorderDistance(int32_t border);

		/** Gets the size of the area that is active for resize events.
		 * @return The distance from the edge that is active for resize events.
		 */
		int32_t getResizableBorderDistance() const;

		/** Sets if the widget is resizable.
		 * @param resizable True if the widget should be resizeable, otherwise false.
		 */
		void setResizable(bool resizable);

		/** Gets if the widget is resizable.
		 * @return True if the widget should be resizeable, otherwise false.
		 */
		bool isResizable() const;

		/** Sets if the widget is resizable at top.
		 * @param resizable True if the widget should be resizeable at top, otherwise false.
		 */
		void setTopResizable(bool resizable);

		/** Gets if the widget is resizable at top.
		 * @return True if the widget should be resizeable at top, otherwise false.
		 */
		bool isTopResizable() const;

		/** Sets if the widget is resizable at right.
		 * @param resizable True if the widget should be resizeable at right, otherwise false.
		 */
		void setRightResizable(bool resizable);

		/** Gets if the widget is resizable at right.
		 * @return True if the widget should be resizeable at right, otherwise false.
		 */
		bool isRightResizable() const;

		/** Sets if the widget is resizable at bottom.
		 * @param resizable True if the widget should be resizeable at bottom, otherwise false.
		 */
		void setBottomResizable(bool resizable);

		/** Gets if the widget is resizable at bottom.
		 * @return True if the widget should be resizeable at bottom, otherwise false.
		 */
		bool isBottomResizable() const;

		/** Sets if the widget is resizable at left.
		 * @param resizable True if the widget should be resizeable at left, otherwise false.
		 */
		void setLeftResizable(bool resizable);

		/** Gets if the widget is resizable at left.
		 * @return True if the widget should be resizeable at left, otherwise false.
		 */
		bool isLeftResizable() const;

		/** Sets if the widget should be pushed if the size reaches the minimum.
		 * @param shove True if the widget should be pushed, otherwise false.
		 */
		void setShove(bool shove);

		/** Gets if the widget should be pushed if the size reaches the minimum.
		 * @return True if the widget should be pushed, otherwise false.
		 */
		bool getShove() const;

		/** Sets the mouse cursor for the specified direction.
		 * @param cursor_id For native cursors, this is the resource id to native cursor, or one of the values in NativeCursor
		 */
		void set(CursorDirections direction, uint32_t cursor_id=0);

		/** Sets the mouse cursor type to image for the specified direction.
		 * @param image ImagePtr to a image used for the cursor
		 */
		void set(CursorDirections direction, FIFE::ImagePtr image);

		/** Sets the mouse cursor type to animation for the specified direction.
		 * @param anim AnimationPtr to a loaded animation used for the cursor
		 */
		void set(CursorDirections direction, FIFE::AnimationPtr anim);

		/** Gets the mouse cursor type for the specified direction.
		 */
		FIFE::MouseCursorType getType(CursorDirections direction) const;

		/** Gets the mouse cursor handle for the specified direction.
		 */
		uint32_t getId(CursorDirections direction) const;

		/** Gets the mouse image for the specified direction.
		 */
		FIFE::ImagePtr getImage(CursorDirections direction);

		/** Gets the mouse animation for the specified direction.
		 */
		FIFE::AnimationPtr getAnimation(CursorDirections direction);

		// Inherited from Window

		virtual void resizeToContent(bool recursiv=true);

		// Inherited from FocusListener
	
		virtual void focusLost(const Event& event);
	
	
		// Inherited from Window / MouseListener

		virtual void mouseEntered(MouseEvent& mouseEvent);

		virtual void mouseExited(MouseEvent& mouseEvent);

		virtual void mousePressed(MouseEvent& mouseEvent);

		virtual void mouseReleased(MouseEvent& mouseEvent);

		virtual void mouseMoved(MouseEvent& mouseEvent);
	
		virtual void mouseDragged(MouseEvent& mouseEvent);

	protected:
		// initialized the 8 cursors, see CursorDirections
		void initCursors();
		// restore cursor
		void restoreCursor();
		// save cursor so we can restore it later
		void saveCursor();

		// access to fifes cursor class
		FIFE::Cursor* m_cursor;
		// distance from the widgets edge that is active for resizing events
		int32_t m_borderDistance;
		// is resizable
		bool m_resizable;
		// allow resize on top
		bool m_resizableTop;
		// allow resize on right
		bool m_resizableRight;
		// allow resize on bottom
		bool m_resizableBottom;
		// allow resize on left
		bool m_resizableLeft;
		// is currently resizing
		bool m_resizing;

		// indicate the changed size per direction
		bool m_resizeTop;
		bool m_resizeRight;
		bool m_resizeBottom;
		bool m_resizeLeft;
		// is shove allowed
		bool m_shove;

		struct CursorState	{
			FIFE::MouseCursorType cursorType;
			uint32_t cursorId;
			FIFE::ImagePtr cursorImage;
			FIFE::AnimationPtr cursorAnimation;
		};
		// old saved cursor
		CursorState m_saved;
		// contains the 8 CursorStates, one per direction
		std::vector<CursorState> m_cursors;
	};
}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
