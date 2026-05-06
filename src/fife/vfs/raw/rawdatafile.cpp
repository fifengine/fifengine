// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "rawdatafile.h"

// Standard C++ library includes
#include <cassert>
#include <limits>
#include <string>
#include <utility>

// 3rd party library includes

// FIFE includes
#include "util/base/exception.h"

namespace FIFE
{

    RawDataFile::RawDataFile(std::string file) :
        m_file(std::move(file)), m_stream(m_file.c_str(), std::ios::binary), m_filesize(0)
    {
        if (!m_stream) {
            throw CannotOpenFile(m_file);
        }

        m_stream.seekg(0, std::ios::end);
        std::streamoff const streamSize = m_stream.tellg();
        assert(streamSize >= 0);
        assert(streamSize <= static_cast<std::streamoff>(std::numeric_limits<uint32_t>::max()));
        m_filesize = static_cast<uint32_t>(streamSize);
        m_stream.seekg(0, std::ios::beg);
    }

    RawDataFile::~RawDataFile() = default;

    uint32_t RawDataFile::getSize() const
    {
        return m_filesize;
    }

    void RawDataFile::readInto(uint8_t* buffer, uint32_t start, uint32_t length)
    {
        m_stream.seekg(start);
        m_stream.read(static_cast<char*>(static_cast<void*>(buffer)), length);
    }

} // namespace FIFE
