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
#include <iostream>

// 3rd party library includes
#include <Rocket/Core/Context.h>
#include <Rocket/Core/Input.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "librocketinputprocessor.h"

namespace FIFE {
	
	LibRocketInputProcessor::LibRocketInputProcessor(Rocket::Core::Context* context) 
	: 
	m_context(context),
	m_keyModState(0),
	m_wheelCounter(0) {
		populateKeyMap();
	}
	
	LibRocketInputProcessor::~LibRocketInputProcessor() {
	}
	
	void LibRocketInputProcessor::updateKeyModState() {
		
		SDLMod modState = SDL_GetModState();
		
		m_keyModState = 0;
		
		if((modState & KMOD_NONE) != KMOD_NONE) {
			
			if((modState & KMOD_SHIFT) == KMOD_SHIFT) {
				m_keyModState |= Rocket::Core::Input::KM_SHIFT;
			}
			
			if((modState & KMOD_CTRL) == KMOD_CTRL) {
				m_keyModState |= Rocket::Core::Input::KM_CTRL;
			}
			
			if((modState & KMOD_ALT) == KMOD_ALT) {
				m_keyModState |= Rocket::Core::Input::KM_ALT;
			}
			// KMOD_META is gone so we change it to KMOD_GUI
			if((modState & KMOD_GUI) == KMOD_GUI) {
				m_keyModState |= Rocket::Core::Input::KM_META;
			}
			
			if((modState & KMOD_NUM) == KMOD_NUM) {
				m_keyModState |= Rocket::Core::Input::KM_NUMLOCK;
			}
			
			if((modState & KMOD_CAPS) == KMOD_CAPS) {
				m_keyModState |= Rocket::Core::Input::KM_CAPSLOCK;
			}
			
		}
		
	}
	
	bool LibRocketInputProcessor::onSdlEvent(SDL_Event& event) {
		
		bool consumed = false;
		
		updateKeyModState();
		
		switch(event.type) {
			
			case SDL_KEYUP:
			case SDL_KEYDOWN:
				consumed = processKeyInput(event);
				break;
				
			case SDL_TEXTINPUT:
				consumed = processTextInput(event);
				break;

			case SDL_MOUSEWHEEL:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
				consumed = processMouseInput(event);
				break;
				
			case SDL_MOUSEMOTION:
				consumed = processMouseMotion(event);
				break;
			
			default:
				break;
		}
		
		return consumed;
	}
	
	void LibRocketInputProcessor::turn() {
		if(m_wheelCounter != 0) {
			m_context->ProcessMouseWheel(m_wheelCounter, m_keyModState);
			m_wheelCounter = 0;
		}
	}
	
	bool LibRocketInputProcessor::processMouseMotion(SDL_Event& event) {
		
		int x = static_cast<int>(event.motion.x);
		int y = static_cast<int>(event.motion.y);
		
		m_context->ProcessMouseMove(x, y, m_keyModState);
		
		return false;
	}
	
	bool LibRocketInputProcessor::processMouseInput(SDL_Event& event) {
		
		int index = (event.button.button == SDL_BUTTON_LEFT)   ? 0 : 
					(event.button.button == SDL_BUTTON_RIGHT)  ? 1 :
					(event.button.button == SDL_BUTTON_MIDDLE) ? 2 : 3;

		
		if(event.type == SDL_MOUSEBUTTONDOWN) {
			m_context->ProcessMouseButtonDown(index, m_keyModState);
		} else if (event.type == SDL_MOUSEBUTTONUP) {
			m_context->ProcessMouseButtonUp(index, m_keyModState);
		} else if (event.type == SDL_MOUSEWHEEL) {
			processMouseWheelMotion(event);
		}
		
		return false;
	}
	
	bool LibRocketInputProcessor::processMouseWheelMotion(SDL_Event& event) {
		// mousewheel up
		//if (event.wheel.y > 0 || (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED && event.wheel.y < 0)) {
		if (event.wheel.y > 0) {
			if(m_wheelCounter <= 0) {
				m_wheelCounter--;
			} else {
				//the wheel had been moving downwards so sent those movements before reseting the counter
				m_context->ProcessMouseWheel(m_wheelCounter, m_keyModState);
				m_wheelCounter = -1;
			}
		}
		// mousewheel down
		//else if (event.wheel.y < 0 || (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED && event.wheel.y > 0)) {
		else if (event.wheel.y < 0) {
			if(m_wheelCounter >= 0) {
				m_wheelCounter++;
			} else {
				//the wheel had been moving upwards so sent those movements before reseting the counter
				m_context->ProcessMouseWheel(m_wheelCounter, m_keyModState);
				m_wheelCounter = 1;
			}
		}
		
		return false;
	}
	
	bool LibRocketInputProcessor::processKeyInput(SDL_Event& event) {
		
		Rocket::Core::Input::KeyIdentifier key = m_keyMap[event.key.keysym.sym];
		
		if(event.type == SDL_KEYDOWN) {
			
			m_context->ProcessKeyDown(key, m_keyModState);
			
			if(key == Rocket::Core::Input::KI_RETURN) {
				m_context->ProcessTextInput((Rocket::Core::word) '\n');
			}
				
		} else {
			m_context->ProcessKeyUp(m_keyMap[event.key.keysym.sym], m_keyModState);
		}
		
		return false;
	}
	
	bool LibRocketInputProcessor::processTextInput(SDL_Event& event) {
		Rocket::Core::String text(event.text.text);
		m_context->ProcessTextInput(text);

		return false;
	}

	void LibRocketInputProcessor::populateKeyMap() {
		
		m_keyMap[SDLK_UNKNOWN] = Rocket::Core::Input::KI_UNKNOWN;
		m_keyMap[SDLK_SPACE] = Rocket::Core::Input::KI_SPACE;
		m_keyMap[SDLK_0] = Rocket::Core::Input::KI_0;
		m_keyMap[SDLK_1] = Rocket::Core::Input::KI_1;
		m_keyMap[SDLK_2] = Rocket::Core::Input::KI_2;
		m_keyMap[SDLK_3] = Rocket::Core::Input::KI_3;
		m_keyMap[SDLK_4] = Rocket::Core::Input::KI_4;
		m_keyMap[SDLK_5] = Rocket::Core::Input::KI_5;
		m_keyMap[SDLK_6] = Rocket::Core::Input::KI_6;
		m_keyMap[SDLK_7] = Rocket::Core::Input::KI_7;
		m_keyMap[SDLK_8] = Rocket::Core::Input::KI_8;
		m_keyMap[SDLK_9] = Rocket::Core::Input::KI_9;
		m_keyMap[SDLK_a] = Rocket::Core::Input::KI_A;
		m_keyMap[SDLK_b] = Rocket::Core::Input::KI_B;
		m_keyMap[SDLK_c] = Rocket::Core::Input::KI_C;
		m_keyMap[SDLK_d] = Rocket::Core::Input::KI_D;
		m_keyMap[SDLK_e] = Rocket::Core::Input::KI_E;
		m_keyMap[SDLK_f] = Rocket::Core::Input::KI_F;
		m_keyMap[SDLK_g] = Rocket::Core::Input::KI_G;
		m_keyMap[SDLK_h] = Rocket::Core::Input::KI_H;
		m_keyMap[SDLK_i] = Rocket::Core::Input::KI_I;
		m_keyMap[SDLK_j] = Rocket::Core::Input::KI_J;
		m_keyMap[SDLK_k] = Rocket::Core::Input::KI_K;
		m_keyMap[SDLK_l] = Rocket::Core::Input::KI_L;
		m_keyMap[SDLK_m] = Rocket::Core::Input::KI_M;
		m_keyMap[SDLK_n] = Rocket::Core::Input::KI_N;
		m_keyMap[SDLK_o] = Rocket::Core::Input::KI_O;
		m_keyMap[SDLK_p] = Rocket::Core::Input::KI_P;
		m_keyMap[SDLK_q] = Rocket::Core::Input::KI_Q;
		m_keyMap[SDLK_r] = Rocket::Core::Input::KI_R;
		m_keyMap[SDLK_s] = Rocket::Core::Input::KI_S;
		m_keyMap[SDLK_t] = Rocket::Core::Input::KI_T;
		m_keyMap[SDLK_u] = Rocket::Core::Input::KI_U;
		m_keyMap[SDLK_v] = Rocket::Core::Input::KI_V;
		m_keyMap[SDLK_w] = Rocket::Core::Input::KI_W;
		m_keyMap[SDLK_x] = Rocket::Core::Input::KI_X;
		m_keyMap[SDLK_y] = Rocket::Core::Input::KI_Y;
		m_keyMap[SDLK_z] = Rocket::Core::Input::KI_Z;
		m_keyMap[SDLK_SEMICOLON] = Rocket::Core::Input::KI_OEM_1;
		m_keyMap[SDLK_PLUS] = Rocket::Core::Input::KI_OEM_PLUS;
		m_keyMap[SDLK_COMMA] = Rocket::Core::Input::KI_OEM_COMMA;
		m_keyMap[SDLK_MINUS] = Rocket::Core::Input::KI_OEM_MINUS;
		m_keyMap[SDLK_PERIOD] = Rocket::Core::Input::KI_OEM_PERIOD;
		m_keyMap[SDLK_SLASH] = Rocket::Core::Input::KI_OEM_2;
		m_keyMap[SDLK_BACKQUOTE] = Rocket::Core::Input::KI_OEM_3;
		m_keyMap[SDLK_LEFTBRACKET] = Rocket::Core::Input::KI_OEM_4;
		m_keyMap[SDLK_BACKSLASH] = Rocket::Core::Input::KI_OEM_5;
		m_keyMap[SDLK_RIGHTBRACKET] = Rocket::Core::Input::KI_OEM_6;
		m_keyMap[SDLK_QUOTEDBL] = Rocket::Core::Input::KI_OEM_7;
		m_keyMap[SDLK_KP_0] = Rocket::Core::Input::KI_NUMPAD0;
		m_keyMap[SDLK_KP_1] = Rocket::Core::Input::KI_NUMPAD1;
		m_keyMap[SDLK_KP_2] = Rocket::Core::Input::KI_NUMPAD2;
		m_keyMap[SDLK_KP_3] = Rocket::Core::Input::KI_NUMPAD3;
		m_keyMap[SDLK_KP_4] = Rocket::Core::Input::KI_NUMPAD4;
		m_keyMap[SDLK_KP_5] = Rocket::Core::Input::KI_NUMPAD5;
		m_keyMap[SDLK_KP_6] = Rocket::Core::Input::KI_NUMPAD6;
		m_keyMap[SDLK_KP_7] = Rocket::Core::Input::KI_NUMPAD7;
		m_keyMap[SDLK_KP_8] = Rocket::Core::Input::KI_NUMPAD8;
		m_keyMap[SDLK_KP_9] = Rocket::Core::Input::KI_NUMPAD9;
		m_keyMap[SDLK_KP_ENTER] = Rocket::Core::Input::KI_NUMPADENTER;
		m_keyMap[SDLK_KP_MULTIPLY] = Rocket::Core::Input::KI_MULTIPLY;
		m_keyMap[SDLK_KP_PLUS] = Rocket::Core::Input::KI_ADD;
		m_keyMap[SDLK_KP_MINUS] = Rocket::Core::Input::KI_SUBTRACT;
		m_keyMap[SDLK_KP_PERIOD] = Rocket::Core::Input::KI_DECIMAL;
		m_keyMap[SDLK_KP_DIVIDE] = Rocket::Core::Input::KI_DIVIDE;
		m_keyMap[SDLK_KP_EQUALS] = Rocket::Core::Input::KI_OEM_NEC_EQUAL;
		m_keyMap[SDLK_BACKSPACE] = Rocket::Core::Input::KI_BACK;
		m_keyMap[SDLK_TAB] = Rocket::Core::Input::KI_TAB;
		m_keyMap[SDLK_CLEAR] = Rocket::Core::Input::KI_CLEAR;
		m_keyMap[SDLK_RETURN] = Rocket::Core::Input::KI_RETURN;
		m_keyMap[SDLK_PAUSE] = Rocket::Core::Input::KI_PAUSE;
		m_keyMap[SDLK_CAPSLOCK] = Rocket::Core::Input::KI_CAPITAL;
		m_keyMap[SDLK_PAGEUP] = Rocket::Core::Input::KI_PRIOR;
		m_keyMap[SDLK_PAGEDOWN] = Rocket::Core::Input::KI_NEXT;
		m_keyMap[SDLK_END] = Rocket::Core::Input::KI_END;
		m_keyMap[SDLK_HOME] = Rocket::Core::Input::KI_HOME;
		m_keyMap[SDLK_LEFT] = Rocket::Core::Input::KI_LEFT;
		m_keyMap[SDLK_UP] = Rocket::Core::Input::KI_UP;
		m_keyMap[SDLK_RIGHT] = Rocket::Core::Input::KI_RIGHT;
		m_keyMap[SDLK_DOWN] = Rocket::Core::Input::KI_DOWN;
		m_keyMap[SDLK_INSERT] = Rocket::Core::Input::KI_INSERT;
		m_keyMap[SDLK_DELETE] = Rocket::Core::Input::KI_DELETE;
		m_keyMap[SDLK_HELP] = Rocket::Core::Input::KI_HELP;
		m_keyMap[SDLK_LGUI] = Rocket::Core::Input::KI_LWIN;
		m_keyMap[SDLK_RGUI] = Rocket::Core::Input::KI_RWIN;
		m_keyMap[SDLK_F1] = Rocket::Core::Input::KI_F1;
		m_keyMap[SDLK_F2] = Rocket::Core::Input::KI_F2;
		m_keyMap[SDLK_F3] = Rocket::Core::Input::KI_F3;
		m_keyMap[SDLK_F4] = Rocket::Core::Input::KI_F4;
		m_keyMap[SDLK_F5] = Rocket::Core::Input::KI_F5;
		m_keyMap[SDLK_F6] = Rocket::Core::Input::KI_F6;
		m_keyMap[SDLK_F7] = Rocket::Core::Input::KI_F7;
		m_keyMap[SDLK_F8] = Rocket::Core::Input::KI_F8;
		m_keyMap[SDLK_F9] = Rocket::Core::Input::KI_F9;
		m_keyMap[SDLK_F10] = Rocket::Core::Input::KI_F10;
		m_keyMap[SDLK_F11] = Rocket::Core::Input::KI_F11;
		m_keyMap[SDLK_F12] = Rocket::Core::Input::KI_F12;
		m_keyMap[SDLK_F13] = Rocket::Core::Input::KI_F13;
		m_keyMap[SDLK_F14] = Rocket::Core::Input::KI_F14;
		m_keyMap[SDLK_F15] = Rocket::Core::Input::KI_F15;
		m_keyMap[SDLK_NUMLOCKCLEAR] = Rocket::Core::Input::KI_NUMLOCK;
		m_keyMap[SDLK_SCROLLLOCK] = Rocket::Core::Input::KI_SCROLL;
		m_keyMap[SDLK_LSHIFT] = Rocket::Core::Input::KI_LSHIFT;
		m_keyMap[SDLK_RSHIFT] = Rocket::Core::Input::KI_RSHIFT;
		m_keyMap[SDLK_LCTRL] = Rocket::Core::Input::KI_LCONTROL;
		m_keyMap[SDLK_RCTRL] = Rocket::Core::Input::KI_RCONTROL;
		m_keyMap[SDLK_LALT] = Rocket::Core::Input::KI_LMENU;
		m_keyMap[SDLK_RALT] = Rocket::Core::Input::KI_RMENU;
	}
};
