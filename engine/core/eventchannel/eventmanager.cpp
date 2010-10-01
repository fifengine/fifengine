/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "eventchannel/key/ec_key.h"
#include "eventchannel/key/ec_keyevent.h"
#include "eventchannel/key/ec_ikeyfilter.h"
#include "eventchannel/mouse/ec_mouseevent.h"
#include "eventchannel/command/ec_command.h"

#include "eventmanager.h"

namespace FIFE {

	EventManager::EventManager():
		m_commandlisteners(),
		m_keylisteners(),
		m_mouselisteners(),
		m_sdleventlisteners(),
		m_keystatemap(),
		m_keyfilter(0),
		m_mousestate(0),
		m_mostrecentbtn(MouseEvent::EMPTY)
 	{
	}

	EventManager::~EventManager() {
	}

	template<typename T>
	void removeListener(std::deque<T>& vec, T& listener) {
		vec.push_back(listener);
	}

	template<typename T>
	void addListener(std::deque<T>& vec, T& listener) {
		vec.push_back(listener);
	}

	void EventManager::addCommandListener(ICommandListener* listener) {
		addListener<ICommandListener*>(m_pending_commandlisteners, listener);
	}

	void EventManager::addCommandListenerFront(ICommandListener* listener) {
		addListener<ICommandListener*>(m_pending_commandlisteners, listener);
	}

	void EventManager::removeCommandListener(ICommandListener* listener) {
		removeListener<ICommandListener*>(m_pending_cldeletions, listener);
	}

	void EventManager::addKeyListener(IKeyListener* listener) {
		addListener<IKeyListener*>(m_pending_keylisteners, listener);
	}

	void EventManager::addKeyListenerFront(IKeyListener* listener) {
		addListener<IKeyListener*>(m_pending_keylisteners_front, listener);
	}

	void EventManager::removeKeyListener(IKeyListener* listener) {
		removeListener<IKeyListener*>(m_pending_kldeletions, listener);
	}

	void EventManager::addMouseListener(IMouseListener* listener) {
		addListener<IMouseListener*>(m_pending_mouselisteners, listener);
	}

	void EventManager::addMouseListenerFront(IMouseListener* listener) {
		addListener<IMouseListener*>(m_pending_mouselisteners, listener);
	}

	void EventManager::removeMouseListener(IMouseListener* listener) {
		removeListener<IMouseListener*>(m_pending_mldeletions, listener);
	}

	void EventManager::addSdlEventListener(ISdlEventListener* listener) {
		addListener<ISdlEventListener*>(m_pending_sdleventlisteners, listener);
	}

	void EventManager::addSdlEventListenerFront(ISdlEventListener* listener) {
		addListener<ISdlEventListener*>(m_pending_sdleventlisteners, listener);
	}

	void EventManager::removeSdlEventListener(ISdlEventListener* listener) {
		removeListener<ISdlEventListener*>(m_pending_sdldeletions, listener);
	}

	void EventManager::dispatchCommand(Command& command) {
		if(!m_pending_commandlisteners.empty()) {
			std::deque<ICommandListener*>::iterator i = m_pending_commandlisteners.begin();
			while (i != m_pending_commandlisteners.end()) {
				m_commandlisteners.push_back(*i);
				++i;
			}
			m_pending_commandlisteners.clear();
		}

		if(!m_pending_commandlisteners_front.empty()) {
			std::deque<ICommandListener*>::iterator i = m_pending_commandlisteners_front.begin();
			while (i != m_pending_commandlisteners_front.end()) {
				m_commandlisteners.push_front(*i);
				++i;
			}
			m_pending_commandlisteners_front.clear();
		}

		if (!m_pending_cldeletions.empty()) {
			std::deque<ICommandListener*>::iterator i = m_pending_cldeletions.begin();
			while (i != m_pending_cldeletions.end()) {
				std::deque<ICommandListener*>::iterator j = m_commandlisteners.begin();
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

		std::deque<ICommandListener*>::iterator i = m_commandlisteners.begin();
		while (i != m_commandlisteners.end()) {
			(*i)->onCommand(command);
			if (command.isConsumed()) {
				break;
			}
			++i;
		}
	}

	void EventManager::dispatchKeyEvent(KeyEvent& evt) {
		if(!m_pending_keylisteners.empty()) {
			std::deque<IKeyListener*>::iterator i = m_pending_keylisteners.begin();
			while (i != m_pending_keylisteners.end()) {
				m_keylisteners.push_back(*i);
				++i;
			}
			m_pending_keylisteners.clear();
		}

		if(!m_pending_keylisteners_front.empty()) {
			std::deque<IKeyListener*>::iterator i = m_pending_keylisteners_front.begin();
			while (i != m_pending_keylisteners_front.end()) {
				m_keylisteners.push_front(*i);
				++i;
			}
			m_pending_keylisteners_front.clear();
		}

		if (!m_pending_kldeletions.empty()) {
			std::deque<IKeyListener*>::iterator i = m_pending_kldeletions.begin();
			while (i != m_pending_kldeletions.end()) {
				std::deque<IKeyListener*>::iterator j = m_keylisteners.begin();
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

		std::deque<IKeyListener*>::iterator i = m_keylisteners.begin();
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
			++i;
		}
	}

	void EventManager::dispatchMouseEvent(MouseEvent& evt) {
		if(!m_pending_mouselisteners.empty()) {
			std::deque<IMouseListener*>::iterator i = m_pending_mouselisteners.begin();
			while (i != m_pending_mouselisteners.end()) {
				m_mouselisteners.push_back(*i);
				++i;
			}
			m_pending_mouselisteners.clear();
		}

		if(!m_pending_mouselisteners_front.empty()) {
			std::deque<IMouseListener*>::iterator i = m_pending_mouselisteners_front.begin();
			while (i != m_pending_mouselisteners_front.end()) {
				m_mouselisteners.push_front(*i);
				++i;
			}
			m_pending_mouselisteners_front.clear();
		}

		if (!m_pending_mldeletions.empty()) {
			std::deque<IMouseListener*>::iterator i = m_pending_mldeletions.begin();
			while (i != m_pending_mldeletions.end()) {
				std::deque<IMouseListener*>::iterator j = m_mouselisteners.begin();
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

		std::deque<IMouseListener*>::iterator i = m_mouselisteners.begin();
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
					break;
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

	bool EventManager::dispatchSdlEvent(SDL_Event& evt) {
		bool ret = false;
		if (!m_pending_sdleventlisteners.empty()) {
			std::deque<ISdlEventListener*>::iterator i = m_pending_sdleventlisteners.begin();
			while(i != m_pending_sdleventlisteners.end()) {
				m_sdleventlisteners.push_back(*i);
				++i;
			}
			m_pending_sdleventlisteners.clear();
		}

		if (!m_pending_sdleventlisteners_front.empty()) {
			std::deque<ISdlEventListener*>::iterator i = m_pending_sdleventlisteners_front.begin();
			while(i != m_pending_sdleventlisteners_front.end()) {
				m_sdleventlisteners.push_front(*i);
				++i;
			}
			m_pending_sdleventlisteners_front.clear();
		}

		if (!m_pending_sdldeletions.empty()) {
			std::deque<ISdlEventListener*>::iterator i = m_pending_sdldeletions.begin();
			while (i != m_pending_sdldeletions.end()) {
				std::deque<ISdlEventListener*>::iterator j = m_sdleventlisteners.begin();
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

		std::deque<ISdlEventListener*>::iterator i = m_sdleventlisteners.begin();
		while (i != m_sdleventlisteners.end()) {
			ret = ret || (*i)->onSdlEvent(evt);
			++i;
		}
		return ret;
	}

	bool EventManager::combineEvents(SDL_Event& event1, const SDL_Event& event2) {
		if(event1.type == event2.type) {
			switch (event1.type) {
				case SDL_MOUSEMOTION:
					if(event1.motion.state == event2.motion.state) {
						event1.motion.x = event2.motion.x;
						event1.motion.y = event2.motion.y;
						event1.motion.xrel += event2.motion.xrel;
						event1.motion.yrel += event2.motion.yrel;
						return true;
					}
					return false;
			}
		}
		return false;
	}

	void EventManager::processEvents() {
		// The double SDL_PollEvent calls don't throw away events,
		// but try to combine (mouse motion) events.
		SDL_Event event, next_event;
		bool has_next_event = SDL_PollEvent(&event);
		while (has_next_event) {
			has_next_event = SDL_PollEvent(&next_event);
			if(has_next_event && combineEvents(event, next_event))
				continue;

			switch (event.type) {
				case SDL_QUIT: {
					Command cmd;
					cmd.setSource(this);
					cmd.setCommandType(CMD_QUIT_GAME);
					dispatchCommand(cmd);
					}
					break;

				case SDL_ACTIVEEVENT:
					processActiveEvent(event);
					break;

				case SDL_KEYDOWN:
				case SDL_KEYUP:
					processKeyEvent(event);
					break;

				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEMOTION:
				case SDL_MOUSEBUTTONDOWN:
					processMouseEvent(event);
					break;
			}
			if(has_next_event)
				event = next_event;
		}
	}

	void EventManager::processActiveEvent(SDL_Event event) {
		if(dispatchSdlEvent(event))
			return;

		Command cmd;
		cmd.setSource(this);
		SDL_ActiveEvent actevt = event.active;
		if (actevt.state == SDL_APPMOUSEFOCUS)
		{
			if (actevt.gain)
				cmd.setCommandType(CMD_MOUSE_FOCUS_GAINED);
			else
				cmd.setCommandType(CMD_MOUSE_FOCUS_LOST);
		}
		else if (actevt.state == SDL_APPINPUTFOCUS)
		{
			if (actevt.gain)
				cmd.setCommandType(CMD_INPUT_FOCUS_GAINED);
			else
				cmd.setCommandType(CMD_INPUT_FOCUS_LOST);
		}
		else if (actevt.state == SDL_APPACTIVE)
		{
			if (actevt.gain)
				cmd.setCommandType(CMD_APP_RESTORED);
			else
				cmd.setCommandType(CMD_APP_ICONIFIED);
		}
		dispatchCommand(cmd);
	}

	void EventManager::processKeyEvent(SDL_Event event) {
		KeyEvent keyevt;
		keyevt.setSource(this);
		fillKeyEvent(event, keyevt);
		m_keystatemap[keyevt.getKey().getValue()] = (keyevt.getType() == KeyEvent::PRESSED);

		bool dispatchAsSdl = !keyevt.getKey().isFunctionKey();
		if( dispatchAsSdl && m_keyfilter ) {
			dispatchAsSdl = !m_keyfilter->isFiltered(keyevt);
		}

		if( dispatchAsSdl ) {
			if( dispatchSdlEvent(event) )
				return;
		}

		dispatchKeyEvent(keyevt);
	}

	void EventManager::processMouseEvent(SDL_Event event) {
		if(dispatchSdlEvent(event))
			return;

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
		// fire scrollwheel events only once
		if (event.button.button == SDL_BUTTON_WHEELDOWN || event.button.button == SDL_BUTTON_WHEELUP) {
			if (event.type == SDL_MOUSEBUTTONUP) {
				return;
			}
		}
		dispatchMouseEvent(mouseevt);
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
		SDL_keysym keysym = sdlevt.key.keysym;

		keyevt.setShiftPressed(keysym.mod & KMOD_SHIFT);
		keyevt.setControlPressed(keysym.mod & KMOD_CTRL);
		keyevt.setAltPressed(keysym.mod & KMOD_ALT);
		keyevt.setMetaPressed(keysym.mod & KMOD_META);
		keyevt.setNumericPad(keysym.sym >= SDLK_KP0 && keysym.sym <= SDLK_KP_EQUALS);
		keyevt.setKey(Key(static_cast<Key::KeyType>(keysym.sym), keysym.unicode));
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

	EventSourceType EventManager::getEventSourceType() {
		return ES_ENGINE;
	}

	void EventManager::setKeyFilter(IKeyFilter* keyFilter) {
		m_keyfilter = keyFilter;
	}
}
