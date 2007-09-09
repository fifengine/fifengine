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
#include "mffalloutdat1.h"
#include "log.h"
#include "rawdata.h"
#include "exception.h"

#include <list>

namespace FIFE {


	MFFalloutDAT1::MFFalloutDAT1(const std::string& file) : m_datpath(file), m_data(VFS::instance()->open(file))  {
		Log("MFFalloutDAT1") << "loading: " << file << " filesize: " << m_data->getDataLength();
		readIndex();
	}

	MFFalloutDAT1::~MFFalloutDAT1() {}

	MFFalloutDAT1::MFFalloutDAT1(const MFFalloutDAT1&) : m_datpath(), m_data() {}

	MFFalloutDAT1& MFFalloutDAT1::operator=(const MFFalloutDAT1&) {
		return *this;
	}

	void MFFalloutDAT1::readIndex() {
		m_filelist.clear();
		m_data->setIndex(0);

		const uint32_t dircount = m_data->read32Big();
		m_data->moveIndex(4*3);

		std::list<std::string> directorynames;
		for (uint32_t i = 0; i < dircount; ++i) {
			std::string name = readString();
			if (name == ".")
				name = "";

			directorynames.push_back(name);
		}

		for (std::list<std::string>::iterator i = directorynames.begin(); i != directorynames.end(); ++i)
			loadFileList(*i);
	}

	void MFFalloutDAT1::loadFileList(const std::string& dirname) {
		const uint32_t filecount = m_data->read32Big();
		m_data->moveIndex(4*3);
		for (uint32_t i = 0; i < filecount; ++i) {
			MFFalloutRawDataDAT1::s_info info;
			info.name = fixPath(dirname + "/" + readString());
			info.type = m_data->read32Big();
			info.offset = m_data->read32Big();
			info.unpackedLength = m_data->read32Big();
			info.packedLength = m_data->read32Big();

			m_filelist.insert(std::make_pair(info.name, info));
		}
	}

	std::string MFFalloutDAT1::readString() {
		uint8_t length = m_data->read8();
		return m_data->readString(length);
	}

	RawData* MFFalloutDAT1::open(const std::string& file) const {
		const MFFalloutRawDataDAT1::s_info& info = getInfo(file);
		return new RawData(new MFFalloutRawDataDAT1(m_datpath, info));
	}

	bool MFFalloutDAT1::fileExists(const std::string& name) const {
		return m_filelist.find(name) != m_filelist.end();
	}

	const MFFalloutRawDataDAT1::s_info& MFFalloutDAT1::getInfo(const std::string& name) const {
		type_filelist::const_iterator i = m_filelist.find(name);
		if (i == m_filelist.end())
			throw NotFound(name);

		return i->second;
	}

	VFS::type_stringlist MFFalloutDAT1::listFiles(const std::string& pathstr) const {
		return list(pathstr, false);
	}

	VFS::type_stringlist MFFalloutDAT1::listDirectories(const std::string& pathstr) const {
		return list(pathstr, true);
	}

	VFS::type_stringlist MFFalloutDAT1::list(const std::string& pathstr, bool dirs) const {
		VFS::type_stringlist list;
		std::string path = pathstr;

		if (path.find("./") == 0)
			path.erase(0, 2);

		if (path.size() && path[path.size() - 1] != '/')
			path += '/';

		type_filelist::const_iterator end = m_filelist.end();
		for (type_filelist::const_iterator i = m_filelist.begin(); i != end; ++i) {
			const std::string& file = i->first;
			if (file.find(path) == 0) {
				std::string cleanedfile = file.substr(path.size(), file.size()); // strip the pathstr
				bool isdir = cleanedfile.find('/') != std::string::npos; // if we still have a / it's a subdir

				if (isdir) {
					cleanedfile = cleanedfile.substr(0, cleanedfile.find('/'));
					if (cleanedfile.find('/') != cleanedfile.rfind('/')) // check if this is a direct subdir
						continue;
				}

				if (isdir == dirs)
					list.insert(cleanedfile);
			}
		}

		return list;
	}

}
