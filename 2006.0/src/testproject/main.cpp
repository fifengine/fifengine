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
#include "engine.h"
#include "exception.h"
#include "gamestatemanager.h"
#include "mapfactory.h"
#include "gstest.h"
#include "settingsmanager.h"
#include "debugutils.h"
#include "audiomanager.h"

#include <iostream>
#include <string>

static void printusage(const std::string& execname) {
	std::cout << "usage: \n";
	std::cout << execname << " <mapfile.map>" << std::endl;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printusage(argv[0]);
		return 1;
	}

	FIFE::Log::parseCmdLine(argc, argv);

	try {
		FIFE::Engine* engine = FIFE::Engine::instance();
		FIFE::GameStateManager* gsmanager = FIFE::GameStateManager::instance();
		FIFE::SettingsManager* settings = FIFE::SettingsManager::instance();

		unsigned int swidth = settings->read("ScreenWidth", 800);
		unsigned int sheight = settings->read("ScreenHeight", 600);
		unsigned int bpp = settings->read("BitsPerPixel", 32);/*videoInfo->vfmt->BitsPerPixel);*/
		bool fullscreen = settings->read("FullScreen", false);
		bool opengl = settings->read("OpenGL", false);
		bool doublebuf = settings->read("DoubleBuffer", true);
		bool main_is_hwsurface = settings->read("PreferHWSurface", false);

		engine->createMainScreen(swidth, sheight, bpp, fullscreen, main_is_hwsurface, opengl, doublebuf);

    GSTest* gs = NULL;
    // the scriptfilename may NOT be NULL, but an empty string is ok!
    if (argc == 3) 
      gs = new GSTest(argv[2]);
    else
      gs = new GSTest("");
		gs->setMapFile(argv[1]);
		gsmanager->activate("TestGameState");
		//gs->setFontFile(settings->read<std::string>("FontFile", "lib/fonts/FreeMono.ttf"));

		try {
			FIFE::AudioManager::instance()->setAmbientSound(settings->read<std::string>("BackgroundMusic", "music.ogg"));
		} catch (const FIFE::Exception &exception) {
			std::cerr << exception.getMessage() << std::endl;
		}

		engine->start();
		delete gs;
		engine->quit();

	} catch (const FIFE::Exception& exception) {
		std::cerr << exception.getMessage() << std::endl;
		return 1;
	}


	return 0;
}
