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
#include <cegui-0/CEGUI/CEGUI.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "ceguiinputprocessor.h"

namespace FIFE {
	CEGuiInputProcessor::CEGuiInputProcessor() {
		initializeKeyMap();
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
				;
		}
		
		return consumed;
	}
	
	bool CEGuiInputProcessor::processKeyInput(SDL_Event& event) {
		bool consumed = false;

		switch(event.type) {
			case SDL_KEYDOWN:
				if (m_keymap.find(event.key.keysym.sym) != m_keymap.end())
					consumed |= CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(m_keymap[event.key.keysym.sym]);
				break;
				
			case SDL_KEYUP:
				if (m_keymap.find(event.key.keysym.sym) != m_keymap.end())
					consumed = CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(m_keymap[event.key.keysym.sym]);
				break;
				
			default:
				;
		}
		
		return consumed;
	}
	
	bool CEGuiInputProcessor::processTextInput(SDL_Event& event) {
		CEGUI::String character(event.text.text);
		bool consumed = CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(character[0]);

		return consumed;
	}

	bool CEGuiInputProcessor::processMouseInput(SDL_Event& event) {
		bool consumed = false;

		switch(event.type) {
			case SDL_MOUSEBUTTONDOWN:
			{
				switch(event.button.button) {
					case SDL_BUTTON_LEFT:
						consumed = CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::LeftButton);
						break;
						
					case SDL_BUTTON_RIGHT:
						consumed = CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::RightButton);
						break;
						
					case SDL_BUTTON_MIDDLE:
						consumed = CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::MiddleButton) ;
						break;
						
					default:
						;
				}
				break;
			}
				
			case SDL_MOUSEBUTTONUP:
			{
				switch(event.button.button) {
					case SDL_BUTTON_LEFT:
						consumed = CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::LeftButton);
						break;
						
					case SDL_BUTTON_RIGHT:
						consumed = CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::RightButton);
						break;
						
					case SDL_BUTTON_MIDDLE:
						consumed = CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::MiddleButton) ;
						break;
						
					default:
						;
				}
				break;
			}
			
			case SDL_MOUSEWHEEL:
			{
				// mousewheel up or down
				int32_t wheelChange = event.wheel.y;
				if (wheelChange != 0) {
					/*if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
						wheelChange *= -1;
					}*/
					consumed = CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseWheelChange(wheelChange);
				}
				break;
			}

			default:
				;
		}
		
		return consumed;
	}
	
	bool CEGuiInputProcessor::processMouseMotion(SDL_Event& event) {
	    return CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(static_cast<float>(event.motion.x), static_cast<float>(event.motion.y));
	}

	void CEGuiInputProcessor::initializeKeyMap() {
		m_keymap[SDLK_1] = CEGUI::Key::One;
		m_keymap[SDLK_2] = CEGUI::Key::Two;
		m_keymap[SDLK_3] = CEGUI::Key::Three;
		m_keymap[SDLK_4] = CEGUI::Key::Four;
		m_keymap[SDLK_5] = CEGUI::Key::Five;
		m_keymap[SDLK_6] = CEGUI::Key::Six;
		m_keymap[SDLK_7] = CEGUI::Key::Seven;
		m_keymap[SDLK_8] = CEGUI::Key::Eight;
		m_keymap[SDLK_9] = CEGUI::Key::Nine;
		m_keymap[SDLK_0] = CEGUI::Key::Zero;

		m_keymap[SDLK_q] = CEGUI::Key::Q;
		m_keymap[SDLK_w] = CEGUI::Key::W;
		m_keymap[SDLK_e] = CEGUI::Key::E;
		m_keymap[SDLK_r] = CEGUI::Key::R;
		m_keymap[SDLK_t] = CEGUI::Key::T;
		m_keymap[SDLK_y] = CEGUI::Key::Y;
		m_keymap[SDLK_u] = CEGUI::Key::U;
		m_keymap[SDLK_i] = CEGUI::Key::I;
		m_keymap[SDLK_o] = CEGUI::Key::O;
		m_keymap[SDLK_p] = CEGUI::Key::P;
		m_keymap[SDLK_a] = CEGUI::Key::A;
		m_keymap[SDLK_s] = CEGUI::Key::S;
		m_keymap[SDLK_d] = CEGUI::Key::D;
		m_keymap[SDLK_f] = CEGUI::Key::F;
		m_keymap[SDLK_g] = CEGUI::Key::G;
		m_keymap[SDLK_h] = CEGUI::Key::H;
		m_keymap[SDLK_j] = CEGUI::Key::J;
		m_keymap[SDLK_k] = CEGUI::Key::K;
		m_keymap[SDLK_l] = CEGUI::Key::L;
		m_keymap[SDLK_z] = CEGUI::Key::Z;
		m_keymap[SDLK_x] = CEGUI::Key::X;
		m_keymap[SDLK_c] = CEGUI::Key::C;
		m_keymap[SDLK_v] = CEGUI::Key::V;
		m_keymap[SDLK_b] = CEGUI::Key::B;
		m_keymap[SDLK_n] = CEGUI::Key::N;
		m_keymap[SDLK_m] = CEGUI::Key::M;

		m_keymap[SDLK_COMMA] = CEGUI::Key::Comma;
		m_keymap[SDLK_PERIOD] = CEGUI::Key::Period;
		m_keymap[SDLK_SLASH] = CEGUI::Key::Slash;
		m_keymap[SDLK_BACKSLASH] = CEGUI::Key::Backslash;
		m_keymap[SDLK_MINUS] = CEGUI::Key::Minus;
		m_keymap[SDLK_EQUALS] = CEGUI::Key::Equals;
		m_keymap[SDLK_SEMICOLON] = CEGUI::Key::Semicolon;
		m_keymap[SDLK_COLON] = CEGUI::Key::Colon;
		m_keymap[SDLK_LEFTBRACKET] = CEGUI::Key::LeftBracket;
		m_keymap[SDLK_RIGHTBRACKET] = CEGUI::Key::RightBracket;
		m_keymap[SDLK_QUOTE] = CEGUI::Key::Apostrophe;
		m_keymap[SDLK_BACKQUOTE] = CEGUI::Key::Grave;
		m_keymap[SDLK_AT] = CEGUI::Key::At;
		m_keymap[SDLK_UNDERSCORE] = CEGUI::Key::Underline;

		m_keymap[SDLK_RETURN] = CEGUI::Key::Return;
		m_keymap[SDLK_SPACE] = CEGUI::Key::Space;
		m_keymap[SDLK_BACKSPACE] = CEGUI::Key::Backspace;
		m_keymap[SDLK_TAB] = CEGUI::Key::Tab;

		m_keymap[SDLK_ESCAPE] = CEGUI::Key::Escape;
		m_keymap[SDLK_PAUSE] = CEGUI::Key::Pause;
		m_keymap[SDLK_SYSREQ] = CEGUI::Key::SysRq;
		m_keymap[SDLK_POWER] = CEGUI::Key::Power;
		m_keymap[SDLK_SLEEP] = CEGUI::Key::Sleep;

		m_keymap[SDLK_CALCULATOR] = CEGUI::Key::Calculator;
		m_keymap[SDLK_MAIL] = CEGUI::Key::Mail;
		m_keymap[SDLK_COMPUTER] = CEGUI::Key::MyComputer;
		m_keymap[SDLK_MEDIASELECT] = CEGUI::Key::MediaSelect;
		m_keymap[SDLK_AC_STOP] = CEGUI::Key::Stop;

		m_keymap[SDLK_AUDIOPLAY] = CEGUI::Key::PlayPause;
		m_keymap[SDLK_AUDIOSTOP] = CEGUI::Key::MediaStop;
		m_keymap[SDLK_AUDIOPREV] = CEGUI::Key::PrevTrack;
		m_keymap[SDLK_AUDIONEXT] = CEGUI::Key::NextTrack;
		m_keymap[SDLK_AUDIOMUTE] = CEGUI::Key::Mute;
		m_keymap[SDLK_VOLUMEUP] = CEGUI::Key::VolumeUp;
		m_keymap[SDLK_VOLUMEDOWN] = CEGUI::Key::VolumeDown;

		m_keymap[SDLK_AC_BACK] = CEGUI::Key::WebBack;
		m_keymap[SDLK_AC_FORWARD] = CEGUI::Key::WebForward;
		m_keymap[SDLK_AC_HOME] = CEGUI::Key::WebHome;
		m_keymap[SDLK_AC_BOOKMARKS] = CEGUI::Key::WebFavorites;
		m_keymap[SDLK_AC_SEARCH] = CEGUI::Key::WebSearch;
		m_keymap[SDLK_AC_REFRESH] = CEGUI::Key::WebRefresh;
		m_keymap[SDLK_AC_STOP] = CEGUI::Key::WebStop;

		m_keymap[SDLK_NUMLOCKCLEAR] = CEGUI::Key::NumLock;
		m_keymap[SDLK_SCROLLLOCK] = CEGUI::Key::ScrollLock;
		m_keymap[SDLK_CAPSLOCK] = CEGUI::Key::Capital;

		m_keymap[SDLK_F1] = CEGUI::Key::F1;
		m_keymap[SDLK_F2] = CEGUI::Key::F2;
		m_keymap[SDLK_F3] = CEGUI::Key::F3;
		m_keymap[SDLK_F4] = CEGUI::Key::F4;
		m_keymap[SDLK_F5] = CEGUI::Key::F5;
		m_keymap[SDLK_F6] = CEGUI::Key::F6;
		m_keymap[SDLK_F7] = CEGUI::Key::F7;
		m_keymap[SDLK_F8] = CEGUI::Key::F8;
		m_keymap[SDLK_F9] = CEGUI::Key::F9;
		m_keymap[SDLK_F10] = CEGUI::Key::F10;
		m_keymap[SDLK_F11] = CEGUI::Key::F11;
		m_keymap[SDLK_F12] = CEGUI::Key::F12;
		m_keymap[SDLK_F13] = CEGUI::Key::F13;
		m_keymap[SDLK_F14] = CEGUI::Key::F14;
		m_keymap[SDLK_F15] = CEGUI::Key::F15;

		m_keymap[SDLK_LCTRL] = CEGUI::Key::LeftControl;
		m_keymap[SDLK_LALT] = CEGUI::Key::LeftAlt;
		m_keymap[SDLK_LSHIFT] = CEGUI::Key::LeftShift;
		m_keymap[SDLK_LGUI] = CEGUI::Key::LeftWindows;
		m_keymap[SDLK_RCTRL] = CEGUI::Key::RightControl;
		m_keymap[SDLK_RALT] = CEGUI::Key::RightAlt;
		m_keymap[SDLK_RSHIFT] = CEGUI::Key::RightShift;
		m_keymap[SDLK_RGUI] = CEGUI::Key::RightWindows;
		m_keymap[SDLK_MENU] = CEGUI::Key::AppMenu;

		m_keymap[SDLK_KP_0] = CEGUI::Key::Numpad0;
		m_keymap[SDLK_KP_1] = CEGUI::Key::Numpad1;
		m_keymap[SDLK_KP_2] = CEGUI::Key::Numpad2;
		m_keymap[SDLK_KP_3] = CEGUI::Key::Numpad3;
		m_keymap[SDLK_KP_4] = CEGUI::Key::Numpad4;
		m_keymap[SDLK_KP_5] = CEGUI::Key::Numpad5;
		m_keymap[SDLK_KP_6] = CEGUI::Key::Numpad6;
		m_keymap[SDLK_KP_7] = CEGUI::Key::Numpad7;
		m_keymap[SDLK_KP_8] = CEGUI::Key::Numpad8;
		m_keymap[SDLK_KP_9] = CEGUI::Key::Numpad9;
		m_keymap[SDLK_KP_PERIOD] = CEGUI::Key::Decimal;
		m_keymap[SDLK_KP_PLUS] = CEGUI::Key::Add;
		m_keymap[SDLK_KP_MINUS] = CEGUI::Key::Subtract;
		m_keymap[SDLK_KP_MULTIPLY] = CEGUI::Key::Multiply;
		m_keymap[SDLK_KP_DIVIDE] = CEGUI::Key::Divide;
		m_keymap[SDLK_KP_ENTER] = CEGUI::Key::NumpadEnter;
		m_keymap[SDLK_KP_COMMA] = CEGUI::Key::NumpadComma;
		m_keymap[SDLK_KP_EQUALS] = CEGUI::Key::NumpadEquals;

		m_keymap[SDLK_UP] = CEGUI::Key::ArrowUp;
		m_keymap[SDLK_LEFT] = CEGUI::Key::ArrowLeft;
		m_keymap[SDLK_RIGHT] = CEGUI::Key::ArrowRight;
		m_keymap[SDLK_DOWN] = CEGUI::Key::ArrowDown;

		m_keymap[SDLK_HOME] = CEGUI::Key::Home;
		m_keymap[SDLK_END] = CEGUI::Key::End;
		m_keymap[SDLK_PAGEUP] = CEGUI::Key::PageUp;
		m_keymap[SDLK_PAGEDOWN] = CEGUI::Key::PageDown;
		m_keymap[SDLK_INSERT] = CEGUI::Key::Insert;
		m_keymap[SDLK_DELETE] = CEGUI::Key::Delete;
	}
}
