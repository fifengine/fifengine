// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_VFSHOSTSYSTEM_H
#define FIFE_VFS_VFSHOSTSYSTEM_H

// Standard C++ library includes
#include <set>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfssource.h"

namespace FIFE
{

    /**
     * The most basic VFSSource for "normal" filesystems.
     * For example, '/' or './tests/data'.
     */
    class VFSDirectory : public VFSSource
    {
    public:
        /** Constructor
         * Creates the given file system's VFS Source.
         */
        explicit VFSDirectory(VFS* vfs, std::string root = "./");
        /** Destructor
         */
        ~VFSDirectory() override;

        bool fileExists(const std::string& filename) const override;
        RawData* open(const std::string& filename) const override;

        std::set<std::string> listFiles(const std::string& path) const override;

        std::set<std::string> listDirectories(const std::string& path) const override;

    private:
        std::string m_root;

        std::set<std::string> list(const std::string& path, bool directorys) const;
    };

} // namespace FIFE

#endif
