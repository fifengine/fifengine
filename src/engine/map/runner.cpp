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

#include "event.h"
#include "map.h"
#include "elevation.h"
#include "layer.h"
#include "objectinfo.h"
#include "runner.h"
#include "view.h"
#include "visual.h"

namespace FIFE { namespace map {

	int Runner::slaveThread(void* data) {
		ScriptingSlave* slave = reinterpret_cast<ScriptingSlave*>(data);
		Debug("slavethread") << "SlaveThread: entering ScriptingSlave::loop()";
		assert(slave);
		slave->loop();
		Debug("slavethread") << "SlaveThread: leaving ScriptingSlave::loop() ";
		delete slave;
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

	struct send_objects {
		Runner& runner;
		send_objects(Runner& r) : runner(r) {}

		void operator()(LayerPtr l) const     { l->forEachObject( send_objects(*this) ); }
		void operator()(ElevationPtr e) const { e->forEachLayer( send_objects(*this) );  }

		void operator()(ObjectPtr object) const {
			if ( !object->isStatic() ) {
				object->set<long>("elevation",object->getElevation());
				object->set<long>("layer",object->getLayerNumber());
				object->set<Point>("position",object->getPosition());
				runner.sendEvent(makeEvent(FIFE_NEW_OBJECT,object));
			}
		}
	};


	void Runner::start() {
		m_slave = new ScriptingSlave();
		setSource(m_slave);
		m_slave->setSource(this);

		sendEvent(makeEvent(FIFE_EXEC,m_ruleset));

		sendEvents();
		m_thread = SDL_CreateThread(Runner::slaveThread, m_slave);

		send_objects send(*this);
		m_map->forEachElevation(send);

		sendEvents();
	}

	void Runner::stop() {
		sendShutdown();
		sendEvents();
		int result;
		SDL_WaitThread(m_thread, &result);
		clearQueue();
		m_thread = 0;
		Log("map_runner")
			<< "Scripting Thread returned " << result;
	}

	void Runner::turn() {
		sendHeartbeat(SDL_GetTicks());
		sendEvents();
		processEvents();
	}

	struct display_objects {
		Runner& runner;
		size_t& nvisuals;
		size_t& nobjects;
		display_objects(Runner& r,size_t& nv,size_t& no)
			: runner(r),nvisuals(nv),nobjects(no) {}

		void operator()(LayerPtr l)      { l->forEachObject( display_objects(*this) ); }

		void operator()(ObjectPtr object)  {
			RenderableLocation loc(object->getVisualLocation());
			size_t iid = ImageCache::instance()->addImageFromLocation(loc);
			if( iid ) {
				Visual* visual = new Visual(object);
				visual->setRenderable(iid, loc.getType());
				object->setVisualId( runner.m_view->addVisual(visual) );
				Debug("map_runner")
					<< "Adding Visual for static object iid:" << iid
					<< " rloc:" << loc.toString();
				++nvisuals;
			}
			++nobjects;
		}

	};

	void Runner::activateElevation(size_t elevation_id) {
		// send the activation command to the ruleset.
		sendNewExecScEvent("ActivateElevation(" +
		                   boost::lexical_cast<std::string>(elevation_id) + ")");

		// display _all_ visuals
		ElevationPtr elevation = m_map->getElevation(elevation_id);
		size_t nv = 0, no = 0;
		display_objects display(*this,nv,no);
		elevation->forEachLayer(display);

		Log("map_runner")
			<< "Displaying " << nv << " visuals from "
			<< no << " objects on elevation " << elevation;
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
