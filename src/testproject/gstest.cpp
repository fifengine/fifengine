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

#include "gstest.h"
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
#include "object.h"

#include "video/gui/guimanager.h"
#include "video/rendermanager.h"
#include "video/renderbackend.h"
#include "guichan_addon/console.h"

#include <guichan.hpp>

#include <iostream>

GSTest::GSTest(const std::string &init_with_script) : FIFE::GameState("TestGameState"), m_mapfile(), m_fontfile(), m_map(0),
m_mapview(new FIFE::MapView()), m_fpslabel(0) {
	setFontFile(FIFE::SettingsManager::instance()->read<std::string>("FontFile", "lib/fonts/FreeMono.ttf"));
	//m_gui->loadStyle("lib/gui/fo2style.xml");

	FIFE::InputManager* inputmanager = FIFE::InputManager::instance();
	inputmanager->registerKeyListener(SDLK_UP, this);
	inputmanager->registerKeyListener(SDLK_DOWN, this);
	inputmanager->registerKeyListener(SDLK_LEFT, this);
	inputmanager->registerKeyListener(SDLK_RIGHT, this);
	// I need the keys for the console, commented out for the moment
	// it would work, but is really strange while typing commands

	inputmanager->registerKeyListener(SDLK_r, this);
	inputmanager->registerKeyListener(SDLK_e, this);
	inputmanager->registerKeyListener(SDLK_o, this);
	inputmanager->registerKeyListener(SDLK_F10, this);

	inputmanager->registerKeyListener(SDLK_a, this);
	inputmanager->registerKeyListener(SDLK_z, this);
	inputmanager->registerKeyListener(SDLK_s, this);
	inputmanager->registerKeyListener(SDLK_x, this);
	inputmanager->registerKeyListener(SDLK_KP_PLUS, this);
	inputmanager->registerKeyListener(SDLK_KP_MINUS, this);
	inputmanager->registerMouseButtonListener(this);
	//inputmanager->setForcedListener( FIFE::Engine::instance()->getGUI() );

	m_mapview->loadOverlayImage("art/intrface/msef000.frm");

	m_scriptengine = FIFE::ScriptEngine::start(init_with_script);
	m_scriptengine->registerMapViewInstance(m_mapview);
	//m_guisetup = new GUISetup();
	//m_gui->attach("console");
}


GSTest::~GSTest() {
///@todo: kefrens: obsolete, roofs have individual alphas, @see MapTile::m_roofAlpha;
//	FIFE::SettingsManager::instance()->write<int>("RoofAlpha", m_mapview->getRoofAlpha());
	delete m_fpslabel;
	delete m_mapview;
	delete m_map;

	FIFE::MapFactory::instance()->cleanup();
}

GSTest::GSTest(const GSTest&) : FIFE::GameState("TestGameState"), m_mapfile(), m_fontfile()
, m_map(0), m_mapview(new FIFE::MapView()) {}

GSTest& GSTest::operator=(const GSTest&) {
	return *this;
}

void GSTest::setMapFile(const std::string& map) {
	m_mapfile = map;
	m_scriptengine->registerMapViewInstance(m_mapview);
}

void GSTest::setFontFile(const std::string& map) {
	m_fontfile = map;
}

void GSTest::activate() {
	m_map = FIFE::MapFactory::instance()->loadFile(m_mapfile);

	FIFE::GUIManager* guimanager = FIFE::GUIManager::instance();

	m_fpslabel = new gcn::Label("fps counter                     ");
	m_fpslabel->setPosition(0, 0);
	guimanager->add(m_fpslabel);

	if (!m_map) {
		FIFE::Log("gstest") << "couldn't load map: " << m_mapfile;
		exit(1);
	}

	m_mapview->setMap(m_map, 0);
	m_mapview->setViewport(FIFE::RenderManager::instance()->getCurrentBackend()->getMainScreen());
	m_mapview->setRoofAlpha(FIFE::SettingsManager::instance()->read<int>("RoofAlpha", 125));

	// should really be in mapview ; maybe boost::signal ?
	m_scriptengine->setActiveMapElevation(0);

// 	FIFE::VFS::type_stringlist list = FIFE::VFS::instance()->listFiles("maps/", ".*\\.map");
// 	FIFE::VFS::type_stringlist list = FIFE::VFS::instance()->list("./proto/");
// 	FIFE::VFS::type_stringlist::const_iterator end = list.end();
// 	for (FIFE::VFS::type_stringlist::const_iterator i = list.begin(); i != end; ++i)
// 		FIFE::Log() << *i;
}

void GSTest::deactivate() {
	delete m_fpslabel;
	m_fpslabel = 0;
	m_mapview->setViewport(0);
	delete m_map;
	m_map = 0;
}

void GSTest::turn() {
	m_mapview->render();

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
	}
}

void GSTest::handleKeyEvent(const SDL_KeyboardEvent& ev) {
	if (ev.state != SDL_PRESSED)
		return;

	switch (ev.keysym.sym) {
			case SDLK_UP:
			m_mapview->setYPos(m_mapview->getYPos() - 20);
			break;

			case SDLK_DOWN:
			m_mapview->setYPos(m_mapview->getYPos() + 20);
			break;

			case SDLK_LEFT:
			m_mapview->setXPos(m_mapview->getXPos() - 20);
			break;

			case SDLK_RIGHT:
			m_mapview->setXPos(m_mapview->getXPos() + 20);
			break;

      case SDLK_F10:
        FIFE::GUIManager::instance()->getConsole()->toggleShowHide();
        break;

			case SDLK_o:
				{
					m_mapview->setObjectsVisible(!m_mapview->getObjectsVisible());

					FIFE::MapElevation* elevation = m_mapview->getCurrentElevation();
					const FIFE::MapElevation::type_objects& objects = elevation->getObjects();
					FIFE::MapElevation::type_objects::const_iterator end = objects.end();
					for (FIFE::MapElevation::type_objects::const_iterator i = objects.begin(); i != end; ++i)
						{
							FIFE::Object* obj = *i;
							obj->m_opacity = 255;
						}
				}
			break;

			case SDLK_e: {
				size_t elevation = m_mapview->getElevationNum() + 1;
				if (elevation >= m_map->getElevationCount())
					elevation = 0;

				m_mapview->setElevationNum(elevation);
				break;
			}


			case SDLK_r:
				if( m_mapview->getRoofVisible() )
					{
						m_mapview->setRoofAlpha( 255 );
						m_mapview->setRoofVisible( false );
					}
				else
					{
						m_mapview->setRoofAlpha( 255 );
						m_mapview->setRoofVisible( true );
					}
			break;

			case SDLK_a:
				{
					FIFE::MapElevation* elevation = m_mapview->getCurrentElevation();
					const FIFE::MapElevation::type_objects& objects = elevation->getObjects();
					FIFE::MapElevation::type_objects::const_iterator end = objects.end();
					for (FIFE::MapElevation::type_objects::const_iterator i = objects.begin(); i != end; ++i)
						{
							FIFE::Object* obj = *i;
							obj->m_opacity = std::min(255, std::max( 0, obj->m_opacity + 5 ) );
						}
				}
			break;

			case SDLK_z:
				{
					FIFE::MapElevation* elevation = m_mapview->getCurrentElevation();
					const FIFE::MapElevation::type_objects& objects = elevation->getObjects();
					FIFE::MapElevation::type_objects::const_iterator end = objects.end();
					for (FIFE::MapElevation::type_objects::const_iterator i = objects.begin(); i != end; ++i)
						{
							FIFE::Object* obj = *i;
							obj->m_opacity = std::min(255, std::max( 0, obj->m_opacity - 5 ) );
						}
				}
			break;

			case SDLK_s:
				m_mapview->applyRoofAlpha( 5 );
			break;

			case SDLK_x:
				m_mapview->applyRoofAlpha( -5 );
			break;

			default:
			break;
	}
}

void GSTest::handleMouseButtonEvent(const SDL_MouseButtonEvent& event) {
	m_mapview->onMouseClick(event.type, event.button, event.x, event.y);
}
