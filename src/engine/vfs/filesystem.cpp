// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <filesystem>
#include <iterator>
#include <string>

// FIFE includes
#include "vfs/filesystem.h"

namespace fs = std::filesystem;

namespace FIFE
{

    bool HasParentPath(fs::path const & path)
    {
        return path.has_parent_path();
    }

    fs::path GetParentPath(fs::path const & path)
    {
        return path.parent_path();
    }

    std::string GetFilenameFromPath(fs::path const & path)
    {
        return path.filename().string();
    }

    std::string GetFilenameFromDirectoryIterator(fs::directory_iterator const & iter)
    {
        if (iter == fs::end(iter)) {
            return "";
        }
        return iter->path().filename().string();
    }

    std::string GetPathIteratorAsString(fs::path::iterator const & pathIter)
    {
        return (*pathIter).string();
    }

    fs::path GetAbsolutePath(fs::path const & path)
    {
        return fs::absolute(path);
    }

    fs::path GetAbsolutePath(std::string const & path)
    {
        return fs::absolute(fs::path(path));
    }

    std::string GetExtension(fs::path const & path)
    {
        return path.extension().string();
    }

    std::string GetExtension(std::string const & path)
    {
        return GetExtension(fs::path(path));
    }

    bool HasExtension(fs::path const & path)
    {
        std::string const extension = GetExtension(path);
        return !(extension.empty() || extension == ".");
    }

    bool HasExtension(std::string const & path)
    {
        return HasExtension(fs::path(path));
    }

    std::string GetStem(fs::path const & path)
    {
        if (!HasExtension(path)) {
            return "";
        }
        return path.stem().string();
    }

    std::string GetStem(std::string const & path)
    {
        return GetStem(fs::path(path));
    }

} // namespace FIFE
