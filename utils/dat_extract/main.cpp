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
#include <cassert>
#include <iostream>
#include <string>

// Platform dependent includes
#include <iostream>
#ifndef __WIN32
#include <libgen.h>
#else
#include <getopt.h>
#endif

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "loaders/fallout/vfs_loaders/dat1.h"
#include "loaders/fallout/vfs_loaders/dat2.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfshostsystem.h"
#include "vfs/vfssourcefactory.h"
#include "controller/engine.h"
#include "util/base/exception.h"
#include "util/settingsmanager.h"

using namespace FIFE;

extern int print_usage(const char* prg_name, int exit_code);

int main(int argc, char* argv[]) {
	Engine* engine = NULL;
	VFS* vfs = NULL;
	RawDataPtr rd;
	std::string src_file("critter.dat");
	std::string target_file("");
	int fo_format = 1;
	int c;

	while ((c = getopt (argc, argv, "f:s:t:h")) != -1)
		switch(c) {
			case 'f':
				fo_format = atoi(optarg);
				break;
			case 's':
				src_file = std::string(optarg);
				break;
			case 't':
				target_file = std::string(optarg);
				break;
			case 'h':
				print_usage(argv[0], 0);
		}

	char *argv_fake[1] = {argv[0]};
	try {
		engine = new Engine(true);
	}

	catch (const Exception& exception) {
		std::cerr << "Error: could not start engine: " << exception.getMessage() << std::endl;
	}

	if (src_file.length() == 0) {
		std::cout << "Invalid DAT source file"; 
		print_usage(argv[0], 1);
	}

	if (target_file.length() == 0) {
		std::cout << "Invalid target file to extract";
		print_usage(argv[0], 1);
	}

	if (!(fo_format == 1 || fo_format == 2)) {
		std::cout << "Unknown format " << fo_format;
		print_usage(argv[0], 1);
	}

	try {
		VFSSourceFactory::instance();
		vfs = VFS::instance();
		vfs->addSource(new VFSHostSystem());

		if (vfs->exists(src_file)) {
			if (fo_format == 1) {
				vfs->addSource(new map::loaders::fallout::DAT1(src_file));
			} else {
				vfs->addSource(new map::loaders::fallout::DAT2(src_file));
			}
		} else {
			throw NotFound(src_file);
		}

		if (!vfs->exists(target_file)) {
			throw NotFound(target_file);
		}

		rd = vfs->open(target_file);

		int len = rd->getDataLength();
		uint8_t * buf = new uint8_t[len];
		rd->readInto(buf, len);

		std::string target_filename(target_file);
		if (target_file.find("/") != std::string::npos) {
		target_filename.assign(target_file, target_file.rfind("/") + 1,
			target_file.length() - target_file.rfind("/") - 1);
		}

		FILE* out_fd = fopen(target_filename.c_str(), "wb");
		assert(out_fd);
		fwrite(buf, sizeof(uint8_t), len, out_fd);
		fclose(out_fd);

		delete buf;
	}

	catch (const Exception& exception) {
		std::cerr << "exception: " << exception.getMessage() << std::endl;
		return 1;
	}

	if (engine) {
		delete engine;
	}
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
