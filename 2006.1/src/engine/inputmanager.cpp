/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#include "inputmanager.h"
#include "engine.h"
#include "debugutils.h"
#include "inputlistener.h"
#include "video/gui/guimanager.h"
#include <guichan.hpp>
#include <guichan/sdl/sdlinput.hpp>

#include <SDL.h>
#include <algorithm>

namespace FIFE {

	const std::string InputManager::ENGINE_CONTEXT = "engine";

	InputManager::InputManager() : m_listeners_key(), m_listeners_mousebutton(),
                                 m_forced_listener(0) {
		pushContext(ENGINE_CONTEXT);
	}

	InputManager::~InputManager() {
		popContext(ENGINE_CONTEXT);
	}

	void InputManager::registerEventListener(const std::string& context, InputListener* listener) {
		m_eventlisteners.insert(std::make_pair(context,listener));
	}
	void InputManager::unregisterEventListener(InputListener* listener) {
		type_eventlisteners::const_iterator end = m_eventlisteners.end();
		for (type_eventlisteners::iterator i = m_eventlisteners.begin(); i != end;) {
			if (i->second == listener)
				m_eventlisteners.erase(i++);
			else
				++i;
		}
	}

	/*
	void InputManager::setCurrentContext(const std::string& context){};
	*/
	const std::string& InputManager::getCurrentContext() const {
		if( m_contextstack.empty() ) {
			PANIC_PRINT("no context available!");
		}
		return m_contextstack.back();
	}
	

	void InputManager::pushContext(const std::string& context) {
		m_contextstack.push_back(context);
	}
	void InputManager::popContext(const std::string& context) {
		// FIXME: See comment
		m_contextstack.pop_back();
	}

	void InputManager::mapKeyToEventId(const std::string& context, uint8_t key, int eventId) {
		type_eventmap &kmap = m_context_eventmap[context];
		kmap[key] = eventId;
	}

	void InputManager::unmapKey(const std::string& context, uint8_t key) {
		type_eventmap &kmap = m_context_eventmap[context];
		kmap.erase(key);
	}
	

	void InputManager::setForcedListener(InputListenerRaw* listener) {
		m_forced_listener = listener;
	}

	void InputManager::registerKeyListener(uint8_t key, InputListener* listener) {
		m_listeners_key.insert(std::make_pair(key, listener));
	}

	void InputManager::registerMouseButtonListener(InputListener* listener) {
		m_listeners_mousebutton.push_back(listener);
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
		unregisterEventListener(listener);
	}

	void InputManager::handleEvents() {
		// FIXME: This needs to be re-written more readable!
		// See the not on getCurrentContext
		std::list<InputListener*> eventListener,anyeventListener;
		type_eventmap &kmap = m_context_eventmap[getCurrentContext()];
		type_eventmap &anymap = m_context_eventmap[ENGINE_CONTEXT];

		std::pair<type_eventlisteners::iterator, type_eventlisteners::const_iterator> limits = 
			m_eventlisteners.equal_range(getCurrentContext());
		for(type_eventlisteners::iterator it = limits.first; it != limits.second; ++it) 
			eventListener.push_back(it->second);

		std::pair<type_eventlisteners::iterator, type_eventlisteners::const_iterator> limits2 = 
			m_eventlisteners.equal_range(ENGINE_CONTEXT);
		for(type_eventlisteners::iterator it = limits2.first; it != limits2.second; ++it) 
			anyeventListener.push_back(it->second);

		std::list<InputListener*>::iterator it,end(eventListener.end()),anyend(anyeventListener.end());

		SDL_Event event;
		// ugly, but has to do for now...
		gcn::SDLInput *input = dynamic_cast<gcn::SDLInput*>(GUIManager::instance()->getGuichanGUI()->getInput());
		while (SDL_PollEvent(&event)) {

			switch (event.type) {
				case SDL_QUIT:
					Engine::instance()->stop();
					break;

				case SDL_KEYDOWN:
					// Do this for now, until we can rely on startup scripts
					if( event.key.keysym.sym == SDLK_ESCAPE ) {
						Engine::instance()->stop();
					}
					// FIXME: Damn this IS ugly
					if( kmap.find(event.key.keysym.sym) != kmap.end() ) {
						for(it = eventListener.begin(); it != end; ++it) {
							(*it)->handleEvent( kmap[event.key.keysym.sym] );
						}
					} else if( anymap.find(event.key.keysym.sym) != anymap.end() ) {
						for(it = anyeventListener.begin(); it != anyend; ++it) {
							(*it)->handleEvent( anymap[event.key.keysym.sym] );
						}
					} else {
						handleKeyEvent(event.key);
					}
					break;
				case SDL_KEYUP:
				//Log("DEBUG-k1") << event.key.keysym.sym;
					break;
				default:
					break;
			}
			if ((event.type == SDL_MOUSEBUTTONUP) || (event.type == SDL_MOUSEBUTTONDOWN))
				handleMouseButtonEvent(event.button);
			
			// all events pushed to guichan
			input->pushInput(event);
			if (m_forced_listener) {
				m_forced_listener->handleEvent(&event);
			}

		}
	}

	void InputManager::handleKeyEvent(const SDL_KeyboardEvent& event) {

		// keep this for now ...
		type_listeners_key listeners = m_listeners_key;
		std::pair<type_listeners_key::iterator, type_listeners_key::const_iterator> limits = listeners.equal_range(event.keysym.sym);
		for (type_listeners_key::iterator i = limits.first; i != limits.second; ++i) {
			i->second->handleKeyEvent(event);
		}
	}

	void InputManager::handleMouseButtonEvent(const SDL_MouseButtonEvent& event) {
		std::vector<InputListener*>::iterator i = m_listeners_mousebutton.begin();
		while (i != m_listeners_mousebutton.end()) {
			(*i)->handleMouseButtonEvent(event);
			++i;
		}
	}

}//FIFE
