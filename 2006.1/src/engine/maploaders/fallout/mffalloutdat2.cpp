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

#include "mffalloutdat2.h"
#include "rawdata.h"
#include "exception.h"
#include "log.h"
#include "mffalloutrawdatadat2.h"

namespace FIFE {

	MFFalloutDAT2::MFFalloutDAT2(const std::string& file) : m_datpath(file), m_data(VFS::instance()->open(file)), m_filelist() {
		Log("MFFalloutDAT2") << "loading: " << file << " filesize: " << m_data->getDataLength();
		readFileList();
	}

	MFFalloutDAT2::~MFFalloutDAT2() {
	}

	MFFalloutDAT2::MFFalloutDAT2(const MFFalloutDAT2&) : m_datpath(), m_data(), m_filelist() {
	}

	MFFalloutDAT2& MFFalloutDAT2::operator=(const MFFalloutDAT2&) {
		return *this;
	}

	void MFFalloutDAT2::readFileList() {
		m_filelist.clear();
		m_data->setIndex(m_data->getDataLength() - 8);
		uint32_t fileListLength = m_data->read32Little();
		uint32_t archiveSize = m_data->read32Little();

		Log("MFFalloutDAT2") << "FileListLength: " << fileListLength << " ArchiveSize: " << archiveSize;
		if (archiveSize != m_data->getDataLength())
			throw InvalidFormat("size mismatch");

		m_data->setIndex(archiveSize - fileListLength - 8);
		uint32_t filecount = m_data->read32Little();
		Log("MFFalloutDAT2") << "FileCount: " << filecount;
		for (unsigned int i = 0; i < filecount; ++i) {
			MFFalloutRawDataDAT2::s_info info;

			uint32_t namelen = m_data->read32Little();
			info.name = fixPath(m_data->readString(namelen));

			info.type = m_data->read8();
			info.unpackedLength = m_data->read32Little();
			info.packedLength = m_data->read32Little();
			info.offset = m_data->read32Little();

			m_filelist.insert(std::make_pair(info.name, info));
		}

		Log("MFFalloutDAT2") << "readFileList done";
	}

	bool MFFalloutDAT2::fileExists(const std::string& name) const {
		return m_filelist.find(name) != m_filelist.end();
	}

	const MFFalloutRawDataDAT2::s_info& MFFalloutDAT2::getInfo(const std::string& name) const {
		type_filelist::const_iterator i = m_filelist.find(name);
		if (i == m_filelist.end())
			throw NotFound(name);

		return i->second;
	}

	RawData* MFFalloutDAT2::open(const std::string& file) const {
		const MFFalloutRawDataDAT2::s_info& info = getInfo(file);
		return new RawData(new MFFalloutRawDataDAT2(m_datpath, info));
	}

	VFS::type_stringlist MFFalloutDAT2::listFiles(const std::string& pathstr) const {
		return list(pathstr, false);
	}

	VFS::type_stringlist MFFalloutDAT2::listDirectories(const std::string& pathstr) const {
		return list(pathstr, true);
	}

	VFS::type_stringlist MFFalloutDAT2::list(const std::string& pathstr, bool dirs) const {
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
