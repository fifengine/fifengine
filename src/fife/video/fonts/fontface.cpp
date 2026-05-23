// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "fontface.h"

#include <stdexcept>

namespace FIFE
{

    FontFace::FontFace(AssetHandle handle) : m_assetHandle(handle)
    {
        if (handle.id == 0) {
            throw std::invalid_argument("FontFace AssetHandle must be valid");
        }
    }

    bool FontFace::supports(uint32_t /*codepoint*/) const
    {
        return false;
    }

} // namespace FIFE
