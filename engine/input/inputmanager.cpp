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
#include <algorithm>

// 3rd party library includes
#include <guichan/sdl/sdlinput.hpp>
#include <guichan.hpp>
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/debugutils.h"

#include "events.h"
#include "inputmanager.h"
#include "listener.h"

namespace FIFE { namespace input {

	const std::string Manager::ENGINE_CONTEXT = "engine";

	Manager::Manager() : m_listeners_key(), m_listeners_mousebutton(),
	                     m_forced_listener(0) {
		pushContext(ENGINE_CONTEXT);
	}

	Manager::~Manager() {
		popContext(ENGINE_CONTEXT);
	}

	void Manager::registerEventListener(const std::string& context, Listener* listener) {
		m_eventlisteners.insert(std::make_pair(context, listener));
	}

	void Manager::unregisterEventListener(Listener* listener) {
		type_eventlisteners::const_iterator end = m_eventlisteners.end();
		for (type_eventlisteners::iterator i = m_eventlisteners.begin(); i != end;) {
			if (i->second == listener)
				m_eventlisteners.erase(i++);
			else
				++i;
		}
	}

	const std::string& Manager::getCurrentContext() const {
		if (m_contextstack.empty()) {
			PANIC_PRINT("no context available!");
		}
		return m_contextstack.back();
	}

	void Manager::pushContext(const std::string& context) {
		m_contextstack.push_back(context);
	}

	void Manager::popContext(const std::string& context) {
		m_contextstack.erase(
			std::remove(m_contextstack.begin(),m_contextstack.end(),context));
	}

	void Manager::mapKeyToEventId(const std::string& context, int key, int eventId) {
		type_eventmap &kmap = m_context_eventmap[context];
		kmap[key] = eventId;
	}

	void Manager::unmapKey(const std::string& context, int key) {
		type_eventmap &kmap = m_context_eventmap[context];
		kmap.erase(key);
	}
	
	void Manager::setForcedListener(ListenerRaw* listener) {
		m_forced_listener = listener;
	}

	void Manager::registerKeyListener(int key, Listener* listener) {
		m_listeners_key.insert(std::make_pair(key, listener));
	}

	void Manager::registerMouseButtonListener(Listener* listener) {
		m_listeners_mousebutton.push_back(listener);
	}

	void Manager::unregisterMouseButtonListener(Listener* listener) {
		std::vector<Listener*>::iterator i = m_listeners_mousebutton.begin();
		while (i != m_listeners_mousebutton.end()) {
			if (*i == listener) {
				m_listeners_mousebutton.erase(i);
				return;
			}
			++i;
		}
		Log("input::Manager") << "Cannot remove unknown mouse-button listener";
	}

	void Manager::unregisterKeyListener(Listener* listener) {
		type_listeners_key::const_iterator end = m_listeners_key.end();
		for (type_listeners_key::iterator i = m_listeners_key.begin(); i != end;) {
			if (i->second == listener)
				m_listeners_key.erase(i++);
			else
				++i;
		}
	}

	void Manager::unregisterListener(Listener* listener) {
		unregisterKeyListener(listener);
		unregisterEventListener(listener);
	}

	void Manager::onSdlEvent(const SDL_Event& event) {
		std::list<Listener*> eventListener,anyeventListener;
		type_eventmap &kmap = m_context_eventmap[getCurrentContext()];
		type_eventmap &anymap = m_context_eventmap[ENGINE_CONTEXT];

		std::pair<type_eventlisteners::iterator, type_eventlisteners::const_iterator> limits = 
			m_eventlisteners.equal_range(getCurrentContext());
		for (type_eventlisteners::iterator it = limits.first; it != limits.second; ++it) 
			eventListener.push_back(it->second);

		std::pair<type_eventlisteners::iterator, type_eventlisteners::const_iterator> limits2 = 
			m_eventlisteners.equal_range(ENGINE_CONTEXT);
		for (type_eventlisteners::iterator it = limits2.first; it != limits2.second; ++it) 
			anyeventListener.push_back(it->second);

		std::list<Listener*>::iterator it,end(eventListener.end()),anyend(anyeventListener.end());

		switch (event.type) {
			case SDL_QUIT:
				for(it = anyeventListener.begin(); it != anyend; ++it) {
					(*it)->handleEvent(Event::QUIT_GAME);
				}
				break;
			case SDL_KEYDOWN:
				// Do this for now, until we can rely on startup scripts.
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					for(it = eventListener.begin(); it != end; ++it) {
						(*it)->handleEvent(Event::QUIT_GAME);
					}
				}
				// FIXME: Damn this IS ugly.
				if (kmap.find(event.key.keysym.sym) != kmap.end()) {
					for(it = eventListener.begin(); it != end; ++it) {
						(*it)->handleEvent( kmap[event.key.keysym.sym] );
					}
				} else if (anymap.find(event.key.keysym.sym) != anymap.end()) {
					for(it = anyeventListener.begin(); it != anyend; ++it) {
						(*it)->handleEvent( anymap[event.key.keysym.sym] );
					}
				} else {
					handleKeyEvent(event.key);
				}
				break;

			case SDL_KEYUP:
				// Log("DEBUG-k1") << event.key.keysym.sym;
				break;
			default:
				break;
		}

		if ((event.type == SDL_MOUSEBUTTONUP) || (event.type == SDL_MOUSEBUTTONDOWN))
			handleMouseButtonEvent(event.button);
		
		// push raw events
		if (m_forced_listener) {
			m_forced_listener->handleEvent(const_cast<SDL_Event*>(&event));
		}
	}

	// Apply 'a' to every value under key 'k' in the 'map'.
	// Won't work: select2nd isn't standard.
	// This functional-in-C++ stuff really doesn't work as well as it ought to.
	// But we still might want to make some utilities... we could do it perhaps
	// with an iterator-adapter for iterators over pairs, that pretends to be
	// an iterator over the values (or the keys for that matter).
#if 0
	template <typename key, typename value, typename action>
	void dispatch_to_matches(const std::multimap<key, value>& map,
	                         const key& k, const action& a) {
		typedef std::multimap<key, value> type_map;
		typedef std::pair<typename type_map::iterator, 
		                  typename type_map::iterator> type_ip;
		type_ip range = map.equal_range(k);
		std::for_each(range.first, range.second, 
				          std::compose1(a, std::select2nd<type_ip>()));
	}
#endif

	void Manager::handleKeyEvent(const SDL_KeyboardEvent& event) {
		// Keep this for now ...
		type_listeners_key listeners = m_listeners_key;
		typedef type_listeners_key::iterator iterator;
		typedef type_listeners_key::const_iterator const_iterator;
		typedef std::pair<iterator, const_iterator> iter_pair;
		iter_pair limits = listeners.equal_range(event.keysym.sym);
		for (iterator i = limits.first; i != limits.second; ++i) {
			i->second->handleKeyEvent(event);
		}

#if 0
		dispatch_to_matches(m_listeners_key, uint8_t(event.keysym.sym), 
		                    std::mem_fun(&Listener::handleKeyEvent));
#endif
	}

	// And also an iterator-adapter for doing this double-indirection.
	// But maybe we should really just use boost::ptr_map or something?
	
	void Manager::handleMouseButtonEvent(const SDL_MouseButtonEvent& event) {
		typedef std::vector<Listener*>::iterator iterator;
		for (iterator i = m_listeners_mousebutton.begin();
		     i != m_listeners_mousebutton.end(); ++i) {
			(*i)->handleMouseButtonEvent(event);
		}
	}

} } // FIFE::input
