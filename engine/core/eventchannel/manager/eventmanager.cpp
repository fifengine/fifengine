/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
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

// Standard C++ library includes
#include <iostream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/exception.h"
#include "eventchannel/key/ec_key.h"
#include "eventchannel/key/ec_keyevent.h"
#include "eventchannel/mouse/ec_mouseevent.h"
#include "eventchannel/widget/ec_widgetevent.h"
#include "eventchannel/command/ec_command.h"

#include "eventmanager.h"

namespace FIFE {

	EventManager::EventManager(): 
		m_commandlisteners(), 
		m_keylisteners(),
		m_mouselisteners(), 
		m_sdleventlisteners(), 
		m_widgetlisteners(),
		m_nonconsumablekeys(),
		m_keystatemap(),
		m_mousestate(0),
		m_mostrecentbtn(IMouseEvent::EMPTY)
 	{
	}

	EventManager::~EventManager() {
	}

	void EventManager::fillMouseEvent(const SDL_Event& sdlevt, MouseEvent& mouseevt) {
		mouseevt.setX(sdlevt.button.x);
		mouseevt.setY(sdlevt.button.y);
		mouseevt.setButton(IMouseEvent::EMPTY);
		mouseevt.setType(IMouseEvent::MOVED);
		if ((sdlevt.type == SDL_MOUSEBUTTONUP) || (sdlevt.type == SDL_MOUSEBUTTONDOWN)) {
			switch (sdlevt.button.button) {
				case SDL_BUTTON_LEFT:
					mouseevt.setButton(IMouseEvent::LEFT);
					break;
				case SDL_BUTTON_RIGHT:
					mouseevt.setButton(IMouseEvent::RIGHT);
					break;
				case SDL_BUTTON_MIDDLE:
					mouseevt.setButton(IMouseEvent::MIDDLE);
					break;
				default:
					mouseevt.setButton(IMouseEvent::UNKNOWN_BUTTON);
					break;
			}
			switch (sdlevt.button.button) {
				case SDL_BUTTON_WHEELDOWN:
					mouseevt.setType(IMouseEvent::WHEEL_MOVED_DOWN);
					break;
				case SDL_BUTTON_WHEELUP:
					mouseevt.setType(IMouseEvent::WHEEL_MOVED_UP);
					break;
				default:
					break;
			}
			if (sdlevt.type == SDL_MOUSEBUTTONUP) {
				mouseevt.setType(IMouseEvent::RELEASED);
			} else {
				mouseevt.setType(IMouseEvent::PRESSED);
			}
		}
		if ((mouseevt.getType() == IMouseEvent::MOVED) && m_mousestate) {
			mouseevt.setType(IMouseEvent::DRAGGED);
			mouseevt.setButton(m_mostrecentbtn);
		}
	}

	void EventManager::fillKeyEvent(const SDL_Event& sdlevt, KeyEvent& keyevt) {
		if (sdlevt.type == SDL_KEYDOWN) {
			keyevt.setType(IKeyEvent::PRESSED);
		} else if (sdlevt.type == SDL_KEYUP) {
			keyevt.setType(IKeyEvent::RELEASED);
		} else {
			throw EventException("Invalid event type in fillKeyEvent");
		}
		keyevt.setShiftPressed(sdlevt.key.keysym.mod & KMOD_SHIFT);
		keyevt.setControlPressed(sdlevt.key.keysym.mod & KMOD_CTRL);
		keyevt.setAltPressed(sdlevt.key.keysym.mod & KMOD_ALT);
		keyevt.setMetaPressed(sdlevt.key.keysym.mod & KMOD_META);
		keyevt.setNumericPad(sdlevt.key.keysym.sym >= SDLK_KP0
							&& sdlevt.key.keysym.sym <= SDLK_KP_EQUALS);

		SDL_keysym keysym = sdlevt.key.keysym;
		IKey::KeyType value = IKey::INVALID_KEY;
		std::string repr("");
		
		if (keysym.unicode < 255) {
			value = static_cast<IKey::KeyType>(keysym.unicode);
			repr.push_back(value);
		}

		#define MAP_KEY(_orig, _new) case _orig: value = _new; repr = #_new; break;
		switch (keysym.sym) {
			MAP_KEY(SDLK_TAB, IKey::TAB);
			MAP_KEY(SDLK_LALT, IKey::LEFT_ALT);
			MAP_KEY(SDLK_RALT, IKey::RIGHT_ALT);
			MAP_KEY(SDLK_LSHIFT, IKey::LEFT_SHIFT);
			MAP_KEY(SDLK_RSHIFT, IKey::RIGHT_SHIFT);
			MAP_KEY(SDLK_LCTRL, IKey::LEFT_CONTROL);
			MAP_KEY(SDLK_RCTRL, IKey::RIGHT_CONTROL);
			MAP_KEY(SDLK_BACKSPACE, IKey::BACKSPACE);
			MAP_KEY(SDLK_PAUSE, IKey::PAUSE);
			MAP_KEY(SDLK_ESCAPE, IKey::ESCAPE);
			MAP_KEY(SDLK_DELETE, IKey::DELETE_KEY);
			MAP_KEY(SDLK_INSERT, IKey::INSERT);
			MAP_KEY(SDLK_HOME, IKey::HOME);
			MAP_KEY(SDLK_END, IKey::END);
			MAP_KEY(SDLK_PAGEUP, IKey::PAGE_UP);
			MAP_KEY(SDLK_PRINT, IKey::PRINT_SCREEN);
			MAP_KEY(SDLK_PAGEDOWN, IKey::PAGE_DOWN);
			MAP_KEY(SDLK_F1, IKey::F1);
			MAP_KEY(SDLK_F2, IKey::F2);
			MAP_KEY(SDLK_F3, IKey::F3);
			MAP_KEY(SDLK_F4, IKey::F4);
			MAP_KEY(SDLK_F5, IKey::F5);
			MAP_KEY(SDLK_F6, IKey::F6);
			MAP_KEY(SDLK_F7, IKey::F7);
			MAP_KEY(SDLK_F8, IKey::F8);
			MAP_KEY(SDLK_F9, IKey::F9);
			MAP_KEY(SDLK_F10, IKey::F10);
			MAP_KEY(SDLK_F11, IKey::F11);
			MAP_KEY(SDLK_F12, IKey::F12);
			MAP_KEY(SDLK_F13, IKey::F13);
			MAP_KEY(SDLK_F14, IKey::F14);
			MAP_KEY(SDLK_F15, IKey::F15);
			MAP_KEY(SDLK_NUMLOCK, IKey::NUM_LOCK);
			MAP_KEY(SDLK_CAPSLOCK, IKey::CAPS_LOCK);
			MAP_KEY(SDLK_SCROLLOCK, IKey::SCROLL_LOCK);
			MAP_KEY(SDLK_RMETA, IKey::RIGHT_META);
			MAP_KEY(SDLK_LMETA, IKey::LEFT_META);
			MAP_KEY(SDLK_LSUPER, IKey::LEFT_SUPER);
			MAP_KEY(SDLK_RSUPER, IKey::RIGHT_SUPER);
			MAP_KEY(SDLK_MODE, IKey::ALT_GR);
			MAP_KEY(SDLK_UP, IKey::UP);
			MAP_KEY(SDLK_DOWN, IKey::DOWN);
			MAP_KEY(SDLK_LEFT, IKey::LEFT);
			MAP_KEY(SDLK_RIGHT, IKey::RIGHT);
			MAP_KEY(SDLK_RETURN, IKey::ENTER);
			MAP_KEY(SDLK_KP_ENTER, IKey::ENTER);
		case SDLK_SPACE:
			// Special characters like ~ (tilde) ends up with the keysym.sym SDLK_SPACE which
			// without this check would be lost. The check is only valid on key down events in SDL.
			if (sdlevt.type == SDL_KEYUP || keysym.unicode == ' ') {
				value = IKey::SPACE;
			}
			break;
		default:
			break;
		}

		if (!(keysym.mod & KMOD_NUM)) {
			switch (keysym.sym) {
			MAP_KEY(SDLK_KP0, IKey::INSERT);
			MAP_KEY(SDLK_KP1, IKey::END);
			MAP_KEY(SDLK_KP2, IKey::DOWN);
			MAP_KEY(SDLK_KP3, IKey::PAGE_DOWN);
			MAP_KEY(SDLK_KP4, IKey::LEFT);
			MAP_KEY(SDLK_KP5, IKey::INVALID_KEY);
			MAP_KEY(SDLK_KP6, IKey::RIGHT);
			MAP_KEY(SDLK_KP7, IKey::HOME);
			MAP_KEY(SDLK_KP8, IKey::UP);
			MAP_KEY(SDLK_KP9, IKey::PAGE_UP);
			default:
				break;
			}
		}

		keyevt.setKey(Key(value, repr));
	}

	template<typename T>
	void removeListener(std::vector<T>& vec, T& listener) {
		typename std::vector<T>::iterator i = vec.begin();
		while (i != vec.end()) {
			if ((*i) == listener) {
				vec.erase(i);
				return;
			}
			++i;
		}
	}

	template<typename T>
	void addListener(std::vector<T>& vec, T& listener) {
		vec.push_back(listener);
	}

	void EventManager::addCommandListener(ICommandListener* listener) {
		addListener<ICommandListener*>(m_commandlisteners, listener);
	}

	void EventManager::removeCommandListener(ICommandListener* listener) {
		removeListener<ICommandListener*>(m_commandlisteners, listener);
	}

	void EventManager::addKeyListener(IKeyListener* listener) {
		addListener<IKeyListener*>(m_keylisteners, listener);
	}

	void EventManager::removeKeyListener(IKeyListener* listener) {
		removeListener<IKeyListener*>(m_keylisteners, listener);
	}

	void EventManager::addMouseListener(IMouseListener* listener) {
		addListener<IMouseListener*>(m_mouselisteners, listener);
	}

	void EventManager::removeMouseListener(IMouseListener* listener) {
		removeListener<IMouseListener*>(m_mouselisteners, listener);
	}

	void EventManager::addSdlEventListener(ISdlEventListener* listener) {
		addListener<ISdlEventListener*>(m_sdleventlisteners, listener);
	}

	void EventManager::removeSdlEventListener(ISdlEventListener* listener) {
		removeListener<ISdlEventListener*>(m_sdleventlisteners, listener);
	}

	void EventManager::addWidgetListener(IWidgetListener* listener) {
		addListener<IWidgetListener*>(m_widgetlisteners, listener);
	}

	void EventManager::removeWidgetListener(IWidgetListener* listener) {
		removeListener<IWidgetListener*>(m_widgetlisteners, listener);
	}

	void EventManager::dispatchCommand(ICommand& command) {
		std::vector<ICommandListener*>::iterator i = m_commandlisteners.begin();
		while (i != m_commandlisteners.end()) {
			(*i)->onCommand(command);
			if (command.isConsumed()) {
				break;
			}
			++i;
		}
	}

	void EventManager::dispatchKeyEvent(IKeyEvent& evt) {
		bool nonconsumablekey = false;
		for (std::vector<int>::iterator it = m_nonconsumablekeys.begin(); 
		     it != m_nonconsumablekeys.end(); ++it) {
			if (*it == evt.getKey().getValue()) {
				nonconsumablekey = true;
				break;
			}
		}

		std::vector<IKeyListener*>::iterator i = m_keylisteners.begin();
		while (i != m_keylisteners.end()) {
			switch (evt.getType()) {
				case IKeyEvent::PRESSED:
					(*i)->keyPressed(evt);
					break;
				case IKeyEvent::RELEASED:
					(*i)->keyReleased(evt);
					break;
				default:
					break;
			}
			if ((!nonconsumablekey) && (evt.isConsumed())) {
				break;
			}
			++i;
		}
	}

	void EventManager::fillModifiers(InputEvent& evt) {
		evt.setAltPressed(m_keystatemap[IKey::ALT_GR] |
						m_keystatemap[IKey::LEFT_ALT] |
						m_keystatemap[IKey::RIGHT_ALT]);
		evt.setControlPressed(m_keystatemap[IKey::LEFT_CONTROL] |
							m_keystatemap[IKey::RIGHT_CONTROL]);
		evt.setMetaPressed(m_keystatemap[IKey::LEFT_META] |
							m_keystatemap[IKey::RIGHT_META]);
		evt.setShiftPressed(m_keystatemap[IKey::LEFT_SHIFT] |
							m_keystatemap[IKey::RIGHT_SHIFT]);
	}

	void EventManager::dispatchMouseEvent(IMouseEvent& evt) {
		std::vector<IMouseListener*>::iterator i = m_mouselisteners.begin();
		while (i != m_mouselisteners.end()) {
			switch (evt.getType()) {
				case IMouseEvent::MOVED:
					(*i)->mouseMoved(evt);
					break;
				case IMouseEvent::PRESSED:
					(*i)->mousePressed(evt);
					break;
				case IMouseEvent::RELEASED:
					(*i)->mouseReleased(evt);
					break;
				case IMouseEvent::WHEEL_MOVED_DOWN:
					(*i)->mouseWheelMovedDown(evt);
					break;
				case IMouseEvent::WHEEL_MOVED_UP:
					(*i)->mouseWheelMovedUp(evt);
					break;
				case IMouseEvent::CLICKED:
					(*i)->mouseClicked(evt);
					break;
				case IMouseEvent::ENTERED:
					(*i)->mouseEntered(evt);
				case IMouseEvent::EXITED:
					(*i)->mouseExited(evt);
					break;
				case IMouseEvent::DRAGGED:
					(*i)->mouseDragged(evt);
					break;
				default:
					break;
			}
			if (evt.isConsumed()) {
				break;
			}
			++i;
		}
	}

	void EventManager::dispatchSdlEvent(SDL_Event& evt) {
		std::vector<ISdlEventListener*>::iterator i = m_sdleventlisteners.begin();
		while (i != m_sdleventlisteners.end()) {
			(*i)->onSdlEvent(evt);
			++i;
		}
	}

	void EventManager::dispatchWidgetEvent(IWidgetEvent& evt) {
		std::vector<IWidgetListener*>::iterator i = m_widgetlisteners.begin();
		while (i != m_widgetlisteners.end()) {
			(*i)->onWidgetAction(evt);
			if (evt.isConsumed()) {
				break;
			}
			++i;
		}	
	}

	void EventManager::onWidgetAction(IWidgetEvent& evt) {
		dispatchWidgetEvent(evt);
	}

	void EventManager::processEvents(){
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					Command cmd;
					cmd.setSource(this);
					cmd.setCommandType(CMD_QUIT_GAME);
					dispatchCommand(cmd);
					break;
					}
				case SDL_ACTIVEEVENT:
					break;
				case SDL_KEYDOWN:
				case SDL_KEYUP: {
					KeyEvent keyevt;
					keyevt.setSource(this);
					fillKeyEvent(event, keyevt);
					m_keystatemap[keyevt.getKey().getValue()] = (keyevt.getType() == IKeyEvent::PRESSED);
					dispatchKeyEvent(keyevt);
					}
					break;
				case SDL_MOUSEMOTION:
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN: {
					MouseEvent mouseevt;
					mouseevt.setSource(this);
					fillMouseEvent(event, mouseevt);
					fillModifiers(mouseevt);
					if (event.type == SDL_MOUSEBUTTONDOWN) {
						m_mousestate |= static_cast<int>(mouseevt.getButton());
						m_mostrecentbtn = mouseevt.getButton();
					} else if (event.type == SDL_MOUSEBUTTONUP) {
						m_mousestate &= ~static_cast<int>(mouseevt.getButton());
					}
					dispatchMouseEvent(mouseevt);
					break;
					}
			}
			dispatchSdlEvent(event);
		}
	}
	
	EventSourceType EventManager::getEventSourceType() {
		return ES_ENGINE;
	}

	void EventManager::setNonConsumableKeys(const std::vector<int>& keys) {
		m_nonconsumablekeys = keys;
	}

	std::vector<int> EventManager::getNonConsumableKeys() {
		return m_nonconsumablekeys;
	}
}
