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
#include "map/factory.h"
#include "video/rendermanager.h"
#include "video/renderbackend.h"
#include "writeformats/mapdump.h"
#include "debugutils.h"
#include "engine.h"
#include "exception.h"
#include "gamestatemanager.h"
#include "settingsmanager.h"

#include "gstest.h"

static void printusage(const std::string& execname) {
	std::cout << "usage: \n";
	std::cout << execname << " <mapfile.map>" << std::endl;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printusage(argv[0]);
		return 1;
	}

	try {
		FIFE::Engine engine(argc, argv);
		FIFE::VFS::instance()->addFormat(new FIFE::MapDump());		

		FIFE::GameStateManager* gsmanager = FIFE::GameStateManager::instance();
		FIFE::SettingsManager* settings = FIFE::SettingsManager::instance();

		unsigned int swidth = settings->read("ScreenWidth", 1024);
		unsigned int sheight = settings->read("ScreenHeight", 768);
		unsigned char bitsPerPixel = settings->read("BitsPerPixel", 0);
		bool fullscreen = settings->read("FullScreen", false);

		FIFE::CRenderBackend()->createMainScreen(swidth, sheight, bitsPerPixel, fullscreen);

		// it seems this *MUST* be done after setting the video mode!
		SDL_EnableUNICODE(1);

		GSTest* gs = NULL;
		// the scriptfilename may NOT be NULL, but an empty string is ok!
		if (argc == 3)
			gs = new GSTest(argv[2]);
		else
			gs = new GSTest("");
		gs->setMapFile(argv[1]);
		gsmanager->activate("TestGameState");
		gs->writeToDisk();		

		delete gs;

	} catch (const FIFE::Exception& exception) {
		std::cerr << exception.getMessage() << std::endl;
		return 1;
	} catch (const gcn::Exception& exception) {
		std::cerr << exception.getMessage() << std::endl;
	}


	return 0;
}
