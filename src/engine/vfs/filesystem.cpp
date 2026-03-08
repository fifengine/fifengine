// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <filesystem>
#include <iterator>
#include <string>

namespace fs = std::filesystem;

namespace FIFE
{

    bool HasParentPath(const fs::path& path)
    {
        return path.has_parent_path();
    }

    fs::path GetParentPath(const fs::path& path)
    {
        return path.parent_path();
    }

    std::string GetFilenameFromPath(const fs::path& path)
    {
        return path.filename().string();
    }

    std::string GetFilenameFromDirectoryIterator(const fs::directory_iterator& iter)
    {
        if (iter == fs::end(iter)) {
            return "";
        }
        return iter->path().filename().string();
    }

    std::string GetPathIteratorAsString(const fs::path::iterator& pathIter)
    {
        return (*pathIter).string();
    }

    fs::path GetAbsolutePath(const fs::path& path)
    {
        return fs::absolute(path);
    }

    fs::path GetAbsolutePath(const std::string& path)
    {
        return fs::absolute(fs::path(path));
    }

    std::string GetExtension(const fs::path& path)
    {
        return path.extension().string();
    }

    std::string GetExtension(const std::string& path)
    {
        return GetExtension(fs::path(path));
    }

    bool HasExtension(const fs::path& path)
    {
        std::string extension = GetExtension(path);
        return !(extension.empty() || extension == ".");
    }

    bool HasExtension(const std::string& path)
    {
        return HasExtension(fs::path(path));
    }

    std::string GetStem(const fs::path& path)
    {
        if (!HasExtension(path)) {
            return "";
        }
        return path.stem().string();
    }

    std::string GetStem(const std::string& path)
    {
        return GetStem(fs::path(path));
    }

} // namespace FIFE