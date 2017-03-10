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

#ifndef FIFE_GUI_CEGuiInputProcessor
#define FIFE_GUI_CEGuiInputProcessor

// Standard C++ library includes

// 3rd party library includes
#include <SDL_events.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {
	
	class CEGuiInputProcessor {
	public:
		
		/** Constructor.
		 */
		CEGuiInputProcessor();
		
		/** Destructor.
		 */
		~CEGuiInputProcessor();
		
		/** Injects input to the CEGUI system.
		 * 
		 * @return A boolean value indicating if the event was consumed or not.
		 */
		bool onSdlEvent(SDL_Event& event);
		
	private:
		
		/** Process a key input event.
		 */
		bool processKeyInput(SDL_Event& event);
		
		/** Process a text input event.
		 */
		bool processTextInput(SDL_Event& event);

		/** Process a mouse input event.
		 */
		bool processMouseInput(SDL_Event& event);
		
		/** Process a mouse motion event.
		 */
		bool processMouseMotion(SDL_Event& event);

		/** Initialize the key translation map.
		 */
		void initializeKeyMap();

		/** Holds translation of key scancodes from SDL to CEGUI.
		 */
		std::map<SDL_Keycode, CEGUI::Key::Scan> m_keymap;
	};
	
}

#endif //FIFE_GUI_CEGuiInputProcessor
