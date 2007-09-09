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

#include "rawdata.h"
#include "exception.h"
#include "log.h"

#include <algorithm>

namespace FIFE {

	RawData::RawData(RawDataSource* datasource) : m_datasource(datasource), m_index_current(0) {

	}

	RawData::~RawData() {
		delete m_datasource;
	}


	unsigned int RawData::getDataLength() const {
		return m_datasource->getSize();
	}

	unsigned int RawData::getCurrentIndex() const {
		return m_index_current;
	}

	void RawData::setIndex(unsigned int index) {
		if (index > getDataLength())
			throw IndexOverflow(__PRETTY_FUNCTION__);

		m_index_current = index;
	}

	void RawData::moveIndex(int offset) {
		setIndex(getCurrentIndex() + offset);
	}

	void RawData::readInto(uint8_t* buffer, size_t len) {
		if (m_index_current + len > getDataLength()) {
			Log("RawData") << m_index_current << " : " << len << " : " << getDataLength();
			throw IndexOverflow(__PRETTY_FUNCTION__);
		}

		m_datasource->readInto(buffer, m_index_current, len);
		m_index_current += len;
	}

	uint8_t RawData::read8() {
		return read<uint8_t>();
	}

	uint16_t RawData::read16Little() {
		uint16_t val = read<uint16_t>();
		return littleToHost(val);
	}

	uint32_t RawData::read32Little() {
		uint32_t val = read<uint32_t>();
		return littleToHost(val);
	}

	uint16_t RawData::read16Big() {
		uint16_t val = read<uint16_t>();
		return bigToHost(val);
	}

	uint32_t RawData::read32Big() {
		uint32_t val = read<uint32_t>();
		return bigToHost(val);
	}

	std::string RawData::readString(size_t len) {
		char str[len+1];
		readInto(reinterpret_cast<uint8_t*>(str), len);
		str[len] = 0x00;
		return str;
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
			Log("RawData") << "we are on a " << (endian == 1 ? "little endian" : "big endian") << " machine";
		}

		return endian == 1;
	}



}//FIFE
