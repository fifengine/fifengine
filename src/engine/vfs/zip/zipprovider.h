// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_ZIP_PROVIDER_H
#define FIFE_ZIP_PROVIDER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <map>
#include <string>

// 3rd party library includes

// FIFE includes
#include "vfs/vfssourceprovider.h"

namespace FIFE
{
    class VFSSource;
}

namespace FIFE
{
    /**  A VFS provider for Zip archives.
     *
     * @see FIFE::VFSSourceProvider
     */
    class /*FIFE_API*/ ZipProvider : public VFSSourceProvider
    {
        public:
            ZipProvider() : VFSSourceProvider("ZIP")
            {
            }

            bool isReadable(std::string const & file) const;
            VFSSource* createSource(std::string const & file);
            VFSSource* getSource(std::string const & path) const;
            virtual bool hasSource(std::string const & path) const;

        private:
            std::map<std::string, VFSSource*> m_sources;
    };
} // namespace FIFE

#endif
