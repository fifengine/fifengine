/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
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
#include "eventchannel/trigger/ec_trigger.h"

#include "eventmanager.h"

namespace FIFE {

	EventManager::EventManager():
		m_commandlisteners(),
		m_keylisteners(),
		m_mouselisteners(),
		m_sdleventlisteners(),
		m_widgetlisteners(),
		m_pending_widgetlisteners(),
		m_nonconsumablekeys(),
		m_keystatemap(),
		m_mousestate(0),
		m_mostrecentbtn(MouseEvent::EMPTY)
 	{
	}

	EventManager::~EventManager() {
	}

	void EventManager::fillMouseEvent(const SDL_Event& sdlevt, MouseEvent& mouseevt) {
		mouseevt.setX(sdlevt.button.x);
		mouseevt.setY(sdlevt.button.y);
		mouseevt.setButton(MouseEvent::EMPTY);
		mouseevt.setType(MouseEvent::MOVED);
		if ((sdlevt.type == SDL_MOUSEBUTTONUP) || (sdlevt.type == SDL_MOUSEBUTTONDOWN)) {
			switch (sdlevt.button.button) {
				case SDL_BUTTON_LEFT:
					mouseevt.setButton(MouseEvent::LEFT);
					break;
				case SDL_BUTTON_RIGHT:
					mouseevt.setButton(MouseEvent::RIGHT);
					break;
				case SDL_BUTTON_MIDDLE:
					mouseevt.setButton(MouseEvent::MIDDLE);
					break;
				default:
					mouseevt.setButton(MouseEvent::UNKNOWN_BUTTON);
					break;
			}


			if (sdlevt.type == SDL_MOUSEBUTTONUP ) {

				mouseevt.setType(MouseEvent::RELEASED);

			} else {
				mouseevt.setType(MouseEvent::PRESSED);
			}

			switch (sdlevt.button.button) {
				case SDL_BUTTON_WHEELDOWN:
					mouseevt.setType(MouseEvent::WHEEL_MOVED_DOWN);
					break;
				case SDL_BUTTON_WHEELUP:
					mouseevt.setType(MouseEvent::WHEEL_MOVED_UP);
					break;
				default:
					break;
			}
		}
		if ((mouseevt.getType() == MouseEvent::MOVED) && m_mousestate) {
			mouseevt.setType(MouseEvent::DRAGGED);
			mouseevt.setButton(m_mostrecentbtn);
		}
	}

	void EventManager::fillKeyEvent(const SDL_Event& sdlevt, KeyEvent& keyevt) {
		if (sdlevt.type == SDL_KEYDOWN) {
			keyevt.setType(KeyEvent::PRESSED);
		} else if (sdlevt.type == SDL_KEYUP) {
			keyevt.setType(KeyEvent::RELEASED);
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
		Key::KeyType value = Key::INVALID_KEY;
		std::string repr("");

		if (keysym.unicode < 255) {
			value = static_cast<Key::KeyType>(keysym.unicode);
			repr.push_back(value);
		}

		#define MAP_KEY(_orig, _new) case _orig: value = _new; repr = #_new; break;
		switch (keysym.sym) {
			MAP_KEY(SDLK_TAB, Key::TAB);
			MAP_KEY(SDLK_LALT, Key::LEFT_ALT);
			MAP_KEY(SDLK_RALT, Key::RIGHT_ALT);
			MAP_KEY(SDLK_LSHIFT, Key::LEFT_SHIFT);
			MAP_KEY(SDLK_RSHIFT, Key::RIGHT_SHIFT);
			MAP_KEY(SDLK_LCTRL, Key::LEFT_CONTROL);
			MAP_KEY(SDLK_RCTRL, Key::RIGHT_CONTROL);
			MAP_KEY(SDLK_BACKSPACE, Key::BACKSPACE);
			MAP_KEY(SDLK_PAUSE, Key::PAUSE);
			MAP_KEY(SDLK_ESCAPE, Key::ESCAPE);
			MAP_KEY(SDLK_DELETE, Key::DELETE_KEY);
			MAP_KEY(SDLK_INSERT, Key::INSERT);
			MAP_KEY(SDLK_HOME, Key::HOME);
			MAP_KEY(SDLK_END, Key::END);
			MAP_KEY(SDLK_PAGEUP, Key::PAGE_UP);
			MAP_KEY(SDLK_PRINT, Key::PRINT_SCREEN);
			MAP_KEY(SDLK_PAGEDOWN, Key::PAGE_DOWN);
			MAP_KEY(SDLK_F1, Key::F1);
			MAP_KEY(SDLK_F2, Key::F2);
			MAP_KEY(SDLK_F3, Key::F3);
			MAP_KEY(SDLK_F4, Key::F4);
			MAP_KEY(SDLK_F5, Key::F5);
			MAP_KEY(SDLK_F6, Key::F6);
			MAP_KEY(SDLK_F7, Key::F7);
			MAP_KEY(SDLK_F8, Key::F8);
			MAP_KEY(SDLK_F9, Key::F9);
			MAP_KEY(SDLK_F10, Key::F10);
			MAP_KEY(SDLK_F11, Key::F11);
			MAP_KEY(SDLK_F12, Key::F12);
			MAP_KEY(SDLK_F13, Key::F13);
			MAP_KEY(SDLK_F14, Key::F14);
			MAP_KEY(SDLK_F15, Key::F15);
			MAP_KEY(SDLK_NUMLOCK, Key::NUM_LOCK);
			MAP_KEY(SDLK_CAPSLOCK, Key::CAPS_LOCK);
			MAP_KEY(SDLK_SCROLLOCK, Key::SCROLL_LOCK);
			MAP_KEY(SDLK_RMETA, Key::RIGHT_META);
			MAP_KEY(SDLK_LMETA, Key::LEFT_META);
			MAP_KEY(SDLK_LSUPER, Key::LEFT_SUPER);
			MAP_KEY(SDLK_RSUPER, Key::RIGHT_SUPER);
			MAP_KEY(SDLK_MODE, Key::ALT_GR);
			MAP_KEY(SDLK_UP, Key::UP);
			MAP_KEY(SDLK_DOWN, Key::DOWN);
			MAP_KEY(SDLK_LEFT, Key::LEFT);
			MAP_KEY(SDLK_RIGHT, Key::RIGHT);
			MAP_KEY(SDLK_RETURN, Key::ENTER);
			MAP_KEY(SDLK_KP_ENTER, Key::ENTER);
		case SDLK_SPACE:
			// Special characters like ~ (tilde) ends up with the keysym.sym SDLK_SPACE which
			// without this check would be lost. The check is only valid on key down events in SDL.
			if (sdlevt.type == SDL_KEYUP || keysym.unicode == ' ') {
				value = Key::SPACE;
			}
			break;
		default:
			break;
		}

		if (!(keysym.mod & KMOD_NUM)) {
			switch (keysym.sym) {
			MAP_KEY(SDLK_KP0, Key::INSERT);
			MAP_KEY(SDLK_KP1, Key::END);
			MAP_KEY(SDLK_KP2, Key::DOWN);
			MAP_KEY(SDLK_KP3, Key::PAGE_DOWN);
			MAP_KEY(SDLK_KP4, Key::LEFT);
			MAP_KEY(SDLK_KP5, Key::INVALID_KEY);
			MAP_KEY(SDLK_KP6, Key::RIGHT);
			MAP_KEY(SDLK_KP7, Key::HOME);
			MAP_KEY(SDLK_KP8, Key::UP);
			MAP_KEY(SDLK_KP9, Key::PAGE_UP);
			default:
				break;
			}
		}

		keyevt.setKey(Key(value, repr));
	}

	template<typename T>
	void removeListener(std::vector<T>& vec, T& listener) {
		vec.push_back(listener);
	}

	template<typename T>
	void addListener(std::vector<T>& vec, T& listener) {
		vec.push_back(listener);
	}

	void EventManager::addCommandListener(ICommandListener* listener) {
		addListener<ICommandListener*>(m_pending_commandlisteners, listener);
	}

	void EventManager::removeCommandListener(ICommandListener* listener) {
		removeListener<ICommandListener*>(m_pending_cldeletions, listener);
	}

	void EventManager::addKeyListener(IKeyListener* listener) {
		addListener<IKeyListener*>(m_pending_keylisteners, listener);
	}

	void EventManager::removeKeyListener(IKeyListener* listener) {
		removeListener<IKeyListener*>(m_pending_kldeletions, listener);
	}

	void EventManager::addMouseListener(IMouseListener* listener) {
		addListener<IMouseListener*>(m_pending_mouselisteners, listener);
	}

	void EventManager::removeMouseListener(IMouseListener* listener) {
		removeListener<IMouseListener*>(m_pending_mldeletions, listener);
	}

	void EventManager::addSdlEventListener(ISdlEventListener* listener) {
		addListener<ISdlEventListener*>(m_pending_sdleventlisteners, listener);
	}

	void EventManager::removeSdlEventListener(ISdlEventListener* listener) {
		removeListener<ISdlEventListener*>(m_pending_sdldeletions, listener);
	}

	void EventManager::addWidgetListener(IWidgetListener* listener) {
		addListener<IWidgetListener*>(m_pending_widgetlisteners, listener);
	}

	void EventManager::removeWidgetListener(IWidgetListener* listener) {
		removeListener<IWidgetListener*>(m_pending_wldeletions, listener);
	}

	void EventManager::dispatchCommand(Command& command) {
		if (!m_pending_cldeletions.empty()) {
			std::vector<ICommandListener*>::iterator i = m_pending_cldeletions.begin();
			while (i != m_pending_cldeletions.end()) {
				std::vector<ICommandListener*>::iterator j = m_commandlisteners.begin();
				while (j != m_commandlisteners.end()) {
					if(*j == *i) {
						m_commandlisteners.erase(j);
						break;
					}
					++j;
				}
				++i;
			}
			m_pending_cldeletions.clear();
		}

		if(!m_pending_commandlisteners.empty()) {
			std::vector<ICommandListener*>::iterator i = m_pending_commandlisteners.begin();
			while (i != m_pending_commandlisteners.end()) {
				m_commandlisteners.push_back(*i);
				++i;
			}
			m_pending_commandlisteners.clear();
		}

		std::vector<ICommandListener*>::iterator i = m_commandlisteners.begin();
		while (i != m_commandlisteners.end()) {
			(*i)->onCommand(command);
			if (command.isConsumed()) {
				break;
			}
			++i;
		}
	}

	void EventManager::dispatchKeyEvent(KeyEvent& evt) {
		bool nonconsumablekey = false;
		for (std::vector<int>::iterator it = m_nonconsumablekeys.begin();
		     it != m_nonconsumablekeys.end(); ++it) {
			if (*it == evt.getKey().getValue()) {
				nonconsumablekey = true;
				break;
			}
		}

		if (!m_pending_kldeletions.empty()) {
			std::vector<IKeyListener*>::iterator i = m_pending_kldeletions.begin();
			while (i != m_pending_kldeletions.end()) {
				std::vector<IKeyListener*>::iterator j = m_keylisteners.begin();
				while (j != m_keylisteners.end()) {
					if(*j == *i) {
						m_keylisteners.erase(j);
						break;
					}
					++j;
				}
				++i;
			}
			m_pending_kldeletions.clear();
		}

		if(!m_pending_keylisteners.empty()) {
			std::vector<IKeyListener*>::iterator i = m_pending_keylisteners.begin();
			while (i != m_pending_keylisteners.end()) {
				m_keylisteners.push_back(*i);
				++i;
			}
			m_pending_keylisteners.clear();
		}

		std::vector<IKeyListener*>::iterator i = m_keylisteners.begin();
		while (i != m_keylisteners.end()) {
			switch (evt.getType()) {
				case KeyEvent::PRESSED:
					(*i)->keyPressed(evt);
					break;
				case KeyEvent::RELEASED:
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
		evt.setAltPressed(m_keystatemap[Key::ALT_GR] |
						m_keystatemap[Key::LEFT_ALT] |
						m_keystatemap[Key::RIGHT_ALT]);
		evt.setControlPressed(m_keystatemap[Key::LEFT_CONTROL] |
							m_keystatemap[Key::RIGHT_CONTROL]);
		evt.setMetaPressed(m_keystatemap[Key::LEFT_META] |
							m_keystatemap[Key::RIGHT_META]);
		evt.setShiftPressed(m_keystatemap[Key::LEFT_SHIFT] |
							m_keystatemap[Key::RIGHT_SHIFT]);
	}

	void EventManager::dispatchMouseEvent(MouseEvent& evt) {
		if (!m_pending_mldeletions.empty()) {
			std::vector<IMouseListener*>::iterator i = m_pending_mldeletions.begin();
			while (i != m_pending_mldeletions.end()) {
				std::vector<IMouseListener*>::iterator j = m_mouselisteners.begin();
				while (j != m_mouselisteners.end()) {
					if(*j == *i) {
						m_mouselisteners.erase(j);
						break;
					}
					++j;
				}
				++i;
			}
			m_pending_mldeletions.clear();
		}

		if(!m_pending_mouselisteners.empty()) {
			std::vector<IMouseListener*>::iterator i = m_pending_mouselisteners.begin();
			while (i != m_pending_mouselisteners.end()) {
				m_mouselisteners.push_back(*i);
				++i;
			}
			m_pending_mouselisteners.clear();
		}

		std::vector<IMouseListener*>::iterator i = m_mouselisteners.begin();
		while (i != m_mouselisteners.end()) {
			switch (evt.getType()) {
				case MouseEvent::MOVED:
					(*i)->mouseMoved(evt);
					break;
				case MouseEvent::PRESSED:
					(*i)->mousePressed(evt);
					break;
				case MouseEvent::RELEASED:
					(*i)->mouseReleased(evt);
					break;
				case MouseEvent::WHEEL_MOVED_DOWN:
					(*i)->mouseWheelMovedDown(evt);
					break;
				case MouseEvent::WHEEL_MOVED_UP:
					(*i)->mouseWheelMovedUp(evt);
					break;
				case MouseEvent::CLICKED:
					(*i)->mouseClicked(evt);
					break;
				case MouseEvent::ENTERED:
					(*i)->mouseEntered(evt);
				case MouseEvent::EXITED:
					(*i)->mouseExited(evt);
					break;
				case MouseEvent::DRAGGED:
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
		if (!m_pending_sdldeletions.empty()) {
			std::vector<ISdlEventListener*>::iterator i = m_pending_sdldeletions.begin();
			while (i != m_pending_sdldeletions.end()) {
				std::vector<ISdlEventListener*>::iterator j = m_sdleventlisteners.begin();
				while (j != m_sdleventlisteners.end()) {
					if(*j == *i) {
						m_sdleventlisteners.erase(j);
						break;
					}
					++j;
				}
				++i;
			}
			m_pending_sdldeletions.clear();
		}

		if (!m_pending_sdleventlisteners.empty()) {
			std::vector<ISdlEventListener*>::iterator i = m_pending_sdleventlisteners.begin();
			while(i != m_pending_sdleventlisteners.end()) {
				m_sdleventlisteners.push_back(*i);
				++i;
			}
			m_pending_sdleventlisteners.clear();
		}

		std::vector<ISdlEventListener*>::iterator i = m_sdleventlisteners.begin();
		while (i != m_sdleventlisteners.end()) {
			(*i)->onSdlEvent(evt);
			++i;
		}
	}

	void EventManager::dispatchWidgetEvent(WidgetEvent& evt) {
		if (!m_pending_wldeletions.empty()) {
			std::vector<IWidgetListener*>::iterator i = m_pending_wldeletions.begin();
			while (i != m_pending_wldeletions.end()) {
				std::vector<IWidgetListener*>::iterator j = m_widgetlisteners.begin();
				while (j != m_widgetlisteners.end()) {
					if(*j == *i) {
						m_widgetlisteners.erase(j);
						break;
					}
					++j;
				}
				++i;
			}
			m_pending_wldeletions.clear();
		}

		if(!m_pending_widgetlisteners.empty()) {
			std::vector<IWidgetListener*>::iterator i = m_pending_widgetlisteners.begin();
			while (i != m_pending_widgetlisteners.end()) {
				m_widgetlisteners.push_back(*i);
				++i;
			}
			m_pending_widgetlisteners.clear();
		}

		std::vector<IWidgetListener*>::iterator i = m_widgetlisteners.begin();
		while (i != m_widgetlisteners.end()) {
			(*i)->onWidgetAction(evt);
			if (evt.isConsumed()) {
				break;
			}
			++i;
		}
	}

	void EventManager::onWidgetAction(WidgetEvent& evt) {
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
					m_keystatemap[keyevt.getKey().getValue()] = (keyevt.getType() == KeyEvent::PRESSED);
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

		pollTriggers();
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

	void EventManager::registerTrigger(Trigger& trigger){
		m_triggers.push_back(&trigger);
	}

	void EventManager::unregisterTrigger(Trigger& trigger){
		m_triggers.remove(&trigger);
	}

	void EventManager::pollTriggers(){
		for (std::list<Trigger*>::iterator it = m_triggers.begin(); it!=m_triggers.end(); ++it) {
			(*it)->pollTrigger();
		}
	}
}
