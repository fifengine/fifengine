/***************************************************************************
 *   Copyright (C) 2005 by the FIFE Team                                   *
 *   fife@developer1.cvsdude.com                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
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
#include "console.h"
#include "gui_label.h"

GSTest::GSTest(const std::string &init_with_script) : FIFE::GameState("TestGameState"), m_mapfile(), m_fontfile(), m_map(0)
, m_mapview(new FIFE::MapView()) {
	setFontFile(FIFE::SettingsManager::instance()->read<std::string>("FontFile", "lib/fonts/FreeMono.ttf"));
	m_gui = FIFE::Engine::instance()->getGUI();
	//m_gui->loadStyle("lib/gui/fo2style.xml");

	m_gui->registerWidget("fpslabel", new FIFE::Label());
	m_gui->registerWidget("console", new FIFE::Console());

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
	inputmanager->registerKeyListener(SDLK_s, this);
	inputmanager->registerKeyListener(SDLK_x, this);
	inputmanager->registerKeyListener(SDLK_KP_PLUS, this);
	inputmanager->registerKeyListener(SDLK_KP_MINUS, this);
	//inputmanager->setForcedListener( FIFE::Engine::instance()->getGUI() );
	m_scriptengine = FIFE::ScriptEngine::start(init_with_script);
	m_scriptengine->registerMapViewInstance(m_mapview);
	//m_guisetup = new GUISetup();
	//m_gui->attach("console");
	m_gui->attach("fpslabel");

}


GSTest::~GSTest() {
	FIFE::SettingsManager::instance()->write<int>("RoofAlpha", m_mapview->getRoofAlpha());
	//delete m_guisetup;
	delete m_mapview;
	delete m_map;

	FIFE::Engine* engine = FIFE::Engine::instance();
	FIFE::MapFactory::instance()->cleanup();
	FIFE::SettingsManager::instance()->write<float>("GlobalGammaLevel", engine->getGlobalGamma());
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
	FIFE::ImageCache::instance()->clearCache();
	FIFE::MapFactory* mapfactory = FIFE::MapFactory::instance();
	FIFE::Engine::instance()->setGlobalGamma(FIFE::SettingsManager::instance()->read<float>("GlobalGammaLevel", 1.0));
	m_map = mapfactory->loadFile(m_mapfile);

	if (!m_map) {
		FIFE::Log("gstest") << "couldn't load map: " << m_mapfile;
		exit(1);
	}
	FIFE::Console* console = dynamic_cast<FIFE::Console*>(m_gui->getWidget("console"));
	FIFE::Log::setConsole(console);

	m_mapview->setMap(m_map, 0);
	m_mapview->setViewport(SDL_GetVideoSurface());
	m_mapview->setRoofAlpha(FIFE::SettingsManager::instance()->read<int>("RoofAlpha", 125));

	// should really be in mapview ; maybe boost::signal ?
	m_scriptengine->setActiveMapElevation(0);
}

void GSTest::deactivate() {
	m_mapview->setViewport(0);
	delete m_map;
	m_map = 0;
	//	m_guisetup->detach();
}

void GSTest::turn() {
	m_mapview->render();
	handleFPS();
}

void GSTest::handleKeyEvent(const SDL_KeyboardEvent& ev) {
	if (ev.state != SDL_PRESSED)
		return;

	switch (ev.keysym.sym) {
			case SDLK_UP:
			m_mapview->setYPos(m_mapview->getYPos() - 1);
			break;

			case SDLK_DOWN:
			m_mapview->setYPos(m_mapview->getYPos() + 1);
			break;

			case SDLK_LEFT:
			m_mapview->setXPos(m_mapview->getXPos() - 1);
			break;

			case SDLK_RIGHT:
			m_mapview->setXPos(m_mapview->getXPos() + 1);
			break;

			case SDLK_o:
			m_mapview->setObjectsVisible(!m_mapview->getObjectsVisible());
			break;

			case SDLK_e: {
				size_t elevation = m_mapview->getElevationNum() + 1;
				if (elevation >= m_map->getElevationCount())
					elevation = 0;

				m_mapview->setElevationNum(elevation);
				break;
			}

			case SDLK_F10:
			if (m_gui->isAttached("console"))
				m_gui->detach("console");
			else
				m_gui->attach("console");
			break;

			case SDLK_r:
			m_mapview->setRoofVisible(!m_mapview->getRoofVisible());
			break;

			case SDLK_a:
			m_mapview->setRoofTransparent(!m_mapview->getRoofTransparent());
			break;

			case SDLK_s:
			m_mapview->setRoofAlpha(m_mapview->getRoofAlpha() + 5);
			break;

			case SDLK_x:
			m_mapview->setRoofAlpha(m_mapview->getRoofAlpha() - 5);
			break;

			case SDLK_KP_PLUS:
			FIFE::Engine::instance()->setGlobalGamma(FIFE::Engine::instance()->getGlobalGamma() + 0.05);
			break;

			case SDLK_KP_MINUS:
			FIFE::Engine::instance()->setGlobalGamma(FIFE::Engine::instance()->getGlobalGamma() - 0.05);
			break;

			default:
			break;
	}
}

void GSTest::handleFPS(void) {
	static int counter = -1;
	static Uint32 starttime = SDL_GetTicks();
	static Uint32 time;
	static char text[16];

	++counter;
	time = SDL_GetTicks();

	if (time - starttime > 1000) {
		double fps = counter*(1000/double(time - starttime));
		snprintf(text,16,"FPS: %f",fps);
		FIFE::Label* fpslabel= dynamic_cast<FIFE::Label*>(m_gui->getWidget("fpslabel"));
		if( fpslabel )
			fpslabel->setLabel(text);
		starttime = time;
		counter = 0;
	}
}

