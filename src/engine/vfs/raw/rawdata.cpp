// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <limits>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/log/logger.h"

#include "rawdata.h"

namespace FIFE
{
    static Logger _log(LM_VFS);

    RawData::RawData(RawDataSource* datasource) : m_datasource(datasource), m_index_current(0) { }

    RawData::~RawData()
    {
        delete m_datasource;
    }

    std::vector<uint8_t> RawData::getDataInBytes()
    {
        // get the total file size
        const uint32_t size = getDataLength();

        // create output vector
        std::vector<uint8_t> target;

        // resize vector to file size
        target.resize(size);

        // read bytes directly into vector
        readInto(target.data(), target.size());

        return target;
    }

    std::vector<std::string> RawData::getDataInLines()
    {
        std::vector<std::string> target;

        std::string line;
        while (getLine(line)) {
            target.push_back(line);
        }
        return target;
    }

    uint32_t RawData::getDataLength() const
    {
        const size_t dataLength = m_datasource->getSize();
        assert(dataLength <= std::numeric_limits<uint32_t>::max());
        return static_cast<uint32_t>(dataLength);
    }

    uint32_t RawData::getCurrentIndex() const
    {
        return m_index_current;
    }

    void RawData::setIndex(uint32_t index)
    {
        if (index > getDataLength()) {
            throw IndexOverflow(__FUNCTION__);
        }

        m_index_current = index;
    }

    void RawData::moveIndex(int32_t offset)
    {
        setIndex(getCurrentIndex() + offset);
    }

    void RawData::readInto(uint8_t* buffer, size_t len)
    {
        assert(len <= std::numeric_limits<uint32_t>::max());
        const uint32_t checkedLen = static_cast<uint32_t>(len);

        if (m_index_current + checkedLen > getDataLength()) {
            FL_LOG(_log, LMsg("RawData") << m_index_current << " : " << checkedLen << " : " << getDataLength());
            throw IndexOverflow(__FUNCTION__);
        }

        m_datasource->readInto(buffer, m_index_current, checkedLen);
        m_index_current += checkedLen;
    }

    uint8_t RawData::read8()
    {
        return readSingle<uint8_t>();
    }

    uint16_t RawData::read16Little()
    {
        auto val = readSingle<uint16_t>();
        return littleToHost(val);
    }

    uint32_t RawData::read32Little()
    {
        auto val = readSingle<uint32_t>();
        return littleToHost(val);
    }

    uint16_t RawData::read16Big()
    {
        auto val = readSingle<uint16_t>();
        return bigToHost(val);
    }

    uint32_t RawData::read32Big()
    {
        auto val = readSingle<uint32_t>();
        return bigToHost(val);
    }

    std::string RawData::readString(size_t len)
    {
        std::vector<uint8_t> strVector;
        strVector.resize(len);
        readInto(strVector.data(), len);

        std::string ret(strVector.begin(), strVector.end());

        return ret;
    }

    void RawData::read(std::string& outbuffer, int32_t size)
    {
        if ((size < 0) || ((size + m_index_current) > getDataLength())) {
            const int32_t remaining = static_cast<int32_t>(getDataLength() - m_index_current);
            size                    = remaining;
        }
        if (size == 0) {
            outbuffer = "";
            return;
        }

        outbuffer.resize(static_cast<size_t>(size));

        // read directly into string
        readInto(static_cast<uint8_t*>(static_cast<void*>(outbuffer.data())), static_cast<size_t>(size));
    }

    bool RawData::getLine(std::string& buffer)
    {
        if (getCurrentIndex() >= getDataLength()) {
            return false;
        }

        buffer = "";
        char c = 0;
        while (getCurrentIndex() < getDataLength() && (c = read8()) != '\n') {
            buffer += c;
        }

        return true;
    }

    bool RawData::littleEndian()
    {
        static int32_t endian = 2;
        if (endian == 2) {
            uint32_t value    = 0x01;
            uint8_t firstByte = 0;
            std::memcpy(&firstByte, &value, sizeof(firstByte));
            endian = static_cast<int32_t>(firstByte);
            FL_LOG(
                _log,
                LMsg("RawData") << "we are on a " << (endian == 1 ? "little endian" : "big endian") << " machine");
        }

        return endian == 1;
    }

} // namespace FIFE
