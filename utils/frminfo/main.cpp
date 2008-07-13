/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

// Standard C++ library includes
#include <iostream>
#include <string>
#include <vector>

// Platform dependent includes
#ifndef __WIN32
#include <libgen.h>
#else
#include <getopt.h>
#endif

// 3rd party library includes
#include <SDL.h>

// FIFE includes
#include "map/loaders/fallout/animatedpal.h"
#include "map/loaders/fallout/critlst.h"
#include "map/loaders/fallout/dat2.h"
#include "map/loaders/fallout/frm.h"
#include "map/loaders/fallout/list.h"
#include "map/factory.h"
#include "vfs/vfshostsystem.h"
#include "vfs/vfssourcefactory.h"
#include "video/renderbackends/sdl/sdlimage.h"
#include "video/gui/guimanager.h"
#include "video/renderbackend.h"
#include "video/rendermanager.h"
#include "engine.h"
#include "util/base/exception.h"
#include "util/settingsmanager.h"

#include "frm_raw_loader.h"

/* Function that compensate for missing libgen.h on Windows */
#ifdef __WIN32
static char *dirname(char *in) {
	char *p;
	if (in == NULL)
		return NULL;
	p = in + strlen(in) - 1;
	while (*p != '\\' && *p != '/' && *p != ':') {
		p--;
	}
	return ++p;
}
#endif

static void printusage(const std::string& execname) {
	std::cout << "usage: \n";
	std::cout << execname << " -t path_to_tile [-o outfile.bmp]" << std::endl;
	std::cout << "Extracts graphics from 'master.dat' and save as bmp" << std::endl;
	std::cout << "Furthermore:" << std::endl;
	std::cout << " " << execname << " -s		 : show list of lists (with indices for next step)" << std::endl;
	std::cout << " " << execname << " -l n	 : list content of list n (see above)" << std::endl;
	std::cout << std::endl;
	std::cout << "Then something like:" << std::endl;
	std::cout << " " << execname << " -t art/walls/SHIP09.FRM		 : saves as 'out.bmp'" << std::endl;
	std::cout << std::endl;
	std::cout << "I assume 'master.dat' and 'critter.dat' is in the current working dir." << std::endl;
	std::cout << "More switches:" << std::endl;
	std::cout << " -o outfile		 : either .bmp or .png please" <<std::endl;
	std::cout << " -D master.dat	: alternative filename; NOT! path" << std::endl;
	std::cout << " -C critter.dat : alternative filename; NOT! path" << std::endl;
	std::cout << " -d [0-6]			 : direction for frame (default 0)" << std::endl;
	std::cout << " -i [0-N]			 : frame index (default 0)" << std::endl;
}

extern int write_png(const char *filename, SDL_Surface& surface);

FIFE::Engine * engine = NULL;
FIFE::map::Factory* mapfactory = NULL;
FIFE::VFS* vfs = NULL;
FIFE::RawFRM* tile = NULL;
FIFE::map::loaders::fallout::list* list = NULL;

typedef struct s_list_type {
	const unsigned int numEntries;
	const char *path;
} s_list_type;

s_list_type knownLists[] = {
	{21, "art/backgrnd/backgrnd.lst"},
	{13, "art/heads/heads.lst"},
	{468, "art/intrface/intrface.lst"},
	{367, "art/inven/inven.lst"},
	{5, "art/inven/new.lst"},
	{192, "art/items/items.lst"},
	{3, "art/items/new.lst"},
	{56, "art/misc/misc.lst"},
	{7, "art/scenery/new.lst"},
	{1863, "art/scenery/scenery.lst"},
	{175, "art/skilldex/skilldex.lst"},
	{3102, "art/tiles/tiles.lst"},
	{1, "art/walls/new.lst"},
	{1690, "art/walls/walls.lst"},
	{103, "art/critters/critters.lst"}
	// -t path/to/sprite works, if critter.dat is found.
};

void cleanup() {
	/*if (mapfactory);
		delete mapfactory;*/
	if(tile)
		delete tile;
	if (list)
		delete list;
	if (vfs);
		vfs->cleanup();
		SDL_Quit();
}

void showList(int k) {
	list = new FIFE::map::loaders::fallout::list(knownLists[k].path);
	char* tmp = strdup(knownLists[k].path);
	char* tmp2 = dirname(tmp);

	std::cout << "** list of files in " << tmp2 << "**" << std::endl;
	for (unsigned int i=0; i < knownLists[k].numEntries; i++) {
		std::cout << tmp2 << "/" << list->getProFile(i) << std::endl;
	}
	free(tmp);
}

const char* action_suffix[] = {
	/*idle =*/ "aa",
	/*walk =*/ "ab",
	/*run =*/ "at",
	/*climb =*/ "ae",
	/*pickup =*/ "ak",
	/*use = */"al",
	/*dodge =*/ "an",
	/*hitfront =*/ "ao",
	/*hitback =*/ "ap",
	/*kick =*/ "ar",
	/*run =*/ "at",
	/*throw =*/ "as",
	/*punch = */"aq"
};

void showCritList(int k) {
	FIFE::map::loaders::fallout::CritLST critlist(knownLists[k].path);
	char* tmp = strdup(knownLists[k].path);
	char* tmp2 = dirname(tmp);
	
	std::cout << "** list of files in " << tmp2 << "**" << std::endl;
	for (unsigned int i=0; i < knownLists[k].numEntries; i++) {
		for(int j=0; j< sizeof(action_suffix)/sizeof(action_suffix[0]); ++j) {
			std::string fname (tmp2);
			fname += "/" + critlist.getProFile(i)+action_suffix[j]+".frm";
			if( FIFE::VFS::instance()->exists(fname)) {
				std::cout << fname << std::endl;
			}
		}
	}
	free(tmp);
}

int main(int argc, char* argv[]) {
	std::string tilepath;
	std::string outfile = std::string("out.bmp");
	std::string masterDat = std::string("master.dat");
	std::string critterDat = std::string("critter.dat");
	int showListCnt = -1;
	unsigned int direction = 0;
	unsigned int frameidx = 0;
	int c;
	
	while ((c = getopt (argc, argv, "hsl:o:t:d:i:D:C:")) != -1)
		switch(c) {
			case 's':
				std::cout << "** list of graphic lists **" << std::endl;
				for (int i=0; i<15; i++) {
					std::cout << i << ": " << knownLists[i].path << std::endl;
				}
				return 0;
				break;
			case 'l':
				showListCnt = atoi(optarg);
				break;
			case 'o':
				outfile = std::string(optarg);
				break;
			case 't':
				tilepath = std::string(optarg);
				break;
			case 'd':
				direction = atoi(optarg);
				break;
			case 'i':
				frameidx = atoi(optarg);
				break;
			case 'D':
				masterDat = std::string(optarg);
				break;
			case 'C':
				critterDat = std::string(optarg);
				break;
			case 'h':
				printusage(argv[0]);
				return 0;
				break;
		}
	
	if (showListCnt == -1 && tilepath.size() == 0) {
		printusage(argv[0]);
		return 1;
	}

  char *argv_fake[1] = {argv[0]};
  try {
    engine = new FIFE::Engine(0, argv_fake);
  }
  catch (const FIFE::Exception& exception) {
    std::cerr << "Error: could not start engine: " << exception.getMessage() << std::endl;
  }
	SDL_Init(SDL_INIT_VIDEO);
	//SDL_SetVideoMode(640, 480, 32, 0);
	bool saveAsPNG = false;
	if ((outfile.find(".png") > -1) || (outfile.find(".PNG") > -1))
		saveAsPNG = true;

	try {
    FIFE::VFSSourceFactory::instance();
    vfs = FIFE::VFS::instance();

		FIFE::map::Factory::instance();

		vfs->addSource(new FIFE::VFSHostSystem());
		if (vfs->exists(masterDat)) {
			vfs->addSource(new FIFE::map::loaders::fallout::DAT2(masterDat));
		}
		if (vfs->exists(critterDat)) {
			vfs->addSource(new FIFE::map::loaders::fallout::DAT2(critterDat));
		}
		if (showListCnt > -1) {
			if(showListCnt==14) {
				showCritList(14);
			} else {
				showList(showListCnt);
			}
		}

		if (tilepath.size() > 0) {
      FIFE::map::loaders::fallout::AnimatedPalette* m_palette = 
        FIFE::map::loaders::fallout::AnimatedPalette::load("content/gfx/fallout_palette.xml");
			tile = new FIFE::RawFRM(tilepath.c_str(), m_palette);
			if (tile == NULL) {
				std::cerr << "Error: got a NULL instead of an Image!" << std::endl;
				cleanup();
				return 1;
			}

			uint8_t * rgba_data = tile->getFrameRGBA(direction, frameidx);
			assert(rgba_data);
			std::pair<uint16_t, uint16_t> dims = tile->getFrameDimension(direction, frameidx);
			uint16_t width = dims.first;
			uint16_t height = dims.second;
			std::cout << "size = " << width << "," << height << std::endl;
			std::cout << "num_frames	= " << tile->getNumFrames() << std::endl;
			std::cout << "frames_per_second	= " << tile->getFramesPerSecond() << std::endl;
			std::cout << "action_frame = "<<tile->getActionFrameIdx() << std::endl;
			SDL_Surface* surface = 
				SDL_CreateRGBSurface(SDL_SWSURFACE , width, height, 32, 
					0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
			SDL_LockSurface(surface);

			unsigned int size = width * height * 4;
			uint8_t* pixeldata = static_cast<uint8_t*>(surface->pixels);
			std::copy(rgba_data, rgba_data + size, pixeldata);
			SDL_UnlockSurface(surface);

			if (saveAsPNG)
				write_png(outfile.c_str(), *surface);
			else
				SDL_SaveBMP(surface, outfile.c_str());
			
			SDL_FreeSurface(surface);
			std::cout << "shift_xy = " 
				<< tile->getShiftX(direction,frameidx) << " ," 
				<< tile->getShiftY(direction,frameidx) << std::endl;
			delete [] rgba_data;
		}

	} catch (const FIFE::Exception& exception) {
		std::cerr << "Caught exception: " << exception.getMessage() << std::endl;
		cleanup();
		return 1;
	}

	cleanup();
	return 0;
}
