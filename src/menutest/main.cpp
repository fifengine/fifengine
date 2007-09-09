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

#include "engine.h"
#include "exception.h"
#include "gamestatemanager.h"
#include "menutest.h"
#include "settingsmanager.h"
#include "debugutils.h"
#include "audiomanager.h"

#include "video/rendermanager.h"
#include "video/renderbackend.h"

#include <guichan.hpp>

#include <iostream>
#include <string>

static void printusage(const std::string& execname) {
	std::cout << "usage: \n";
	std::cout << execname << " menu_script.lua" << std::endl;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printusage(argv[0]);
		return 1;
	}


	try {
		FIFE::Engine* engine = FIFE::Engine::instance();
		FIFE::Log::parseCmdLine(argc, argv);
		FIFE::GameStateManager* gsmanager = FIFE::GameStateManager::instance();
		FIFE::SettingsManager* settings = FIFE::SettingsManager::instance();

		unsigned int swidth = settings->read("ScreenWidth", 800);
		unsigned int sheight = settings->read("ScreenHeight", 600);
		unsigned char bitsPerPixel = settings->read("BitsPerPixel", 0);
		bool fullscreen = settings->read("FullScreen", false);

		FIFE::RenderManager::instance()->getCurrentBackend()->createMainScreen(swidth, sheight, bitsPerPixel, fullscreen);

		MenuTest* gs = NULL;
		// the scriptfilename may NOT be NULL, but an empty string is ok!
		if (argc == 2)
			gs = new MenuTest(argv[1]);
		else
			gs = new MenuTest("");

		FIFE::ScriptEngine *se = FIFE::ScriptEngine::instance();
		se->setGlobalInt("screen_width", swidth);
		se->setGlobalInt("screen_height", sheight);
		gsmanager->activate("MenuGameState");

		engine->start();
		delete gs;

	} catch (const FIFE::Exception& exception) {
		std::cerr << exception.getMessage() << std::endl;
		return 1;
	} catch (const gcn::Exception& exception) {
		std::cerr << exception.getMessage() << std::endl;
	}


	return 0;
}
