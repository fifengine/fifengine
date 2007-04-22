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
}
