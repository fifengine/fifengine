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
#include <guichan.hpp>
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
// FIXME: Do we really have all these dependencies here?
#include "input/manager.h"
#include "vfs/vfs.h"
#include "video/gui/guimanager.h"
#include "video/rendermanager.h"
#include "video/renderbackend.h"
#include "debugutils.h"
#include "engine.h"
#include "events.h"
#include "exception.h"
#include "imagecache.h"
#include "settingsmanager.h"

#include "camera.h"
#include "control.h"
#include "elevation.h"
#include "grid.h"
#include "view.h"
#include "viewgamestate.h"

namespace FIFE { namespace map {
	ViewGameState::ViewGameState() 
		: FIFE::GameState("MapView"),
		m_filename(),
		m_control(new Control()),
		m_map(0),
		m_view(0) {
		m_camera = new Camera(m_control);
		input::Manager::instance()->registerEventListener("map_view", this);
	}

	ViewGameState::~ViewGameState() {
		delete m_control;
		delete m_camera;
	}

	void ViewGameState::setFile(const std::string& map) {
		m_filename = map;
	}

	void ViewGameState::activate() {
		try {
			m_control->load(m_filename);
			m_control->start();
		} catch( CannotOpenFile& e ) {
			Warn("view_state")
				<< "Couldn't open " << m_filename 
				<< " (" << e.getMessage() << ")";
			m_valid_map = false;
			return;
		}
		m_valid_map = true;

		m_view = m_control->getView();
		m_camera->zoomTo( m_view->getCurrentElevation()->get<Point>("default-player-position") );

		input::Manager* inputmanager = input::Manager::instance();
		inputmanager->pushContext("map_view");
		inputmanager->registerMouseButtonListener(this);

	}

	void ViewGameState::deactivate() {
		if(!m_valid_map)
			return;
		input::Manager* inputmanager = input::Manager::instance();
		inputmanager->popContext("map_view");
		inputmanager->unregisterMouseButtonListener(this);
		m_control->stop();
	}

	void ViewGameState::turn() {
		if(m_valid_map) {
			m_control->turn();
			m_camera->render();
		}
	}

	void ViewGameState::handleEvent(int event) {
		if(!m_valid_map)
			return;

		FIFE::map::Elevation* elevation = m_view->getCurrentElevation();
		FIFE::map::Grid *obj_grid=0, *floor_grid=0, *roof_grid=0;

		// FIXME Hardcoded map structure
		if( elevation->getNumGrids() > 0)
			floor_grid = elevation->getGrid(0);
		if( elevation->getNumGrids() > 1)
			obj_grid   = elevation->getGrid(1);
		if( elevation->getNumGrids() > 2)
			roof_grid  = elevation->getGrid(2);

		switch (event) {
			case FIFE::Event::GO_NORTH:
				m_camera->moveBy(Point(0,-20));
				break;
		
			case FIFE::Event::GO_SOUTH:
				m_camera->moveBy(Point(0,+20));
				break;
		
			case FIFE::Event::GO_EAST:
				m_camera->moveBy(Point(-20,0));
				break;
		
			case FIFE::Event::GO_WEST:
				m_camera->moveBy(Point(+20,0));
				break;

			case FIFE::Event::TEST_TOGGLE_OBJECTS:
				if(obj_grid)
					obj_grid->toggleObjectsVisible();
				break;
	
			case FIFE::Event::TEST_CHANGE_ELEVATION:
			{
				size_t elevation = m_view->getElevationNum() + 1;
				if (elevation >= m_view->getElevationCount())
					elevation = 0;
	
				m_control->activateElevation(elevation);
				break;
			}
	
			case FIFE::Event::TEST_TOGGLE_ROOF:
				if(roof_grid)
					roof_grid->toggleTilesVisible();
				break;
				
			case FIFE::Event::TEST_TOGGLE_TILE_GRID:
				if(floor_grid)
					floor_grid->toggleGridOverlayEnabled();
				break;
				
			case FIFE::Event::TEST_TOGGLE_OBJ_GRID:
				if(obj_grid)
					obj_grid->toggleGridOverlayEnabled();
				break;

			case FIFE::Event::TEST_INCREASE_ALPHA:
				if(roof_grid)
					roof_grid->addToGlobalAlpha(5);
				break;
		
			case FIFE::Event::TEST_DECREASE_ALPHA:
				if(roof_grid)
					roof_grid->addToGlobalAlpha(-5);
				break;
	
			default:
				break;
		}
	}

  void ViewGameState::handleMouseButtonEvent(const SDL_MouseButtonEvent& event) {
    m_view->onMouseClick(event.type, event.button, event.x, event.y);
  }
} } // FIFE::map
