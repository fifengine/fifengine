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
#include "objectmanager.h"
#include "objectinfo.h"
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

	void Runner::initialize(View* view, Map* map, ObjectManager* mom) {
		m_mom = mom;
		m_map = map;
		m_view = view;

		type_cmdmap::iterator it = m_commands.begin();
		for(; it != m_commands.end(); ++it) {
			it->second->initialize(view, map, m_mom);
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
		sendEvent(makeEvent(FIFE_EXEC,m_ruleset));
		sendEvents();
		the_thread = SDL_CreateThread(slaveThread, 0);

		std::string script;
		for (size_t i = 1; i < m_mom->m_objects.size(); ++i) {
			ObjectPtr moi = m_mom->getObject(i);
			if (moi->isStatic() || !m_ruleset.isValid()) {
				m_static_objects[moi->getLocation().elevation].push_back(moi);
			} else {
				script += packObject(moi, i);
			}
		}

		Debug("map_runner") << "Script Object Injektion!\n";
		sendNewExecScEvent(script);
		sendEvents();
		Debug("map_runner") << "Script Object Injektion! (returned)\n";
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
		sendNewExecScEvent("ruleset:activateElevation(" +
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
			visual->setRenderable(iid);

			visual->setLocation(moi->getLocation());

			Debug("map::runner")
				<< "Adding Visual for static object iid:" << iid
				<< " rloc:" << loc.toString();
			moi->debugPrint();

			m_view->addVisual(visual);
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

	// Rewrote this using a stringstream instead of multiple lexical_casts.
	// --zahlman
	std::string Runner::packObject(ObjectPtr moi, size_t id) {
		std::stringstream ss;

		ss << "ruleset:createObject({";
		
/*		// Disabled for now.
		ObjectInfo::type_params::iterator it = moi->params.begin();
		for (; it != moi->params.end(); ++it) {
			ss << it->first << " = '" << it->second << "', ";
		}
*/
		// Push standard ruleset data
		ss << "visual='" << moi->getVisualLocation().getFilename() << "',";
		ss << "objtype='" << moi->get<std::string>("objtype") << "',";
		ss << "name='" << moi->get<std::string>("name") << "',";
		
		Location& loc = moi->getLocation();
		
		ss << " location = { elevation = " ;
		if (loc.elevation > 1000) {
			ss << "nil";
		} else {
			ss << loc.elevation;
		}
		
		ss << ", grid = " << loc.layer
		   << ", x = " << loc.position.x
		   << ", y = " << loc.position.y
		   << " }, id = " << id << "}); ";

		return ss.str();
	}

	void Runner::doCommand(const command::Info& cmd) {
		command::Command* command = m_commands[cmd.commandId];
		if (command) { command->execute(cmd); }
	}

	void Runner::registerCommand(size_t commandId, command::Command* mc) {
		m_commands[commandId] = mc;
	}
} } // FIFE::map
