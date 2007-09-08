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
#include <string>
#include <iostream>
#include <vector>

// 3rd party library includes
#include <boost/scoped_array.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "vfs/vfssource.h"
#include "vfs/vfssourcefactory.h"
#include "vfs/vfshostsystem.h"
#include "util/exception.h"

#include "vfsutility.h"

namespace FIFE {
	VFSUtility::VFSUtility() {
	}

	VFSUtility::~VFSUtility() {
	}

	std::vector<uint8_t> VFSUtility::readBytes(const std::string& fname) {
		std::vector<uint8_t> target;
		RawDataPtr data = VFS::instance()->open(fname);
		uint32_t size = data->getDataLength();
		uint8_t* array = new uint8_t[size];
		data->readInto(array, size);
		for (uint32_t i = 0; i < size; i++) {
			target.push_back(array[i]);
		}
		delete array;
		return target;
	}

	std::vector<std::string> VFSUtility::readLines(const std::string& fname) {
		std::vector<std::string> target;
		RawDataPtr data = VFS::instance()->open(fname);

		std::string line;
		while (data->getLine(line)) {
			target.push_back(line);
		}
		return target;
	}

	std::vector<std::string> VFSUtility::listFiles(const std::string& path) {
		std::vector<std::string> target;
		VFS::type_stringlist files = VFS::instance()->listFiles(path);
		for (VFS::type_stringlist::iterator i = files.begin(); i != files.end(); ++i) {
			target.push_back(*i);
		}
		return target;
	}

	std::vector<std::string> VFSUtility::listDirectories(const std::string& path) {
		std::vector<std::string> target;
		VFS::type_stringlist files = VFS::instance()->listDirectories(path);
		for (VFS::type_stringlist::iterator i = files.begin(); i != files.end(); ++i) {
			target.push_back(*i);
		}
		return target;
	}

	bool VFSUtility::addSource(const std::string& sname) {
		VFSSource * src = VFSSourceFactory::instance()->createSource(sname);
		if (src) {
			VFS::instance()->addSource(src);
			return true;
		} else {
			Warn("VFSUtility") << "Invalid source: " << sname;
			return false;
		}
	}
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
