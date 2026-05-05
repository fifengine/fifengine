// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_DIRECTORYPROVIDER_H
#define FIFE_VFS_DIRECTORYPROVIDER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <map>
#include <string>

// 3rd party library includes

// FIFE includes
#include "vfssourceprovider.h"

namespace FIFE
{

    class VFSSource;

    /** Provider for OS directories.
     */
    class /*FIFE_API*/ DirectoryProvider : public VFSSourceProvider
    {
        public:
            DirectoryProvider() : VFSSourceProvider("OS Directory")
            {
            }

            /** Check if a given directory is readable.
             * The path must either be an absolute path or relative
             * to the current working directory where the client was launched
             *
             * @param path the filename to check
             * @return true if readable, false otherwise
             */
            bool isReadable(std::string const & path) const override;

            VFSSource* createSource(std::string const & path) override;

            VFSSource* getSource(std::string const & path) const override;

            bool hasSource(std::string const & path) const override;

        private:
            std::string m_name;
            std::map<std::string, VFSSource*> m_sources;
    };
} // namespace FIFE

#endif
