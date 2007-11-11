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

#ifndef FIFE_EVENTCHANNEL_EVENTMANAGER_H
#define FIFE_EVENTCHANNEL_EVENTMANAGER_H

// Standard C++ library includes
//
#include <vector>
#include <map>

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "util/singleton.h"
#include "eventchannel/key/ec_ikeylistener.h"
#include "eventchannel/key/ec_ikey.h"
#include "eventchannel/ec_ieventcontroller.h"
#include "eventchannel/widget/ec_iwidgetlistener.h"

namespace FIFE {

	class ICommandListener;
	class InputEvent;
	class MouseEvent;
	class KeyEvent;

	/**  Event Manager manages all events related to FIFE
	 */
	class EventManager: 
		public DynamicSingleton<EventManager>, 
		public IEventController,
		public IWidgetListener {
	public:
		/** Constructor.
		 */
		EventManager();

		/** Destructor
		 */
		virtual ~EventManager();

		void addCommandListener(ICommandListener* listener);
		void removeCommandListener(ICommandListener* listener);
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

		void dispatchCommand(ICommand& command);

		void onWidgetAction(IWidgetEvent& evt);

		/** Process the SDL event queue.
		 * This is to be called only by the engine itself once per frame. 
		 * It passes appropriate events to their listeners
		 */
		void processEvents();

	private:
		void dispatchKeyEvent(IKeyEvent& evt);
		void dispatchMouseEvent(IMouseEvent& evt);
		void dispatchSdlEvent(SDL_Event& evt);
		void dispatchWidgetEvent(IWidgetEvent& evt);
		void fillModifiers(InputEvent& evt);
		void fillKeyEvent(const SDL_Event& sdlevt, KeyEvent& keyevt);
		void fillMouseEvent(const SDL_Event& sdlevt, MouseEvent& mouseevt);

		std::vector<ICommandListener*> m_commandlisteners;
		std::vector<ICommandListener*> m_pending_commandlisteners;

		std::vector<IKeyListener*> m_keylisteners;
		std::vector<IKeyListener*> m_pending_keylisteners;

		std::vector<IMouseListener*> m_mouselisteners;
		std::vector<IMouseListener*> m_pending_mouselisteners;

		std::vector<ISdlEventListener*> m_sdleventlisteners;
		std::vector<ISdlEventListener*> m_pending_sdleventlisteners;

		std::vector<IWidgetListener*> m_widgetlisteners;
		std::vector<IWidgetListener*> m_pending_widgetlisteners;

		std::vector<int> m_nonconsumablekeys;
		std::map<int, bool> m_keystatemap;
		int m_mousestate;
		IMouseEvent::MouseButtonType m_mostrecentbtn;
	};
} //FIFE

#endif
