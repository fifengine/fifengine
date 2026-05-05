// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "controllermappingsaver.h"

// Standard C++ library includes
#include <cstdio>
#include <string>

// 3rd party library includes

// FIFE includes
#include "util/base/exception.h"

namespace FIFE
{
    void ControllerMappingSaver::save(std::string const & data, std::string const & filename)
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
