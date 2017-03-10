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

#ifndef FIFE_GUI_LIBROCKETINPUTPROCESSOR_H
#define FIFE_GUI_LIBROCKETINPUTPROCESSOR_H

// Standard C++ library includes
#include <map>

// 3rd party library includes
#include <Rocket/Core/Input.h>
#include <Rocket/Core/Types.h>
#include <SDL_events.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fife_stdint.h"

namespace Rocket {
	namespace Core {
		class Context;
	}
}

namespace FIFE {
	
	class LibRocketInputProcessor { 
	public:
		
		/** Constructor
		 */
		LibRocketInputProcessor(Rocket::Core::Context* context);
		
		/** Destructor
		 */
		~LibRocketInputProcessor();
		
		/** Processes SDL input and converts it to librocket input, then forwards it to
		 * the librocket context.
         * 
		 * NOTE There is no way to get, if an event is consumed by Rocket currently.
		 *      This is for in case it gets implemented in the future.
		 * 
		 * @param evt The SDL input.
		 * @return A boolean value indicating if the event was consumed by librocket or not.
    
		 */
		bool onSdlEvent(SDL_Event &evt);
		
		/**
		 * Called each frame to perform update operations.
		 */
		void turn();
		
	private:
		
	    /** Updates the key mod state bitmask.
		 */
		void updateKeyModState();
		
		/** Process a mouse motion event.
		 */
		bool processMouseMotion(SDL_Event& event);
		
		/** Process a mouse input event.
		 */
		bool processMouseInput(SDL_Event& event);
		
		/** Process a mouse wheel motion event.
		 */
		bool processMouseWheelMotion(SDL_Event& event);
		
		/** Process a key input event.
		 */
		bool processKeyInput(SDL_Event& event);
		
		/** Process a text input event.
		 */
		bool processTextInput(SDL_Event& event);

		/** Creates the key map.
		 */
		void populateKeyMap();
		
		/** Reference to librocket's context.
		 */
		Rocket::Core::Context* m_context;
		
		/** Bitmask that stores key modifiers.
		 */
		uint32_t m_keyModState;
		
		/** Counts how many times the wheel has been moved. Negative
		 * value means that the wheel has been moved abs(m_wheelCounter) upwards,
		 * positive value means that the wheel has been moved m_wheelCounter times downwards.
		 */
		int32_t m_wheelCounter;
		
		/** Keymap to convert SDL key to Librocket key.
		 */
		std::map<SDL_Keycode, Rocket::Core::Input::KeyIdentifier> m_keyMap;
	};
	
};

#endif //FIFE_GUI_LIBROCKETINPUTPROCESSOR_H