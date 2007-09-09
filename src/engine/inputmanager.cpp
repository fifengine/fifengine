/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
#include "inputmanager.h"
#include "engine.h"
#include "debugutils.h"
#include "inputlistener.h"

#include <SDL.h>
#include <algorithm>

namespace FIFE {

	InputManager::InputManager() : m_listeners_key(), m_forced_listener(0) {}

	InputManager::~InputManager() {}

	InputManager::InputManager(const InputManager&) : m_listeners_key(), m_forced_listener(0) {}

	InputManager& InputManager::operator=(const InputManager&) {
		return *this;
	}

	void InputManager::setForcedListener(InputListenerRaw* listener) {
		m_forced_listener = listener;
	}

	void InputManager::registerKeyListener(uint8_t key, InputListener* listener) {
		m_listeners_key.insert(std::make_pair(key, listener));
	}

	void InputManager::unregisterKeyListener(InputListener* listener) {
		type_listeners_key::const_iterator end = m_listeners_key.end();
		for (type_listeners_key::iterator i = m_listeners_key.begin(); i != end;) {
			if (i->second == listener)
				m_listeners_key.erase(i++);
			else
				++i;
		}
	}

	void InputManager::unregisterListener(InputListener* listener) {
		unregisterKeyListener(listener);
	}

	void InputManager::handleEvents() {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {


			switch (event.type) {
					case SDL_QUIT:
						Engine::instance()->stop();
						break;

					case SDL_KEYUP:
					case SDL_KEYDOWN:
					handleKeyEvent(event.key);
					break;
					/*case SDL_KEYUP:
						break;
					case SDL_KEYDOWN:
					case SDL_MOUSEMOTION:
					case SDL_MOUSEBUTTONDOWN:
					case SDL_MOUSEBUTTONUP:
						Engine::instance()->getGUI()->input(event);
						break;*/
					default:
						break;
			}

			if (m_forced_listener) {
				m_forced_listener->handleEvent(&event);
			}

		}
	}

	void InputManager::handleKeyEvent(const SDL_KeyboardEvent& event) {

		type_listeners_key listeners = m_listeners_key;
		std::pair<type_listeners_key::iterator, type_listeners_key::const_iterator> limits = listeners.equal_range(event.keysym.sym);
		for (type_listeners_key::iterator i = limits.first; i != limits.second; ++i) {
			i->second->handleKeyEvent(event);
		}
	}

}//FIFE
