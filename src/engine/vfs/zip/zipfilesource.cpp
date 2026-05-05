// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "zipfilesource.h"

// Standard C++ library includes
#include <cassert>
#include <cstring>

// 3rd party library includes

// FIFE includes
namespace FIFE
{

    ZipFileSource::ZipFileSource(uint8_t* data, uint32_t datalen) : m_data(data), m_datalen(datalen)
    {
    }

    ZipFileSource::~ZipFileSource()
    {
        delete[] m_data;
    }

    uint32_t ZipFileSource::getSize() const
    {
        return m_datalen;
    }

    void ZipFileSource::readInto(uint8_t* target, uint32_t start, uint32_t len)
    {
        assert(start + len <= m_datalen);
        memcpy(target, m_data + start, len);
    }
} // namespace FIFE
