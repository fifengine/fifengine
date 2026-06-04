// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_FONTS_FONTDEFINITIONLOADER_H
#define FIFE_VIDEO_FONTS_FONTDEFINITIONLOADER_H

#include <string>
#include <vector>

#include "fonttypes.h"
#include "platform.h"

namespace FIFE
{

    class FIFE_API FontDefinitionLoader
    {
        public:
            static std::vector<FontDefinition> loadFromXml(std::string const & xmlContent);

            std::vector<FontDefinition> loadFromFile(std::string const & path);
            std::vector<FontDefinition> loadFromString(std::string const & xml);
    };

} // namespace FIFE

#endif
