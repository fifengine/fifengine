/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdata.h"
#include "util/base/exception.h"
#include "util/log/logger.h"

#include "dat1.h"

namespace FIFE {
	static Logger _log(LM_FO_LOADERS);

	DAT1::DAT1(VFS* vfs, const std::string& file) : VFSSource(vfs), m_datpath(file), m_data(vfs->open(file))  {
		FL_LOG(_log, LMsg("MFFalloutDAT1") 
			<< "loading: " << file 
			<< " filesize: " << m_data->getDataLength());

		m_data->setIndex(0);

		const uint32_t dircount = m_data->read32Big();
		m_data->moveIndex(4*3);

		FL_LOG(_log, LMsg("MFFalloutDAT1") 
			<< "number of directories " << dircount);

		// Sanity check. Each dir entry needs min. 16 bytes.
		if( dircount*16 > m_data->getDataLength() ) {
			throw InvalidFormat("directory count larger than filesize.");
		}

		std::list<std::string> dir_names;
		for (uint32_t i = 0; i < dircount; ++i) {
			std::string name = readString();
			if (name == ".") {
				name = "";
			}
			dir_names.push_back(name);
		}

		for(std::list<std::string>::iterator i= dir_names.begin(); i!= dir_names.end(); ++i)
			loadFileList(*i);
	}

	void DAT1::loadFileList(const std::string& dirname) {
		const uint32_t filecount = m_data->read32Big();
		m_data->moveIndex(4*3);
		for (uint32_t i = 0; i < filecount; ++i) {
			RawDataDAT1::s_info info;
			info.name = fixPath(dirname + "/" + readString());
			info.type = m_data->read32Big();
			info.offset = m_data->read32Big();
			info.unpackedLength = m_data->read32Big();
			info.packedLength = m_data->read32Big();

			m_filelist.insert(std::make_pair(info.name, info));
		}
	}

	std::string DAT1::readString() {
		uint8_t length = m_data->read8();
		return m_data->readString(length);
	}

	RawData* DAT1::open(const std::string& file) const {
		const RawDataDAT1::s_info& info = getInfo(file);
		return new RawData(new RawDataDAT1(getVFS(), m_datpath, info));
	}

	bool DAT1::fileExists(const std::string& name) const {
		return m_filelist.find(name) != m_filelist.end();
	}

	const RawDataDAT1::s_info& DAT1::getInfo(const std::string& name) const {
		type_filelist::const_iterator i = m_filelist.find(name);
		if (i == m_filelist.end())
			throw NotFound(name);

		return i->second;
	}

	std::set<std::string> DAT1::listFiles(const std::string& pathstr) const {
		return list(pathstr, false);
	}

	std::set<std::string> DAT1::listDirectories(const std::string& pathstr) const {
		return list(pathstr, true);
	}

	std::set<std::string> DAT1::list(const std::string& pathstr, bool dirs) const {
		std::set<std::string> list;
		std::string path = pathstr;

		// Normalize the path
		if (path.find("./") == 0) {
			path.erase(0, 2);
		}
		
		size_t lastIndex = path.size();
		if (lastIndex != 0 && path[lastIndex-1] != '/') {
			path += '/';
		}

		type_filelist::const_iterator end = m_filelist.end();
		for (type_filelist::const_iterator i = m_filelist.begin(); i != end; ++i) {
			const std::string& file = i->first;
			if (file.find(path) == 0) {
				std::string cleanedfile = file.substr(path.size(), file.size()); // strip the pathstr
				bool isdir = cleanedfile.find('/') != std::string::npos; // if we still have a / it's a subdir

				if (isdir) {
					cleanedfile = cleanedfile.substr(0, cleanedfile.find('/'));
					if (cleanedfile.find('/') != cleanedfile.rfind('/')) {
						// check if this is a direct subdir
						continue;
					}
				}

				if (isdir == dirs) {
					list.insert(cleanedfile);
				}
			}
		}

		return list;
	}
} // FIFE
