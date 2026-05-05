// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_ZIP_SOURCE_H
#define FIFE_ZIP_SOURCE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <set>
#include <string>

// 3rd party library includes

// FIFE includes
#include "vfs/vfssource.h"
#include "ziptree.h"

namespace FIFE
{
    class RawData;
}
namespace FIFE
{
    class VFS;
}

namespace FIFE
{
    /**  Implements a Zip archive file source.
     *
     * @see FIFE::VFSSource
     */
    class /*FIFE_API*/ ZipSource : public VFSSource
    {
        public:
            ZipSource(VFS* vfs, std::string const & zip_file);
            ~ZipSource();

            ZipSource(ZipSource const &)            = delete;
            ZipSource& operator=(ZipSource const &) = delete;

            /// WARNING: fileExists, listFiles and listDirectories are not
            // thread-safe, and will probably break if called from multiple
            // threads at the same time.
            bool fileExists(std::string const & file) const;
            std::set<std::string> listFiles(std::string const & path) const;
            std::set<std::string> listDirectories(std::string const & path) const;

            virtual RawData* open(std::string const & path) const;

        private:
            void readIndex();
            bool readFileToIndex();

            ZipTree m_zipTree;
            RawData* m_zipfile;
    };

} // namespace FIFE

#endif
