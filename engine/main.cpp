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
#include <iostream>
#include <string>

// 3rd party library includes
#include <guichan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "audio/audiomanager.h"
#include "eventchannel/manager/eventmanager.h"
#include "map/viewgamestate.h"
#include "map/objectinfo.h"
#include "script/luascript.h"
#include "video/gui/guimanager.h"
#include "video/rendermanager.h"
#include "video/renderbackend.h"
#include "util/gamestate/gamestatemanager.h"
#include "util/debugutils.h"
#include "util/exception.h"
#include "util/settingsmanager.h"

#include "engine.h"

#ifdef HAVE_MOVIE
#include "video/movie/video_gamestate.h"
#endif

void initScripts(FIFE::Engine* engine) {
	std::string startup_script("content/scripts/startup.lua");
	std::string script_setup("content/scripts/fallout2.lua");
	std::string map_to_load("");

	FIFE::SettingsManager* settings = FIFE::SettingsManager::instance();

	unsigned int swidth = settings->read("ScreenWidth", 800);
	unsigned int sheight = settings->read("ScreenHeight", 600);

	FIFE::LuaScript::instance()->runFile(startup_script);

	const std::vector<std::string>& scripts = engine->getCommandLine("-script");
	if( !scripts.empty() ) {
		for(size_t i=0; i!=scripts.size(); ++i) {
			FIFE::LuaScript::instance()->runFile(scripts[i]);
		}
	} else {
		if( !engine->getCommandLine().empty() 
			&& !engine->getCommandLine().front().empty()
			&& engine->getCommandLine().front()[0] != '-' ) {
			FIFE::LuaScript::instance()->runFile( engine->getCommandLine().front());
		} else {
			FIFE::LuaScript::instance()->runFile(script_setup);
		}
	}
	const std::vector<std::string>& maps = engine->getCommandLine("-map");
	if( !maps.empty() ) {
		map_to_load = maps.front();
	}

	if (map_to_load.size() > 0)
		FIFE::LuaScript::instance()->setGlobalString("_tmp_map_to_load", map_to_load);

	FIFE::LuaScript::instance()->setGlobalInt("screen_width", swidth);
	FIFE::LuaScript::instance()->setGlobalInt("screen_height", sheight);

}

int main(int argc, char* argv[]) {

	
	FIFE::map::ViewGameState * mapview = NULL;

#ifdef HAVE_MOVIE
	FIFE::VideoGameState * video_view = NULL;
#endif

	FIFE::Engine* engine = NULL;
	try {
		engine = new FIFE::Engine(argc, argv);

		FIFE::SettingsManager* settings = FIFE::SettingsManager::instance();

		unsigned int swidth = settings->read("ScreenWidth", 800);
		unsigned int sheight = settings->read("ScreenHeight", 600);
		unsigned char bitsPerPixel = settings->read("BitsPerPixel", 0);
		bool fullscreen = settings->read("FullScreen", false);

		FIFE::CRenderBackend()->createMainScreen(swidth, sheight, bitsPerPixel, fullscreen);

		// it seems this *MUST* be done after setting the video mode!
		SDL_EnableUNICODE(1);
		initScripts(engine);

		FIFE::EventManager& em = *FIFE::EventManager::instance();
		// construct a mapview-gamestate; inactive by default
		mapview = new FIFE::map::ViewGameState(em, em, em, em, em);
		mapview->getMap(); // just to suppress the 'unused variable warning'

#ifdef HAVE_MOVIE
		video_view = new FIFE::VideoGameState();
#endif

		// Play music in the background
		FIFE::audio::Manager::instance()->setAmbientSound(settings->read<std::string>("BackgroundMusic", "content/audio/music/maybe.ogg"));

	} catch (const FIFE::Exception& exception) {
		std::cerr << "fife:exception during startup: " << exception.getMessage() << std::endl;
		delete mapview;
		delete engine;
		return 1;
	} catch (const gcn::Exception& exception) {
		std::cerr << "gcn:exception during startup: " << exception.getMessage() << std::endl;
		delete mapview;
		delete engine;
		return 1;
	}

	try {
		engine->start();
	} catch (const FIFE::Exception & exception) {
		std::cerr << "uncaught fife:exception during game: " << exception.getMessage() << std::endl;
		delete mapview;
		delete engine;
		return 1;
	}
	catch (const gcn::Exception& exception) {
		std::cerr << "uncaught gcn:exception during game: " << exception.getMessage() << std::endl;
		delete mapview;
		delete engine;
		return 1;
	}
	delete mapview;

#ifdef HAVE_MOVIE
	delete video_view;
#endif

	delete engine;

	FIFE::Log("guimap_test") << "map objects left alive: " << FIFE::map::ObjectInfo::globalCount();
	//FIXME: something about saving settings ?

	return 0;
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
