// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cstdio>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"

#include "controllermappingsaver.h"

namespace FIFE
{
    void ControllerMappingSaver::save(const std::string& data, const std::string& filename)
    {
        FILE* fp = nullptr;
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
        fp = _fsopen(filename.c_str(), "w", _SH_DENYNO);
#else
        fp = fopen(filename.c_str(), "w");
#endif
        fputs(data.c_str(), fp);
        fclose(fp);
    }
} // namespace FIFE
