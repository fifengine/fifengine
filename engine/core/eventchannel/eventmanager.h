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

#ifndef FIFE_EVENTCHANNEL_EVENTMANAGER_H
#define FIFE_EVENTCHANNEL_EVENTMANAGER_H

// Standard C++ library includes
//
#include <vector>
#include <map>
#include <list>

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "eventchannel/command/ec_command.h"
#include "eventchannel/command/ec_icommandcontroller.h"
#include "eventchannel/command/ec_icommandlistener.h"

#include "eventchannel/key/ec_ikeycontroller.h"
#include "eventchannel/key/ec_ikeylistener.h"
#include "eventchannel/key/ec_keyevent.h"
#include "eventchannel/key/ec_key.h"

#include "eventchannel/mouse/ec_imousecontroller.h"
#include "eventchannel/mouse/ec_imouselistener.h"
#include "eventchannel/mouse/ec_mouseevent.h"

#include "eventchannel/sdl/ec_isdleventcontroller.h"
#include "eventchannel/sdl/ec_isdleventlistener.h"

#include "eventchannel/widget/ec_iwidgetcontroller.h"
#include "eventchannel/widget/ec_iwidgetlistener.h"
#include "eventchannel/widget/ec_widgetevent.h"

#include "eventchannel/trigger/ec_itriggercontroller.h"

namespace FIFE {

	class ICommandListener;
	class InputEvent;
	class MouseEvent;
	class KeyEvent;

	/**  Event Manager manages all events related to FIFE
	 */
	class EventManager: 
		public ICommandController,
		public IKeyController, 
		public IMouseController, 
		public ISdlEventController, 
		public IWidgetController,
		public IEventSource,
		public IWidgetListener,
		public ITriggerController {
	public:
		/** Constructor.
		 */
		EventManager();

		/** Destructor
		 */
		virtual ~EventManager();

		void addCommandListener(ICommandListener* listener);
		void removeCommandListener(ICommandListener* listener);
		void dispatchCommand(Command& command);
		void addKeyListener(IKeyListener* listener);
		void removeKeyListener(IKeyListener* listener);
		void addMouseListener(IMouseListener* listener);
		void removeMouseListener(IMouseListener* listener);
		void addSdlEventListener(ISdlEventListener* listener);
		void removeSdlEventListener(ISdlEventListener* listener);
		void addWidgetListener(IWidgetListener* listener);
		void removeWidgetListener(IWidgetListener* listener);
		EventSourceType getEventSourceType();
		void setNonConsumableKeys(const std::vector<int>& keys);
		std::vector<int> getNonConsumableKeys();

		void onWidgetAction(WidgetEvent& evt);

		void registerTrigger(Trigger& trigger);
		void unregisterTrigger(Trigger& trigger);

		/** Process the SDL event queue.
		 * This is to be called only by the engine itself once per frame. 
		 * It passes appropriate events to their listeners
		 */
		void processEvents();

	private:
		void dispatchKeyEvent(KeyEvent& evt);
		void dispatchMouseEvent(MouseEvent& evt);
		void dispatchSdlEvent(SDL_Event& evt);
		void dispatchWidgetEvent(WidgetEvent& evt);
		void fillModifiers(InputEvent& evt);
		void fillKeyEvent(const SDL_Event& sdlevt, KeyEvent& keyevt);
		void fillMouseEvent(const SDL_Event& sdlevt, MouseEvent& mouseevt);
		void pollTriggers();
		
		std::vector<ICommandListener*> m_commandlisteners;
		std::vector<ICommandListener*> m_pending_commandlisteners;
		std::vector<ICommandListener*> m_pending_cldeletions;

		std::vector<IKeyListener*> m_keylisteners;
		std::vector<IKeyListener*> m_pending_keylisteners;
		std::vector<IKeyListener*> m_pending_kldeletions;

		std::vector<IMouseListener*> m_mouselisteners;
		std::vector<IMouseListener*> m_pending_mouselisteners;
		std::vector<IMouseListener*> m_pending_mldeletions;

		std::vector<ISdlEventListener*> m_sdleventlisteners;
		std::vector<ISdlEventListener*> m_pending_sdleventlisteners;
		std::vector<ISdlEventListener*> m_pending_sdldeletions;

		std::vector<IWidgetListener*> m_widgetlisteners;
		std::vector<IWidgetListener*> m_pending_widgetlisteners;
		std::vector<IWidgetListener*> m_pending_wldeletions;

		std::vector<int> m_nonconsumablekeys;
		std::map<int, bool> m_keystatemap;
		int m_mousestate;
		MouseEvent::MouseButtonType m_mostrecentbtn;

		std::list<Trigger*> m_triggers;
	};
} //FIFE

#endif
