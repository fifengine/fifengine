/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#include "mapviewgamestate.h"
#include <SDL.h>

#include "mapfactory.h"
#include "map.h"
#include "mapelevation.h"
#include "maptile.h"
#include "debugutils.h"
#include "imagecache.h"
#include "inputmanager.h"
#include "mapview.h"
#include "engine.h"
#include "settingsmanager.h"
#include "vfs.h"

#include "video/gui/guimanager.h"
#include "video/rendermanager.h"
#include "video/renderbackend.h"

#include <guichan.hpp>
#include "events.h"

namespace FIFE {

MapViewGameState::MapViewGameState() : FIFE::GameState("MapView"), m_mapfile(), m_map(0),
										m_mapview(new FIFE::MapView()) {

	FIFE::InputManager* inputmanager = FIFE::InputManager::instance();
	inputmanager->registerEventListener("map_view",this);
	// FIXME Load Key Bindings someplace else! Done!
 	/*inputmanager->mapKeyToEvent("map_view",SDLK_UP, FIFE::Event::GO_NORTH);
 	inputmanager->mapKeyToEvent("map_view",SDLK_DOWN, FIFE::Event::GO_SOUTH);
 	inputmanager->mapKeyToEvent("map_view",SDLK_LEFT, FIFE::Event::GO_EAST);
 	inputmanager->mapKeyToEvent("map_view",SDLK_RIGHT, FIFE::Event::GO_WEST);*/

// 	inputmanager->registerKeyListener(SDLK_DOWN, this);
// 	inputmanager->registerKeyListener(SDLK_LEFT, this);
// 	inputmanager->registerKeyListener(SDLK_RIGHT, this);
// 
// 	inputmanager->registerKeyListener(SDLK_r, this);
// 	inputmanager->registerKeyListener(SDLK_e, this);
// 	inputmanager->registerKeyListener(SDLK_o, this);
// 	inputmanager->registerKeyListener(SDLK_F10, this);
// 
// 	inputmanager->registerKeyListener(SDLK_a, this);
// 	inputmanager->registerKeyListener(SDLK_s, this);
// 	inputmanager->registerKeyListener(SDLK_x, this);
// 	inputmanager->registerKeyListener(SDLK_KP_PLUS, this);
// 	inputmanager->registerKeyListener(SDLK_KP_MINUS, this);
	inputmanager->registerMouseButtonListener(this);
	//inputmanager->setForcedListener( FIFE::Engine::instance()->getGUI() );

	m_mapview->loadOverlayImage("art/intrface/msef000.frm");

	m_scriptengine = FIFE::ScriptEngine::instance();
	m_scriptengine->registerMapViewInstance(m_mapview);
	//m_guisetup = new GUISetup();
	//m_gui->attach("console");
}


  MapViewGameState::~MapViewGameState() {
    // disabled: there should be a central place for this; not scattered around everywhere
    //FIFE::SettingsManager::instance()->write<int>("RoofAlpha", m_mapview->getRoofAlpha());
    delete m_mapview;
    delete m_map;

    FIFE::MapFactory::instance()->cleanup();
  }

  MapViewGameState::MapViewGameState(const MapViewGameState&) : FIFE::GameState("MapView"), m_mapfile(),
                                                                m_map(0), m_mapview(new FIFE::MapView()) {}

  MapViewGameState& MapViewGameState::operator=(const MapViewGameState&) {
    return *this;
  }

  void MapViewGameState::setMapFile(const std::string& map) {
    m_mapfile = map;
    //m_scriptengine->registerMapViewInstance(m_mapview);
  }

	void MapViewGameState::activate() {
		m_map = FIFE::MapFactory::instance()->loadFile(m_mapfile);
		FIFE::InputManager* inputmanager = FIFE::InputManager::instance();
		inputmanager->pushContext("map_view");

		//FIFE::GUIManager* guimanager = FIFE::GUIManager::instance();
		
		/*
			m_fpslabel = new gcn::Label("fps counter                     ");
			m_fpslabel->setPosition(0, 0);
			guimanager->add(m_fpslabel);
			*/
		
		if (!m_map) {
			FIFE::Log("MapViewGameState") << "couldn't load map: " << m_mapfile;
			exit(1);
		}
		
		m_mapview->setMap(m_map, 0);
		m_mapview->setViewport(FIFE::RenderManager::instance()->getCurrentBackend()->getMainScreen());
		m_mapview->setRoofAlpha(FIFE::SettingsManager::instance()->read<int>("RoofAlpha", 128));
		
		// should really be in mapview ; maybe boost::signal ?
		m_scriptengine->setActiveMapElevation(0);

		if( m_map->hasScript(Map::OnMapStartScript) ) {
			Log("mapviewgamestate") 
				<< "Executing: OnMapStartScript ";
			m_scriptengine->execute( m_map->getScript(Map::OnMapStartScript) );
		}
	}

  void MapViewGameState::deactivate() {
	FIFE::InputManager* inputmanager = FIFE::InputManager::instance();
	inputmanager->popContext("map_view");
    m_mapview->setViewport(0);
    delete m_map;
    m_map = 0;
  }

  void MapViewGameState::turn() {
    m_mapview->render();

    /*
       static int counter = -1;
       static Uint32 starttime = SDL_GetTicks();
       static Uint32 time;

       ++counter;
       time = SDL_GetTicks();

       if (time - starttime > 1000) {
       double fps = counter*(1000/double(time - starttime));
       m_fpslabel->setCaption("fps: " + boost::lexical_cast<std::string>(fps));
       starttime = time;
       counter = 0;
       }*/
  }

	void MapViewGameState::handleEvent(int event) {
		/*
		if (ev.state != SDL_PRESSED)
			return;
		*/
		
		switch (event) {
			case FIFE::Event::GO_NORTH:
				m_mapview->setYPos(m_mapview->getYPos() - 20);
				break;
		
			case FIFE::Event::GO_SOUTH:
				m_mapview->setYPos(m_mapview->getYPos() + 20);
				break;
		
			case FIFE::Event::GO_EAST:
				m_mapview->setXPos(m_mapview->getXPos() - 20);
				break;
		
			case FIFE::Event::GO_WEST:
				m_mapview->setXPos(m_mapview->getXPos() + 20);
				break;
	
			case FIFE::Event::TEST_TOGGLE_OBJECTS:
				m_mapview->setObjectsVisible(!m_mapview->getObjectsVisible());
				break;
	
			case FIFE::Event::TEST_CHANGE_ELEVATION:
			{
				size_t elevation = m_mapview->getElevationNum() + 1;
				if (elevation >= m_map->getElevationCount())
				elevation = 0;
	
				m_mapview->setElevationNum(elevation);
				break;
			}
	
			case FIFE::Event::TEST_TOGGLE_ROOF:
				m_mapview->setRoofVisible(!m_mapview->getRoofVisible());
				break;
			case FIFE::Event::TEST_TOGGLE_TILE_GRID:
				m_mapview->setTileGridVisible(!m_mapview->getTileGridVisible());
				break;
			case FIFE::Event::TEST_TOGGLE_OBJ_GRID:
				m_mapview->setObjectGridVisible(!m_mapview->getObjectGridVisible());
				break;
	/*
		case SDLK_a:
					m_mapview->setRoofTransparent(!m_mapview->getRoofTransparent());
					break;
	*/
			case FIFE::Event::TEST_INCREASE_ALPHA:
				m_mapview->applyRoofAlpha( 5 );
				break;
		
			case FIFE::Event::TEST_DECREASE_ALPHA:
					m_mapview->applyRoofAlpha( -5 );
					break;
	
			default:
					break;
		}
	
	}

  void MapViewGameState::handleMouseButtonEvent(const SDL_MouseButtonEvent& event) {
    m_mapview->onMouseClick(event.type, event.button, event.x, event.y);
  }

}
