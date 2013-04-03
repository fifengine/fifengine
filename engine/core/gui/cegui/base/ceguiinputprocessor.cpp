/***************************************************************************
 *   Copyright (C) 2005-2012 by the FIFE team                              *
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
#include <CEGUI/CEGUI.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "ceguiinputprocessor.h"

namespace FIFE {
	CEGuiInputProcessor::CEGuiInputProcessor() {
	}
	
	CEGuiInputProcessor::~CEGuiInputProcessor() {
	}
	
	bool CEGuiInputProcessor::onSdlEvent(SDL_Event& event) {
		bool consumed = false;
		
		switch(event.type) {
			
			case SDL_KEYUP:
			case SDL_KEYDOWN:
				consumed = processKeyInput(event);
				break;
				
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
				consumed = processMouseInput(event);
				break;
				
			case SDL_MOUSEMOTION:
				consumed = processMouseMotion(event);
				break;
			
			default:
				;
		}
		
		return consumed;
	}
	
	bool CEGuiInputProcessor::processKeyInput(SDL_Event& event) {
                bool consumed = false;

		switch(event.type) {
			case SDL_KEYDOWN:
				if((event.key.keysym.unicode & 0xFF80) == 0)
				{
					consumed = CEGUI::System::getSingleton().injectChar(event.key.keysym.unicode & 0x7F);
				}
				break;
				
			case SDL_KEYUP:
				consumed = CEGUI::System::getSingleton().injectKeyUp(event.key.keysym.scancode);
				break;
				
			default:
				;
		}
		
		return consumed;
	}
	
	bool CEGuiInputProcessor::processMouseInput(SDL_Event& event) {
                bool consumed = false;

		switch(event.type) {
			case SDL_MOUSEBUTTONDOWN:
				switch(event.button.button) {
					case SDL_BUTTON_LEFT:
						consumed = CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
						break;
						
					case SDL_BUTTON_RIGHT:
						consumed = CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::RightButton);
						break;
						
					case SDL_BUTTON_MIDDLE:
						consumed = CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::MiddleButton) ;
						break;
						
					case SDL_BUTTON_WHEELDOWN:
						consumed = CEGUI::System::getSingleton().injectMouseWheelChange(-1);
						break;
						
					case SDL_BUTTON_WHEELUP:
						consumed = CEGUI::System::getSingleton().injectMouseWheelChange(1);
						break;
						
					default:
						;
				}
				break;
				
			case SDL_MOUSEBUTTONUP:
				switch(event.button.button) {
					case SDL_BUTTON_LEFT:
						consumed = CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
						break;
						
					case SDL_BUTTON_RIGHT:
						consumed = CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::RightButton);
						break;
						
					case SDL_BUTTON_MIDDLE:
						consumed = CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::MiddleButton) ;
						break;
						
					case SDL_BUTTON_WHEELDOWN:
						break;
						
					case SDL_BUTTON_WHEELUP:
						break;
						
					default:
						;
				}
				break;
				
			default:
				;
		}
		
		return consumed;
	}
	
	bool CEGuiInputProcessor::processMouseMotion(SDL_Event& event) {
	    return CEGUI::System::getSingleton().injectMousePosition(static_cast<float>(event.motion.x), static_cast<float>(event.motion.y));
	}
}
