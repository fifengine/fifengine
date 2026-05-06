// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_VFSHOSTSYSTEM_H
#define FIFE_VFS_VFSHOSTSYSTEM_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <set>
#include <string>

// 3rd party library includes

// FIFE includes
#include "vfssource.h"

namespace FIFE
{

    /**
     * The most basic VFSSource for "normal" filesystems.
     * For example, '/' or './tests/data'.
     */
    class FIFE_API VFSDirectory : public VFSSource
    {
        public:
            /** Constructor
             * Creates the given file system's VFS Source.
             */
            explicit VFSDirectory(VFS* vfs, std::string root = "./");
            /** Destructor
             */
            ~VFSDirectory() override;

            bool fileExists(std::string const & filename) const override;
            RawData* open(std::string const & filename) const override;

            std::set<std::string> listFiles(std::string const & path) const override;

            std::set<std::string> listDirectories(std::string const & path) const override;

        private:
            std::string m_root;

            std::set<std::string> list(std::string const & path, bool directorys) const;
    };

} // namespace FIFE

#endif
