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

#ifndef FIFE_EVENTCHANNEL_IEVENTCONTROLLER_H
#define FIFE_EVENTCHANNEL_IEVENTCONTROLLER_H

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
#include "command/ec_icommandcontroller.h"
#include "command/ec_icommanddispatcher.h"
#include "key/ec_ikey.h"
#include "key/ec_ikeycontroller.h"
#include "mouse/ec_imousecontroller.h"
#include "sdl/ec_isdleventcontroller.h"
#include "widget/ec_iwidgetcontroller.h"
#include "source/ec_ieventsource.h"

namespace FIFE {

	/**  Interface that can be passed to different subsystems so that they
	 * can subscribe themselves to events they are interested in, and send
	 * commands to other subsystems
	 */
	class IEventController: 
		public ICommandController,
		public IKeyController, 
		public IMouseController, 
		public ISdlEventController, 
		public IWidgetController,
		public ICommandDispatcher, 
		public IEventSource {
	public:

		/** Destructor
		 */
		virtual ~IEventController() {}

		virtual void addCommandListener(ICommandListener* listener) = 0;
		virtual void removeCommandListener(ICommandListener* listener) = 0;
		virtual void addKeyListener(IKeyListener* listener) = 0;
		virtual void removeKeyListener(IKeyListener* listener) = 0;
		virtual void addMouseListener(IMouseListener* listener) = 0;
		virtual void removeMouseListener(IMouseListener* listener) = 0;
		virtual void addSdlEventListener(ISdlEventListener* listener) = 0;
		virtual void removeSdlEventListener(ISdlEventListener* listener) = 0;
		virtual void addWidgetListener(IWidgetListener* listener) = 0;
		virtual void removeWidgetListener(IWidgetListener* listener) = 0;
		virtual EventSourceType getEventSourceType() = 0;
		virtual void setNonConsumableKeys(const std::vector<int>& keys) = 0;
		virtual std::vector<int> getNonConsumableKeys() = 0;

		void dispatchCommand(ICommand& command) = 0;
	};

} //FIFE

#endif
