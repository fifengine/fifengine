// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <string>
#include <utility>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"

#include "rawdatafile.h"

namespace FIFE
{

    RawDataFile::RawDataFile(std::string  file) :
        m_file(std::move(file)), m_stream(m_file.c_str(), std::ios::binary), m_filesize(0)
    {
        if (!m_stream) {
            throw CannotOpenFile(m_file);
        }

        m_stream.seekg(0, std::ios::end);
        m_filesize = m_stream.tellg();
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
        m_stream.read(reinterpret_cast<char*>(buffer), length);
    }

} // namespace FIFE
