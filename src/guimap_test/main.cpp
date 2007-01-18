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

// Standard C++ library includes
#include <iostream>
#include <string>

// 3rd party library includes
#include <guichan.hpp>

// FIFE includes
#include "audio/manager.h"
#include "map/viewgamestate.h"
#include "script/scriptbackendmanager.h"
#include "video/gui/guimanager.h"
#include "video/rendermanager.h"
#include "video/renderbackend.h"

#include "engine.h"
#include "debugutils.h"
#include "exception.h"
#include "gamestatemanager.h"
#include "settingsmanager.h"

int main(int argc, char* argv[]) {

	std::string startup_script("content/scripts/startup.lua");
	std::string script_setup("content/scripts/fallout2.lua");
	std::string map_to_load("");
	
	if (argc == 2) {
		script_setup = argv[1];
	}
	if (argc == 3) {
		if (std::string(argv[1]) == "-map")
			map_to_load = argv[2];
	}

	FIFE::map::ViewGameState * mapview = NULL;
	FIFE::Engine* engine = NULL;
	try {
		engine = new FIFE::Engine(argc, argv);

		FIFE::GameStateManager::instance();
		FIFE::SettingsManager* settings = FIFE::SettingsManager::instance();

		unsigned int swidth = settings->read("ScreenWidth", 800);
		unsigned int sheight = settings->read("ScreenHeight", 600);
		unsigned char bitsPerPixel = settings->read("BitsPerPixel", 0);
		bool fullscreen = settings->read("FullScreen", false);

		FIFE::CRenderBackend()->createMainScreen(swidth, sheight, bitsPerPixel, fullscreen);

		// it seems this *MUST* be done after setting the video mode!
		SDL_EnableUNICODE(1);

		FIFE::audio::Manager::instance();
		FIFE::GUIManager::instance();

		FIFE::ScriptBackendManager::instance()->select("Lua");
		FIFE::CScriptEngine()->runFile(startup_script);
		FIFE::CScriptEngine()->runFile(script_setup);
		FIFE::CScriptEngine()->setGlobalInt("screen_width", swidth);
		FIFE::CScriptEngine()->setGlobalInt("screen_height", sheight);
		if (map_to_load.size() > 0)
			FIFE::CScriptEngine()->setGlobalString("_tmp_map_to_load", map_to_load);

		// construct a mapview-gamestate; inactive by default
		mapview = new FIFE::map::ViewGameState();
		mapview->getMap(); // just to suppress the 'unused variable warning'

		// Play music in the background
		FIFE::audio::Manager::instance()->setAmbientSound(settings->read<std::string>("BackgroundMusic", "music.ogg"));

	} catch (const FIFE::Exception& exception) {
		std::cerr << "fife:exception during startup: " << exception.getMessage() << std::endl;
		return 1;
	} catch (const gcn::Exception& exception) {
		std::cerr << "gcn:exception during startup: " << exception.getMessage() << std::endl;
		return 1;
	}

	try {
		engine->start();
	} catch (const FIFE::Exception & exception) {
		std::cerr << "uncaught fife:exception during game: " << exception.getMessage() << std::endl;
		return 1;
	}
	catch (const gcn::Exception& exception) {
		std::cerr << "uncaught gcn:exception during game: " << exception.getMessage() << std::endl;
		return 1;
	}
	delete mapview;
	delete engine;
	//FIXME: something about saving settings ?

	return 0;
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
