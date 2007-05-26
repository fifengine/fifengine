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

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "util/singleton.h"

#include "../key/ec_ikeylistener.h"
#include "../key/ec_ikey.h"
#include "../ec_ieventcontroller.h"

namespace FIFE {

	class ICommandListener;

	/**  Event Manager manages all events related to FIFE
	 */
	class EventManager: 
		public DynamicSingleton<EventManager>, 
		public IEventController {
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

		void dispatchCommand(const ICommand& command);

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

		std::vector<ICommandListener*> m_commandlisteners;
		std::vector<IKeyListener*> m_keylisteners;
		std::vector<IMouseListener*> m_mouselisteners;
		std::vector<ISdlEventListener*> m_sdleventlisteners;
		std::vector<IWidgetListener*> m_widgetlisteners;
	};

} //FIFE

#endif
