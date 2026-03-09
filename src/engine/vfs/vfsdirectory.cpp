// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <set>
#include <string>
#include <utility>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "vfs/raw/rawdata.h"
#include "vfs/raw/rawdatafile.h"

#include "filesystem.h"
#include "vfsdirectory.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_VFS);

    VFSDirectory::VFSDirectory(VFS* vfs, std::string  root) : VFSSource(vfs), m_root(std::move(root))
    {
        FL_DBG(_log, LMsg("VFSDirectory created with root path ") << m_root);
        if (!m_root.empty() && *(m_root.end() - 1) != '/') {
            m_root.append(1, '/');
        }
    }

    VFSDirectory::~VFSDirectory() = default;

    bool VFSDirectory::fileExists(const std::string& name) const
    {
        std::string filename = m_root + name;

        fs::path path(filename);
        std::ifstream file(path.string().c_str());

        if (file) {
            return true;
        }

        return false;
    }

    RawData* VFSDirectory::open(const std::string& file) const
    {
        return new RawData(new RawDataFile(m_root + file));
    }

    std::set<std::string> VFSDirectory::listFiles(const std::string& path) const
    {
        return list(path, false);
    }

    std::set<std::string> VFSDirectory::listDirectories(const std::string& path) const
    {
        return list(path, true);
    }

    std::set<std::string> VFSDirectory::list(const std::string& path, bool directorys) const
    {
        std::set<std::string> list;
        std::string dir = m_root;

        // Avoid double slashes
        if (path[0] == '/' && m_root[m_root.size() - 1] == '/') {
            dir.append(path.substr(1));
        } else {
            dir.append(path);
        }

        try {
            fs::path path(dir);
            if (!fs::exists(path) || !fs::is_directory(path)) {
                return list;
            }

            fs::directory_iterator end;
            for (fs::directory_iterator i(path); i != end; ++i) {
                if (fs::is_directory(*i) != directorys) {
                    continue;
                }

                std::string filename = GetFilenameFromDirectoryIterator(i);
                if (!filename.empty()) {
                    list.insert(filename);
                }
            }
        } catch (const fs::filesystem_error& ex) {
            throw Exception(ex.what());
        }

        return list;
    }
} // namespace FIFE
