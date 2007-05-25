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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/debugutils.h"
#include "util/exception.h"
#include "../key/ec_key.h"
#include "../key/ec_keyevent.h"
#include "../mouse/ec_mouseevent.h"
#include "../widget/ec_widgetevent.h"
#include "../command/ec_command.h"


#include "eventmanager.h"

namespace FIFE {

	IKey::KeyType convertKeyCharacter(const SDL_Event& evt) {
		SDL_keysym keysym = evt.key.keysym;
		
		IKey::KeyType value = IKey::INVALID_KEY;

		if (keysym.unicode < 255)
		{
			value = static_cast<IKey::KeyType>(keysym.unicode);
		}

		switch (keysym.sym)
		{
		case SDLK_TAB:
			value = IKey::TAB;
			break;
		case SDLK_LALT:
			value = IKey::LEFT_ALT;
			break;
		case SDLK_RALT:
			value = IKey::RIGHT_ALT;
			break;
		case SDLK_LSHIFT:
			value = IKey::LEFT_SHIFT;
			break;
		case SDLK_RSHIFT:
			value = IKey::RIGHT_SHIFT;
			break;
		case SDLK_LCTRL:
			value = IKey::LEFT_CONTROL;
			break;
		case SDLK_RCTRL:
			value = IKey::RIGHT_CONTROL;
			break;
		case SDLK_BACKSPACE:
			value = IKey::BACKSPACE;
			break;
		case SDLK_PAUSE:
			value = IKey::PAUSE;
			break;
		case SDLK_SPACE:
			// Special characters like ~ (tilde) ends up
			// with the keysym.sym SDLK_SPACE which
			// without this check would be lost. The check
			// is only valid on key down events in SDL.
			if (evt.type == SDL_KEYUP || keysym.unicode == ' ')
			{
				value = IKey::SPACE;
			}
			break;
		case SDLK_ESCAPE:
			value = IKey::ESCAPE;
			break;
		case SDLK_DELETE:
			value = IKey::DELETE;
			break;
		case SDLK_INSERT:
			value = IKey::INSERT;
			break;
		case SDLK_HOME:
			value = IKey::HOME;
			break;
		case SDLK_END:
			value = IKey::END;
			break;
		case SDLK_PAGEUP:
			value = IKey::PAGE_UP;
			break;
		case SDLK_PRINT:
			value = IKey::PRINT_SCREEN;
			break;
		case SDLK_PAGEDOWN:
			value = IKey::PAGE_DOWN;
			break;
		case SDLK_F1:
			value = IKey::F1;
			break;
		case SDLK_F2:
			value = IKey::F2;
			break;
		case SDLK_F3:
			value = IKey::F3;
			break;
		case SDLK_F4:
			value = IKey::F4;
			break;
		case SDLK_F5:
			value = IKey::F5;
			break;
		case SDLK_F6:
			value = IKey::F6;
			break;
		case SDLK_F7:
			value = IKey::F7;
			break;
		case SDLK_F8:
			value = IKey::F8;
			break;
		case SDLK_F9:
			value = IKey::F9;
			break;
		case SDLK_F10:
			value = IKey::F10;
			break;
		case SDLK_F11:
			value = IKey::F11;
			break;
		case SDLK_F12:
			value = IKey::F12;
			break;
		case SDLK_F13:
			value = IKey::F13;
			break;
		case SDLK_F14:
			value = IKey::F14;
			break;
		case SDLK_F15:
			value = IKey::F15;
			break;
		case SDLK_NUMLOCK:
			value = IKey::NUM_LOCK;
			break;
		case SDLK_CAPSLOCK:
			value = IKey::CAPS_LOCK;
			break;
		case SDLK_SCROLLOCK:
			value = IKey::SCROLL_LOCK;
			break;
		case SDLK_RMETA:
			value = IKey::RIGHT_META;
			break;
		case SDLK_LMETA:
			value = IKey::LEFT_META;
			break;
		case SDLK_LSUPER:
			value = IKey::LEFT_SUPER;
			break;
		case SDLK_RSUPER:
			value = IKey::RIGHT_SUPER;
			break;
		case SDLK_MODE:
			value = IKey::ALT_GR;
			break;
		case SDLK_UP:
			value = IKey::UP;
			break;
		case SDLK_DOWN:
			value = IKey::DOWN;
			break;
		case SDLK_LEFT:
			value = IKey::LEFT;
			break;
		case SDLK_RIGHT:
			value = IKey::RIGHT;
			break;
		case SDLK_RETURN:
			value = IKey::ENTER;
			break;
		case SDLK_KP_ENTER:
			value = IKey::ENTER;
			break;

		default:
			break;
		}

		if (!(keysym.mod & KMOD_NUM))
		{
			switch (keysym.sym)
			{
			case SDLK_KP0:
				value = IKey::INSERT;
				break;
			case SDLK_KP1:
				value = IKey::END;
				break;
			case SDLK_KP2:
				value = IKey::DOWN;
				break;
			case SDLK_KP3:
				value = IKey::PAGE_DOWN;
				break;
			case SDLK_KP4:
				value = IKey::LEFT;
				break;
			case SDLK_KP5:
				value = IKey::INVALID_KEY;
				break;
			case SDLK_KP6:
				value = IKey::RIGHT;
				break;
			case SDLK_KP7:
				value = IKey::HOME;
				break;
			case SDLK_KP8:
				value = IKey::UP;
				break;
			case SDLK_KP9:
				value = IKey::PAGE_UP;
				break;
			default:
				break;
			}
		}

		return value;
	}

	void fillMouseEvent(const SDL_Event& sdlevt, MouseEvent& mouseevt) {
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
	}

	void fillKeyEvent(const SDL_Event& sdlevt, KeyEvent& keyevt) {
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
		keyevt.setKey(Key(convertKeyCharacter(sdlevt)));
	}

	EventManager::EventManager(): m_commandlisteners(), m_keylisteners(),
		m_mouselisteners(), m_sdleventlisteners(), m_widgetlisteners()
 	{
	}

	EventManager::~EventManager() {
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
		Log("EventManager") << "Cannot remove unknown listener";
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

	void EventManager::dispatchCommand(const ICommand& command) {
		std::vector<ICommandListener*>::iterator i = m_commandlisteners.begin();
		while (i != m_commandlisteners.end()) {
			(*i)->onCommand(command);
			++i;
		}
	}

	void EventManager::dispatchKeyEvent(IKeyEvent& evt) {
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
			++i;
		}
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
			++i;
		}	
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
					dispatchKeyEvent(keyevt);
					}
					break;
				case SDL_MOUSEMOTION:
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN: {
					MouseEvent mouseevt;
					mouseevt.setSource(this);
					fillMouseEvent(event, mouseevt);
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
}
