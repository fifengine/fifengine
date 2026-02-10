/***************************************************************************
*   Copyright (C) 2005-2019 by the FIFE team                              *
*   http://www.fifengine.net                                              *
*   This file is part of FIFE.                                            *
*                                                                         *
*   FIFE is free software; you can redistribute it and/or                 *
*   modify it under the terms of the GNU Lesser General Public            *
*   License as published by the Free Software Foundation; either          *
*   version 2.1 of the License, or (at your option) any later version.    *
*                                                                         *
*   This library is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
*   Lesser General Public License for more details.                       *
*                                                                         *
*   You should have received a copy of the GNU Lesser General Public      *
*   License along with this library; if not, write to the                 *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
***************************************************************************/

// Standard C++ library includes
#include <string>
#include <filesystem>
#include <iterator>

namespace fs = std::filesystem;

namespace FIFE {

    bool HasParentPath(const fs::path& path) {
        return path.has_parent_path();
    }

    fs::path GetParentPath(const fs::path& path) {
        return path.parent_path();
    }

    std::string GetFilenameFromPath(const fs::path& path) {
        return path.filename().string();
    }

    std::string GetFilenameFromDirectoryIterator(const fs::directory_iterator& iter) {
        if (iter == fs::end(iter)) {
            return "";
        }
        return iter->path().filename().string();
    }

    std::string GetPathIteratorAsString(const fs::path::iterator& pathIter) {
        return (*pathIter).string();
    }

    fs::path GetAbsolutePath(const std::string& path) {
        return fs::absolute(fs::path(path));
    }

    fs::path GetAbsolutePath(const fs::path& path) {
        return fs::absolute(path);
    }

    bool HasExtension(const std::string& path) {
        return HasExtension(fs::path(path));
    }

    std::string GetExtension(const std::string& path) {
        return GetExtension(fs::path(path));
    }

    std::string GetExtension(const fs::path& path) {
        return path.extension().string();
    }

    bool HasExtension(const fs::path& path) {
        std::string extension = GetExtension(path);
        return !(extension.empty() || extension == ".");
    }

    std::string GetStem(const std::string& path) {
        return GetStem(fs::path(path));
    }

    std::string GetStem(const fs::path& path) {
        if (!HasExtension(path)) {
            return "";
        }
        return path.stem().string();
    }
}
