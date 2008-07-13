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
#include <algorithm>
#include <vector>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/log/logger.h"

#include "rawdata.h"

namespace FIFE {
	static Logger _log(LM_VFS);
	
	RawData::RawData(RawDataSource* datasource) : m_datasource(datasource), m_index_current(0) {

	}

	RawData::~RawData() {
		delete m_datasource;
	}

	std::vector<uint8_t> RawData::getDataInBytes() {
		std::vector<uint8_t> target;
		uint32_t size = getDataLength();
		uint8_t* array = new uint8_t[size];
		readInto(array, size);
		for (uint32_t i = 0; i < size; i++) {
			target.push_back(array[i]);
		}
		delete array;
		return target;
	}

	std::vector<std::string> RawData::getDataInLines() {
		std::vector<std::string> target;

		std::string line;
		while (getLine(line)) {
			target.push_back(line);
		}
		return target;
	}

	unsigned int RawData::getDataLength() const {
		return m_datasource->getSize();
	}

	unsigned int RawData::getCurrentIndex() const {
		return m_index_current;
	}

	void RawData::setIndex(unsigned int index) {
		if (index > getDataLength())
			throw IndexOverflow(__FUNCTION__);

		m_index_current = index;
	}

	void RawData::moveIndex(int offset) {
		setIndex(getCurrentIndex() + offset);
	}

	void RawData::readInto(uint8_t* buffer, size_t len) {
		if (m_index_current + len > getDataLength()) {
			FL_LOG(_log, LMsg("RawData") << m_index_current << " : " << len << " : " << getDataLength());
			throw IndexOverflow(__FUNCTION__);
		}

		m_datasource->readInto(buffer, m_index_current, len);
		m_index_current += len;
	}

	uint8_t RawData::read8() {
		return readSingle<uint8_t>();
	}

	uint16_t RawData::read16Little() {
		uint16_t val = readSingle<uint16_t>();
		return littleToHost(val);
	}

	uint32_t RawData::read32Little() {
		uint32_t val = readSingle<uint32_t>();
		return littleToHost(val);
	}

	uint16_t RawData::read16Big() {
		uint16_t val = readSingle<uint16_t>();
		return bigToHost(val);
	}

	uint32_t RawData::read32Big() {
		uint32_t val = readSingle<uint32_t>();
		return bigToHost(val);
	}

	std::string RawData::readString(size_t len) {
		char* str = new char[len+1];
		readInto(reinterpret_cast<uint8_t*>(str), len);
		str[len] = 0x00;
		std::string ret = str;
		delete [] str;
		return ret;
	}

	void RawData::read(std::string& outbuffer, int size) {
		if ((size < 0) || ((size + m_index_current + 1) > getDataLength())) {
			size = getDataLength() - m_index_current - 1;
		}
		if (size == 0) {
			outbuffer = "";
			return;
		}
		uint8_t* array = new uint8_t[size + 1];
		m_datasource->readInto(array, m_index_current, size);
		array[size] = 0x00;
		outbuffer = reinterpret_cast<char*>(array);
		delete[] array;
		m_index_current += size;
	}
	

	bool RawData::getLine(std::string& buffer) {
		if (getCurrentIndex() >= getDataLength())
			return false;

		buffer = "";
		char c;
		while (getCurrentIndex() < getDataLength() && (c = read8()) != '\n')
			buffer += c;

		return true;
	}

	bool RawData::littleEndian() {
		static int endian = 2;
		if (endian == 2) {
			uint32_t value = 0x01;
			endian = reinterpret_cast<uint8_t*>(&value)[0];
			FL_LOG(_log, LMsg("RawData") << "we are on a " << (endian == 1 ? "little endian" : "big endian") << " machine");
		}

		return endian == 1;
	}



}//FIFE
