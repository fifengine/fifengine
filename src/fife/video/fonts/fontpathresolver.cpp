// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "fontpathresolver.h"

// Standard C++ library includes
#include <algorithm>
#include <array>
#include <cassert>
#include <format>
#include <string>
#include <string_view>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "vfs/filesystem.h"
#include "vfs/vfs.h"

namespace FIFE
{
    namespace
    {
        Logger& _log = []() -> Logger& {
            static Logger log(LM_VIDEO);
            return log;
        }();

        constexpr std::array<std::string_view, 3> fontExtensions = {".ttf", ".ttc", ".otf"};
    } // namespace

    void FontPathResolver::setSearchPaths(std::vector<std::string> const & paths)
    {
        m_searchPaths = paths;
        clearCache();
    }

    void FontPathResolver::clearCache()
    {
        m_cache.clear();
        m_registeredPaths.clear();
    }

    bool FontPathResolver::hasFontExtension(std::string const & filename)
    {
        std::string const ext = GetExtension(filename);
        return std::ranges::any_of(fontExtensions, [&ext](std::string_view fe) {
            return fe == ext;
        });
    }

    RawData* FontPathResolver::resolve(std::string const & filename)
    {
        assert("VFS instance must exist" && VFS::instance());

        // Early exit: not a font file
        if (!hasFontExtension(filename)) {
            FL_DBG(_log, std::format("FontPathResolver: skipped {} (not a font extension)", filename));
            return nullptr;
        }

        // Cache hit
        {
            auto it = m_cache.find(filename);
            if (it != m_cache.end()) {
                FL_DBG(_log, std::format("FontPathResolver: cache hit for {}", filename));
                try {
                    return VFS::instance()->open(it->second);
                } catch (NotFound const &) {
                    FL_WARN(
                        _log, std::format("FontPathResolver: cached path {} for {} is stale", it->second, filename));
                    m_cache.erase(it);
                }
            }
        }

        // Search each font path
        for (auto const & path : m_searchPaths) {
            // Lazy VFS source registration
            if (!m_registeredPaths.contains(path)) {
                if (!VFS::instance()->hasSource(path)) {
                    FL_DBG(_log, std::format("FontPathResolver: lazily registering VFS source: {}", path));
                    VFS::instance()->addNewSource(path);
                }
                m_registeredPaths[path] = true;
            }

            // Check if file exists in this source
            std::string const resolved = std::format("{}/{}", path, filename);
            if (VFS::instance()->exists(resolved)) {
                assert("resolved path must not already be in cache" && !m_cache.contains(filename));
                FL_DBG(_log, std::format("FontPathResolver: resolved {} -> {}", filename, resolved));
                m_cache[filename] = resolved;
                try {
                    return VFS::instance()->open(resolved);
                } catch (NotFound const &) {
                    FL_WARN(_log, std::format("FontPathResolver: found {} but could not open it", resolved));
                    m_cache.erase(filename);
                    return nullptr;
                }
            }
        }

        FL_DBG(_log, std::format("FontPathResolver: could not resolve {}", filename));
        return nullptr;
    }
} // namespace FIFE
