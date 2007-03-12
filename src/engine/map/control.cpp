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
		m_map(),
		m_view(new View()),
		m_runner(new Runner()),
		m_scriptengine(ScriptBackendManager::instance()->current()),
		m_settings(SettingsManager::instance()) {

		registerCommands();
	}

	Control::~Control() {
		stop();

		// Real cleanup after 'stop()'
		std::set<Camera*>::iterator i(m_cameras.begin());
		for(; i != m_cameras.end(); ++i)
			(*i)->controlDeleted();

		delete m_view;
		delete m_runner;
	}

	void Control::load(const std::string& filename) {
		m_map_filename = filename;

		MapPtr map(Factory::instance()->loadMap(m_map_filename));
		if (!map) {
			Log("map_control") << "couldn't load map: " << m_map_filename;
			throw CannotOpenFile(m_map_filename);
		}

		setMap(map);
	}

	void Control::setMap(MapPtr map) {
		stop();
		m_map = map;
		if (m_map->getElevationCount() == 0) {
			Warn("map_control") 
				<< "map: " << m_map_filename << " has no elevations";
			throw CannotOpenFile(m_map_filename);
		}
	}

	void Control::start() {
		m_view->setMap(m_map, 0);
		m_view->setViewport(CRenderBackend()->getMainScreen());
// 		m_view->setRoofAlpha(m_settings->read<int>("RoofAlpha", 128));
		
		if (m_map->hasScript(Map::OnStartScript)) {
			Log("map_control") 
				<< "Executing: OnStartScript ";
			m_scriptengine->execute(m_map->getScript(Map::OnStartScript));
		}

		std::string ruleset_file = m_settings->read<std::string>("Ruleset", 
		                           "content/scripts/demos/example_ruleset.lua");
		m_runner->setRuleset(ScriptContainer::fromFile(ruleset_file));
		m_runner->initialize(m_map, m_view);
		m_runner->start();
		activateElevation(m_map->get<size_t>("_START_ELEVATION", 0));
	}

	void Control::activateElevation(size_t elev) {
		m_view->setMap(m_map, elev);
		m_runner->activateElevation(elev);

		// Assure a default starting position is set
		ElevationPtr current_elevation = m_view->getCurrentElevation();
		if( !current_elevation->hasAttribute("_START_POSITION") ) {
			Point start_pos = current_elevation->centerOfMass();
			Log("map_control")
				<< "No start position found. "
				<< "Using centerOfMass(): " << start_pos;
			current_elevation->set<Point>("_START_POSITION",start_pos);
		}

		// Assure camera(s) are aware of the change.
		resetCameras();
	}

	void Control::stop() {
		if (m_map) {
			m_runner->stop();
			resetCameras();
			m_view->reset();
			m_map.reset();
		}
	}

	bool Control::isRunning() const {
		return m_map;
	}

	void Control::turn() {
		if( isRunning() ) {
			m_runner->turn();
		}
	}

	View* Control::getView() {
		return m_view;
	}

	void Control::addCamera(Camera* camera) {
		m_cameras.insert(camera);
	}

	void Control::removeCamera(Camera* camera) {
		m_cameras.erase(camera);
	}

	void Control::registerCommands() {
		m_runner->registerCommand(1, new command::SetVisual());
		m_runner->registerCommand(2, new command::StartMovement());
		m_runner->registerCommand(3, new command::EnqueueAction());
	}

	void Control::resetCameras() {
		std::set<Camera*>::iterator i(m_cameras.begin());
		for(; i != m_cameras.end(); ++i)
			(*i)->reset();
	}

} } // FIFE::map
