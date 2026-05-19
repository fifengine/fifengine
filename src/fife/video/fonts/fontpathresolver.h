// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_FONTS_FONTPATHRESOLVER_H
#define FIFE_FONTS_FONTPATHRESOLVER_H

// Standard C++ library includes
#include <string>
#include <unordered_map>
#include <vector>

// FIFE includes
#include "util/base/fife_stdint.h"
#include "vfs/raw/rawdata.h"

namespace FIFE
{

    /**
     * Resolves font filenames to font data by searching through configured
     * font paths using the VFS.
     *
     * Features:
     *   - Lazy VFS source registration (paths registered on first use)
     *   - Search cache (successful resolutions cached by filename)
     *   - Early-exit extension filtering (skips non-font extensions)
     */
    class FIFE_API FontPathResolver
    {
        public:
            FontPathResolver()  = default;
            ~FontPathResolver() = default;

            FontPathResolver(FontPathResolver const &)            = delete;
            FontPathResolver& operator=(FontPathResolver const &) = delete;

            void setSearchPaths(std::vector<std::string> const & paths);

            std::vector<std::string> const & getSearchPaths() const
            {
                return m_searchPaths;
            }

            RawData* resolve(std::string const & filename);

            void clearCache();

        private:
            static bool hasFontExtension(std::string const & filename);

            std::vector<std::string> m_searchPaths;
            std::unordered_map<std::string, std::string> m_cache;
            std::unordered_map<std::string, bool> m_registeredPaths;
    };

} // namespace FIFE

#endif
