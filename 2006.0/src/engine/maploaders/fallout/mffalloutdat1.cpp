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

}
