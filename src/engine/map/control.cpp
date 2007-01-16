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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/command/enqueueaction.h"
#include "map/command/setvisual.h"
#include "map/command/startmovement.h"
#include "script/scriptbackendmanager.h"
#include "video/renderbackend.h"
#include "video/rendermanager.h"
#include "exception.h"
#include "log.h"
#include "settingsmanager.h"

#include "camera.h"
#include "control.h"
#include "elevation.h"
#include "factory.h"
#include "map.h"
#include "objectmanager.h"
#include "runner.h"
#include "view.h"

namespace FIFE { namespace map {

	Control::Control() 
		: m_map_filename(""),
		m_map(0),
		m_view(new View()),
		m_runner(new Runner()),
		m_scriptengine(ScriptBackendManager::instance()->current()),
		m_settings(SettingsManager::instance()) {

		m_scriptengine->registerViewInstance(m_view);
		registerCommands();
	}

	Control::~Control() {
		stop();
		m_scriptengine->registerViewInstance(0);
		delete m_view;
		delete m_runner;
	}

	void Control::load(const std::string& filename) {
		m_map_filename = filename;
	}

	void Control::start() {
		stop();

		m_map = Factory::instance()->loadFile(m_map_filename);

		if (!m_map) {
			Log("map_control") << "couldn't load map: " << m_map_filename;
			throw CannotOpenFile(m_map_filename);
		}
		if (m_map->getElevationCount() == 0) {
			Warn("map_control") 
				<< "map: " << m_map_filename << " has no elevations";
			throw CannotOpenFile(m_map_filename);
		}
		
		m_view->setMap(m_map, 0);
		m_view->setViewport(CRenderBackend()->getMainScreen());
// 		m_view->setRoofAlpha(m_settings->read<int>("RoofAlpha", 128));
		
		m_scriptengine->setActiveElevation(0);

		m_om = m_map->getObjectManager();
		m_om->initialize(m_map);

		if (m_map->hasScript(Map::OnStartScript)) {
			Log("map_control") 
				<< "Executing: OnStartScript ";
			m_scriptengine->execute(m_map->getScript(Map::OnStartScript));
		}

		std::string ruleset_file = m_settings->read<std::string>("Ruleset", 
		                           "content/scripts/demos/example_ruleset.lua");
		m_runner->setRuleset(ScriptContainer::fromFile(ruleset_file));
		m_runner->initialize(m_view, m_map, m_om);
		m_runner->start();
		activateElevation(m_map->get<size_t>("default-elevation", 0));

		Point start_pos;
// 		Inactive due to bugs
		if( !m_map->hasAttribute("default-player-position") ) {
			start_pos = m_view->getCurrentElevation()->centerOfMass();
			Log("map_control")
				<< "No default player position found. "
				<< "Using centerOfMass(): " << start_pos;
		}
		start_pos = m_map->get<Point>("default-player-position",start_pos);
		m_view->getCamera()->zoomTo( start_pos );
	}

	void Control::activateElevation(size_t elev) {
		if (m_view->getElevationNum() != elev) {
			m_view->setMap(m_map, elev);
		}
		m_runner->activateElevation(elev);
	}

	void Control::stop() {
		if (m_map) {
			m_runner->stop();
			m_view->reset();
			delete m_map;
			m_map = 0;
		}
	}

	void Control::turn() {
		m_runner->turn();
		m_view->render();
	}

	View* Control::getView() {
		return m_view;
	}

	void Control::registerCommands() {
		m_runner->registerCommand(1, new command::SetVisual());
		m_runner->registerCommand(2, new command::StartMovement());
		m_runner->registerCommand(3, new command::EnqueueAction());
	}

} } // FIFE::map
