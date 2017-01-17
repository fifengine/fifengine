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

#ifndef FIFE_EVENTCHANNEL_EVENTMANAGER_H
#define FIFE_EVENTCHANNEL_EVENTMANAGER_H

// Standard C++ library includes
//
#include <deque>
#include <map>
#include <list>

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "util/base/fife_stdint.h"
#include "eventchannel/command/ec_command.h"
#include "eventchannel/command/ec_icommandcontroller.h"
#include "eventchannel/command/ec_icommandlistener.h"

#include "eventchannel/drop/ec_idropcontroller.h"
#include "eventchannel/drop/ec_idroplistener.h"
#include "eventchannel/drop/ec_dropevent.h"

#include "eventchannel/key/ec_ikeycontroller.h"
#include "eventchannel/key/ec_ikeylistener.h"
#include "eventchannel/key/ec_keyevent.h"
#include "eventchannel/key/ec_key.h"

#include "eventchannel/text/ec_itextcontroller.h"
#include "eventchannel/text/ec_itextlistener.h"
#include "eventchannel/text/ec_textevent.h"
#include "eventchannel/text/ec_text.h"

#include "eventchannel/mouse/ec_imousecontroller.h"
#include "eventchannel/mouse/ec_imouselistener.h"
#include "eventchannel/mouse/ec_mouseevent.h"

#include "eventchannel/sdl/ec_isdleventcontroller.h"
#include "eventchannel/sdl/ec_isdleventlistener.h"

namespace FIFE {

	class ICommandListener;
	class InputEvent;
	class MouseEvent;
	class KeyEvent;
	class IKeyFilter;
	class DropEvent;
	class IMouseFilter;

	/**  Event Manager manages all events related to FIFE
	 */
	class EventManager:
		public ICommandController,
		public IKeyController,
		public ITextController,
		public IMouseController,
		public ISdlEventController,
		public IDropController,
		public IEventSource {
	public:
		/** Constructor.
		 */
		EventManager();

		/** Destructor
		 */
		virtual ~EventManager();

		void addCommandListener(ICommandListener* listener);
		void addCommandListenerFront(ICommandListener* listener);
		void removeCommandListener(ICommandListener* listener);

		void dispatchCommand(Command& command);

		void addKeyListener(IKeyListener* listener);
		void addKeyListenerFront(IKeyListener* listener);
		void removeKeyListener(IKeyListener* listener);

		void addTextListener(ITextListener* listener);
		void addTextListenerFront(ITextListener* listener);
		void removeTextListener(ITextListener* listener);

		void addMouseListener(IMouseListener* listener);
		void addMouseListenerFront(IMouseListener* listener);
		void removeMouseListener(IMouseListener* listener);

		void addSdlEventListener(ISdlEventListener* listener);
		void addSdlEventListenerFront(ISdlEventListener* listener);
		void removeSdlEventListener(ISdlEventListener* listener);

		void addDropListener(IDropListener* listener);
		void addDropListenerFront(IDropListener* listener);
		void removeDropListener(IDropListener* listener);

		EventSourceType getEventSourceType();

		/** Process the SDL event queue.
		 * This is to be called only by the engine itself once per frame.
		 * It passes appropriate events to their listeners
		 */
		void processEvents();

		void setKeyFilter(IKeyFilter* keyFilter);

		void setMouseFilter(IMouseFilter* mouseFilter);

		/** Sets mouse sensitivity
		 * The sensitivity is limited to the range -0.99 - 10.0.
		 */
		void setMouseSensitivity(float sensitivity);

		/** Gets mouse sensitivity
		 */
		float getMouseSensitivity() const;

		/** Sets mouse acceleration
		 * if mouse acceleration is enabled,
		 * then the mouse sensitivity is used as speed max.
		 */
		void setMouseAccelerationEnabled(bool acceleration);

		/** Returns if mouse acceleration is enabled or not.
		 *
		 *  @return True if mouse acceleration is enabled, false if not.
		 */
		bool isMouseAccelerationEnabled() const;

		/** Returns if clipboard have text or not.
		 *
		 *  @return True if the clipboard contains text, false if not.
		 */
		bool isClipboardText() const;

		/** Returns the clipboard text as UTF-8 string.
		 *
		 *  @return UTF-8 string or an empty string if the cliboard have no text.
		 */
		std::string getClipboardText() const;

		/** Sets clipboard text.
		 *
		 *  @param text The text in UTF-8 format.
		 */
		void setClipboardText(const std::string& text);


	private:
		// Helpers for processEvents
		void processWindowEvent(SDL_Event event);
		void processKeyEvent(SDL_Event event);
		void processTextEvent(SDL_Event event);
		void processMouseEvent(SDL_Event event);
		void processDropEvent(SDL_Event event);
		bool combineEvents(SDL_Event& event1, const SDL_Event& event2);

		// Events dispatchers - only dispatchSdlevent may reject the event.
		bool dispatchSdlEvent(SDL_Event& evt);
		void dispatchKeyEvent(KeyEvent& evt);
		void dispatchTextEvent(TextEvent& evt);
		void dispatchMouseEvent(MouseEvent& evt);
		void dispatchDropEvent(DropEvent& evt);

		// Translate events
		void fillModifiers(InputEvent& evt);
		void fillKeyEvent(const SDL_Event& sdlevt, KeyEvent& keyevt);
		void fillTextEvent(const SDL_Event& sdlevt, TextEvent& txtevt);
		void fillMouseEvent(const SDL_Event& sdlevt, MouseEvent& mouseevt);

		std::deque<ICommandListener*> m_commandlisteners;
		std::deque<ICommandListener*> m_pending_commandlisteners;
		std::deque<ICommandListener*> m_pending_commandlisteners_front;
		std::deque<ICommandListener*> m_pending_cldeletions;

		std::deque<IKeyListener*> m_keylisteners;
		std::deque<IKeyListener*> m_pending_keylisteners;
		std::deque<IKeyListener*> m_pending_keylisteners_front;
		std::deque<IKeyListener*> m_pending_kldeletions;

		std::deque<ITextListener*> m_textListeners;
		std::deque<ITextListener*> m_pendingTextListeners;
		std::deque<ITextListener*> m_pendingTextListenersFront;
		std::deque<ITextListener*> m_pendingTlDeletions;

		std::deque<IMouseListener*> m_mouselisteners;
		std::deque<IMouseListener*> m_pending_mouselisteners;
		std::deque<IMouseListener*> m_pending_mouselisteners_front;
		std::deque<IMouseListener*> m_pending_mldeletions;

		std::deque<ISdlEventListener*> m_sdleventlisteners;
		std::deque<ISdlEventListener*> m_pending_sdleventlisteners;
		std::deque<ISdlEventListener*> m_pending_sdleventlisteners_front;
		std::deque<ISdlEventListener*> m_pending_sdldeletions;

		std::deque<IDropListener*> m_dropListeners;
		std::deque<IDropListener*> m_pendingDropListeners;
		std::deque<IDropListener*> m_pendingDropListenersFront;
		std::deque<IDropListener*> m_pendingDlDeletions;

		std::map<int32_t, bool> m_keystatemap;
		IKeyFilter* m_keyfilter;
		IMouseFilter* m_mousefilter;
		int32_t m_mousestate;
		MouseEvent::MouseButtonType m_mostrecentbtn;

		// m_mousesensitivity is the mouse speed factor - 1, so a value of 0 won't
		// influence mouse speed, a value of 1 would double the speed and
		// -.5 would make it half the speed
		float m_mousesensitivity;
		bool m_acceleration;
		bool m_warp;
		bool m_enter;
		uint16_t m_oldx;
		uint16_t m_oldy;
		uint32_t m_lastticks;
		float m_oldvelocity;

	};
} //FIFE

#endif
