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
#include <sstream>

// 3rd party library includes
#include <boost/lexical_cast.hpp>
#include <SDL.h>
#include <SDL_thread.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "command/info.h"
#include "command/command.h"
#include "imagecache.h"
#include "log.h"
#include "scriptingslave.h"

#include "event.h" // <-- ?
#include "map.h"
#include "objectinfo.h"
#include "object_iterator.h"
#include "runner.h"
#include "view.h"
#include "visual.h"

namespace FIFE { namespace map {

	static ScriptingSlave* the_slave = 0;
	static SDL_Thread* the_thread = 0;

	int slaveThread(void*) {
		Debug("slavethread") << "SlaveThread: entering ScriptingSlave::loop()";
		assert(the_slave);
		the_slave->loop();
		Debug("slavethread") << "SlaveThread: leaving ScriptingSlave::loop() ";
		delete the_slave;
		return 0;
	}

	Runner::~Runner() {

		// Delete commands
		type_cmdmap::iterator it = m_commands.begin();
		for(; it != m_commands.end(); ++it) {
			delete it->second;
		}
	}

	void Runner::setRuleset(const ScriptContainer& rs) {
		m_ruleset = rs;
	}

	void Runner::initialize(MapPtr map, View* view) {
		m_map = map;
		m_view = view;

		type_cmdmap::iterator it = m_commands.begin();
		for(; it != m_commands.end(); ++it) {
			it->second->initialize(map, view);
		}
	}

	// Extracted a helper function...
	void Runner::sendNewExecScEvent(const std::string& name) {
		const ScriptContainer::Type str = ScriptContainer::ScriptString;
		ScriptContainer sc(str, name);

		sendEvent(makeEvent(FIFE_EXEC,sc));
	}

	void Runner::start() {
		the_slave = m_slave = new ScriptingSlave();
		setSource(m_slave);
		m_slave->setSource(this);

		bool use_ruleset = false;
		if( use_ruleset ) {
			sendEvent(makeEvent(FIFE_EXEC,m_ruleset));
			use_ruleset = m_ruleset.isValid();
		}

		sendEvents();
		the_thread = SDL_CreateThread(slaveThread, 0);

		for(size_t i =0; i!= m_map->getElevationCount(); ++i) {
			ObjectIterator object_it(m_map->getElevation(i));
			ObjectPtr object;
			while((object = object_it.next())) {
				m_static_objects[object->getLocation().elevation].push_back(object);
				if ( !object->isStatic() && use_ruleset) {
					object->set<long>("elevation",object->getLocation().elevation);
					object->set<long>("layer",object->getLocation().layer);
					object->set<Point>("position",object->getLocation().position);
					sendEvent(makeEvent(FIFE_NEW_OBJECT,object));
				}
			}
		}
		sendEvents();
	}

	void Runner::stop() {
		m_static_objects.clear();
		sendShutdown();
		sendEvents();
		int result;
		SDL_WaitThread(the_thread, &result);
		clearQueue();
		Log("map_runner")
			<< "Scripting Thread returned " << result;
	}

	void Runner::turn() {
		sendHeartbeat(SDL_GetTicks());
		sendEvents();
		processEvents();
	}

	void Runner::activateElevation(size_t elevation) {
		// send the activation command to the ruleset.
		sendNewExecScEvent("ActivateElevation(" +
		                   boost::lexical_cast<std::string>(elevation) + ")");

		// display static visuals
		for (size_t i = 0; i < m_static_objects[elevation].size(); ++i) {
			ObjectPtr moi = m_static_objects[elevation][i];
			if (!m_map->isValidLocation(moi->getLocation())) {
				continue;
			}
			Visual* visual = new Visual(moi);

			RenderableLocation loc(moi->getVisualLocation());
			size_t iid = ImageCache::instance()->addImageFromLocation(loc);
			visual->setRenderable(iid, loc.getType());

			visual->setLocation(moi->getLocation());

			Debug("map_runner")
				<< "Adding Visual for static object iid:" << iid
				<< " rloc:" << loc.toString();
// 			moi->debugPrint();

			moi->setVisualId( m_view->addVisual(visual) );
		}
	}

	void Runner::processEvent(const event_t& e) {
		switch (e.code()) {
			case FIFE_EXECCOMMAND:
				doCommand(e.get<command::Info>());
				break;

			default:
				AsyncBridge::processEvent(e);
		}
	}

	void Runner::sendHeartbeat(size_t beat) {
		sendEvent(event_t(0,FIFE_HEARTBEAT));
	}

	void Runner::doCommand(const command::Info& cmd) {
		command::Command* command = m_commands[cmd.commandId];
		if (command) { command->execute(cmd); }
	}

	void Runner::registerCommand(size_t commandId, command::Command* mc) {
		m_commands[commandId] = mc;
	}
} } // FIFE::map
