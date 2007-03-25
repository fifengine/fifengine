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

#ifndef FIFE_MAP_RUNNER_H
#define FIFE_MAP_RUNNER_H

// Standard C++ library includes
#include <map>

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/command/info.h"
#include "script/scriptcontainer.h"
#include "asyncbridge.h"

struct SDL_Thread;

namespace FIFE { namespace map {

	class Map;
	typedef boost::shared_ptr<Map> MapPtr;

	class View;
	class ObjectManager;
	class ObjectInfo;
	typedef boost::shared_ptr<ObjectInfo> ObjectPtr;
	class ScriptEngine;
	class ScriptingSlave;

	namespace command { class Command; }

	/** The Engine side of the ruleset scripting interface
	 *
	 *  This class controls the interaction between the
	 *  Ruleset through a Asnychronous Bridge interface
	 *  and the rest of the Engine.
	 *
	 *  It starts/stops the scripting thread and handles
	 *  the communication.
	 *  For this it implements a command infrastructure.
	 *  Commands are registered to an integer number
	 *  through the registerCommand function
	 *  Internally a pipe/queue is filled by the
	 *  scripting thread with command info structures
	 *  which are handled by the registered commands.
	 *
	 *  This class is currently only used by the MapControl
	 *  'Master' object which binds all map related stuff
	 *  together.
	 *
	 *  @warning It's not enough to stop calling
	 * 	'turn' to pause the ruleset. A 'pause'
	 *	function needs to be implemented.
	 */
	class Runner : public AsyncBridge {
		public:
			/** Constructor
			 */
			Runner() : AsyncBridge(), m_ruleset(), m_mom(0), m_slave(0) {}

			/** Destructor
			 */
			~Runner();
	
			/** Set the ruleset to be used
			 *  @note Execute this _before_ calling start.
			 *
			 *  @param ruleset A scriptcontainer pointing to a lua snippet,
			 *	that when executed sets up a lua environment to be 
			 *	used as ruleset.
			 */
			void setRuleset(const ScriptContainer& ruleset);

			/** Initialize references to other relevant map objects
			 *  @note Execute this _before_ calling start.
			 *
			 *  The pointers are used to initialize the commands
			 */
			void initialize(MapPtr map, View* view);

			/** Start the ruleset execution
			 *  This will start the ruleset, the scripting thread and will
			 *  return afterwards.
			 */
			void start();

			/** Flush the command queues
			 *  This will perform the commands requested by the
			 *  scripting thread.
			 *  It will also send a 'heartbeat' event to the scripting thread.
			 */
			void turn();

			/** Stop ruleset execution
			 *  This will stop the ruleset execution.
			 *  Destroy the scripting thread and clean up.
			 */
			void stop();

			/** Activate an elevation
			 *
			 */
			void activateElevation(size_t elev);

			/** Display elevation
			 */
			void displayElevation(size_t elev);

			/** Register a command to a specific command_id
			 *  Takes ownership over the commands.
			 */
			void registerCommand(size_t commandid, command::Command* command);

		private:
			static int slaveThread(void* data);
			ScriptContainer m_ruleset;
			ObjectManager* m_mom;
			MapPtr m_map;
			View*  m_view;
			ScriptingSlave* m_slave;
			SDL_Thread*     m_thread;

			void processEvent(const event_t& e);

			void sendHeartbeat(size_t);

			typedef std::map<size_t, command::Command*> type_cmdmap;
			type_cmdmap m_commands;
			void doCommand(const command::Info& cmd);

			void sendNewExecScEvent(const std::string& name);

			friend struct send_objects;
			friend struct display_objects;
	};
} }

#endif // FIFE_MAPRUNNER_H
