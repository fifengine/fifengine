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

#ifndef FIFE_INPUTMANAGER_H
#define FIFE_INPUTMANAGER_H

#include "singleton.h"
#include <stdint.h>
#include <map>
#include <vector>
#include <list>

#include "SDL_events.h"


namespace FIFE {

	class InputListener;
	class InputListenerRaw;

	/**  Input Manager
     *
     * Handles all SDL_Events. It allows classes to register themselves
	 * to be informed of specific keyboard events and mouse events.
	 * 
	 * Another class of use is registering to Events in a specific context.
	 * This should be used together with a GameState, where pushContext()
	 * is called in activate() and popContext() in deactivate().
	 * Only the top-level Context and the specific ENGINE_CONTEXT
	 * receive these events. They are generated from bindings that can be
	 * created once again per context with mapKeyToEventId(context,sdlkey,event).
	 * This function is exported to lua, so that key bindings can easily
	 * be changed in scripting code.
	 *
	 * \bug In fact i think handleKeyEvent should be removed from InputListener
	 * \bug uint8_t does NOT hold the complete range of Key events InputListeners might be interested in.
	 * \note Until we can rely on a startup script and thus bindings created by lua,
	 * The Escape KEY is hardcoded to call Engine::stop().
	 *
	 * This class also allows the user to set a forced listener which is passed all messages.
	 *
	 * To register classes for these tasks they must be derived from either
	 * InputListener or InputListenerRaw.
	 *
	 * @see InputListener, InputListenerRaw, Events
     */
	class InputManager : public DynamicSingleton<InputManager> {
		public:
			/** The Engine context
			 * This Context allways gets Events generated.
			 * It can be used for Screenshots and Engine Shutdown.
			*/
			static const std::string ENGINE_CONTEXT;

			/** Constructor.
			 */
			InputManager();

			/** Destructor
			 */
			virtual ~InputManager();

			/** Register a InputListener for a specific context
			 * @param context The context under which the listener shall receive events
			 * @param listener An instance of a subclass from InputListener.
			 */
			void registerEventListener(const std::string& context, InputListener* listener);

			/** Unregister an InputListener from all contexts it's assigned to.
			 * @param listener An instance of a subclass from InputListener.
			 */
			void unregisterEventListener(InputListener* listener);

			/** Get the current active context
			 * @return The current active context.
			 * @note That if an InputListener changes the current active context,
			 * the return value will reflect that, while handleEvents() will
			 * still procede to flush the SDL Event Queue without noticing 
			 * the change. The change have effect in the next turn.
			 */
			const std::string& getCurrentContext() const;

			/** Put a new active context on the stack.
			 * @see getCurrentContext, popContext
			 */
			void pushContext(const std::string& context);

			/** Pop a context from the stack
			 * This will also remove accumulated contexts above the current
			 * active one. If a context does not exist, a warning is made.
			 * \bug Implementation incomplete
			 * @see getCurrentContext, pushContext
			 */
			void popContext(const std::string& context);

			/** Map a Key to an Event in a specific context
			 */ 
			void mapKeyToEventId(const std::string& context, uint8_t key, int eventId);

			/** Unmaps a Key in a specific context.
			 */
			void unmapKey(const std::string& context, uint8_t key);

			/** Register a class to recieve key events.
			 *
			 * @param key The key to be notified about.
			 * @param listener The class to be notified.
			 */
			void registerKeyListener(uint8_t key, InputListener* listener);

			/** Register a class to recieve mouse events.
			 *
			 * @param listener The class to be notified.
			 */
			void registerMouseButtonListener(InputListener* listener);

			/** Unregisters a listener for all key events.
			 *
			 * @param listener Object to be unregistered.
			 */
			void unregisterKeyListener(InputListener* listener);

			/** Unregister a listener for all key events. 
			 *
			 * Simply calls unregisterKeyListener.
			 *
			 * @param listener Object to be unregistered.
			 */
			void unregisterListener(InputListener*);

			/** Set the forced listener. See class description.
			 *
			 * @param listener The object to be the forced listener.
			 */
			void setForcedListener(InputListenerRaw* listener);

			/** Process the SDL event queue.
			 *
			 * This should never be called by the user. It is called by
			 * Engine::mainLoop once a frame. It passes on all appropriete
			 * messages to the handleKeyEvent and handleMouseButtonEvent
			 * functions and the forced listener object.
			 * \bug Then why is this public?
			 * \bug The code needs refactoring
			 * \bug Calls Engine->stop() directly.
			 * \bug Does the GUI get all events pushed in???
			 */
			void handleEvents();

		private:
			typedef std::multimap<uint8_t, InputListener*> type_listeners_key;
			typedef std::map<uint8_t,int> type_eventmap;
			typedef std::map<std::string,type_eventmap>  type_context_eventmap;
			typedef std::multimap<std::string, InputListener*> type_eventlisteners;
			typedef std::list<std::string> type_contextstack;

			// Multimap of key listeners.
			type_listeners_key m_listeners_key;
			// Vector of mouse listeners.
			std::vector<InputListener*> m_listeners_mousebutton;

			// Map of event maps
			type_context_eventmap m_context_eventmap;
			type_eventlisteners m_eventlisteners;
			type_contextstack m_contextstack;

			// The forced listener.
			InputListenerRaw* m_forced_listener;

			/** Pass a key event to all appropriete key listeners.
			 *
			 * @param event The event to be passed on.
			 */
			void handleKeyEvent(const SDL_KeyboardEvent& event);

			/** Pass a mouse event to all mouse listeners.
			 *
			 * @param event The event to be passed on.
			 */
			void handleMouseButtonEvent(const SDL_MouseButtonEvent& event);

	};

}//FIFE

#endif
