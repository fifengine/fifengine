// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_STRINGUTILS_H_
#define FIFE_STRINGUTILS_H_

// Standard C++ library includes
#include <string>
#include <vector>

#include "util/base/fife_stdint.h"

namespace FIFE
{
    using IntVector = std::vector<int32_t>;
    int makeInt32(const std::string& str);
    IntVector tokenize(const std::string& str, char delim, char group = 0);
} // namespace FIFE

#endif
