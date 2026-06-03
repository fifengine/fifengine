// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "rawdatamemsource.h"

// Standard C++ library includes
#include <algorithm>
#include <span>

// 3rd party library includes

// FIFE includes
namespace FIFE
{

    RawDataMemSource::RawDataMemSource(uint32_t len) : m_data(len)
    {
    }

    RawDataMemSource::~RawDataMemSource() = default;

    uint32_t RawDataMemSource::getSize() const
    {
        return static_cast<uint32_t>(m_data.size());
    }

    void RawDataMemSource::readInto(uint8_t* buffer, uint32_t start, uint32_t length)
    {
        auto const src = std::span(m_data).subspan(start, length);
        std::copy(src.begin(), src.end(), buffer);
    }

    uint8_t* RawDataMemSource::getRawData()
    {
        return m_data.data();
    }

} // namespace FIFE
