// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#pragma once

#include <string>
#include <vector>

#include "fonttypes.h"

namespace FIFE
{

    class FontDefinitionLoader
    {
        public:
            static std::vector<FontDefinition> loadFromXml(std::string const & xmlContent);

            std::vector<FontDefinition> loadFromFile(std::string const & path);
            std::vector<FontDefinition> loadFromString(std::string const & xml);
    };

} // namespace FIFE
