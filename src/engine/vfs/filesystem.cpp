// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <filesystem>
#include <iterator>
#include <string>

namespace fs = std::filesystem;

namespace FIFE
{

    static bool HasParentPath(const fs::path& path)
    {
        return path.has_parent_path();
    }

    static fs::path GetParentPath(const fs::path& path)
    {
        return path.parent_path();
    }

    static std::string GetFilenameFromPath(const fs::path& path)
    {
        return path.filename().string();
    }

    static std::string GetFilenameFromDirectoryIterator(const fs::directory_iterator& iter)
    {
        if (iter == fs::end(iter)) {
            return "";
        }
        return iter->path().filename().string();
    }

    static std::string GetPathIteratorAsString(const fs::path::iterator& pathIter)
    {
        return (*pathIter).string();
    }

    static fs::path GetAbsolutePath(const fs::path& path)
    {
        return fs::absolute(path);
    }

    static fs::path GetAbsolutePath(const std::string& path)
    {
        return fs::absolute(fs::path(path));
    }

    static std::string GetExtension(const fs::path& path)
    {
        return path.extension().string();
    }

    static std::string GetExtension(const std::string& path)
    {
        return GetExtension(fs::path(path));
    }

    static bool HasExtension(const fs::path& path)
    {
        const std::string extension = GetExtension(path);
        return !(extension.empty() || extension == ".");
    }

    static bool HasExtension(const std::string& path)
    {
        return HasExtension(fs::path(path));
    }

    static std::string GetStem(const fs::path& path)
    {
        if (!HasExtension(path)) {
            return "";
        }
        return path.stem().string();
    }

    static std::string GetStem(const std::string& path)
    {
        return GetStem(fs::path(path));
    }

} // namespace FIFE