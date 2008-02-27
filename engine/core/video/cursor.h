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

#ifndef FIFE_CURSOR_H
#define FIFE_CURSOR_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

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
	
	/**  Cursor class manages mouse cursor handling
	 */
	class Cursor {
	public:
		/** Constructor.
		 */
		Cursor(ImagePool* imgpool, AnimationPool* animpool, RenderBackend* renderbackend);

		/** Destructor.
		 */
		virtual ~Cursor() {}

		/** draws cursor on screen
		 */
		virtual void draw();

		/** Sets the current mouse cursor type and possible pool value
		 * @param ctype cursor type
		 * @param cursor_id pool id for the cursor (either image or animation)
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
		
	
	private:
		unsigned int m_cursor_id;
		unsigned int m_drag_id;
		MouseCursorType m_cursor_type;
		MouseCursorType m_drag_type;
		
		RenderBackend* m_renderbackend;
		ImagePool* m_imgpool;
		AnimationPool* m_animpool;
		
		unsigned int m_animtime;
		unsigned int m_drag_animtime;
		
		int m_drag_offset_x;
		int m_drag_offset_y;
		TimeManager* m_timemanager;
	};

} //FIFE

#endif
