// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "controllermappingsaver.h"

// Standard C++ library includes
#include <fstream>
#include <string>

// 3rd party library includes

// FIFE includes
#include "util/base/exception.h"

namespace FIFE
{
    void ControllerMappingSaver::save(std::string const & data, std::string const & filename)
    {
        if (std::ofstream fp(filename); fp.is_open()) {
            fp << data;
        }
    }
} // namespace FIFE
