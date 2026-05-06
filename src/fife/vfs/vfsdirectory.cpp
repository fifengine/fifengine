// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "vfsdirectory.h"

// Standard C++ library includes
#include <set>
#include <string>
#include <utility>

// 3rd party library includes

// FIFE includes
#include "filesystem.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "vfs/raw/rawdata.h"
#include "vfs/raw/rawdatafile.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_VFS);

    VFSDirectory::VFSDirectory(VFS* vfs, std::string root) : VFSSource(vfs), m_root(std::move(root))
    {
        FL_DBG(_log, LMsg("VFSDirectory created with root path ") << m_root);
        if (!m_root.empty() && *(m_root.end() - 1) != '/') {
            m_root.append(1, '/');
        }
    }

    VFSDirectory::~VFSDirectory() = default;

    bool VFSDirectory::fileExists(std::string const & name) const
    {
        std::string const filename = m_root + name;

        fs::path const path(filename);
        std::ifstream file(path.string().c_str());

        return file.is_open();
    }

    RawData* VFSDirectory::open(std::string const & file) const
    {
        return new RawData(new RawDataFile(m_root + file));
    }

    std::set<std::string> VFSDirectory::listFiles(std::string const & path) const
    {
        return list(path, false);
    }

    std::set<std::string> VFSDirectory::listDirectories(std::string const & path) const
    {
        return list(path, true);
    }

    std::set<std::string> VFSDirectory::list(std::string const & path, bool directorys) const
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
            fs::path const dirPath(dir);
            if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
                return list;
            }

            fs::directory_iterator const end;
            for (fs::directory_iterator i(dirPath); i != end; ++i) {
                if (fs::is_directory(*i) != directorys) {
                    continue;
                }

                std::string const filename = GetFilenameFromDirectoryIterator(i);
                if (!filename.empty()) {
                    list.insert(filename);
                }
            }
        } catch (fs::filesystem_error const & ex) {
            throw Exception(ex.what());
        }

        return list;
    }
} // namespace FIFE
