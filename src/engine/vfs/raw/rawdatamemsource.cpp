// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "rawdatamemsource.h"

// Standard C++ library includes
#include <algorithm>

// 3rd party library includes

// FIFE includes
namespace FIFE
{

    RawDataMemSource::RawDataMemSource(uint32_t len) : m_data(new uint8_t[len]), m_datalen(len)
    {
    }

    RawDataMemSource::~RawDataMemSource()
    {
        delete[] m_data;
    }

    uint32_t RawDataMemSource::getSize() const
    {
        return m_datalen;
    }

    void RawDataMemSource::readInto(uint8_t* buffer, uint32_t start, uint32_t length)
    {
        std::copy(m_data + start, m_data + start + length, buffer);
    }

    uint8_t* RawDataMemSource::getRawData() const
    {
        return m_data;
    }

} // namespace FIFE
