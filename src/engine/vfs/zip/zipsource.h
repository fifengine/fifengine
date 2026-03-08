// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_ZIP_SOURCE_H
#define FIFE_ZIP_SOURCE_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
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
    class ZipSource : public VFSSource
    {
    public:
        ZipSource(VFS* vfs, const std::string& zip_file);
        ~ZipSource();

        /// WARNING: fileExists, listFiles and listDirectories are not
        // thread-safe, and will probably break if called from multiple
        // threads at the same time.
        bool fileExists(const std::string& file) const;
        std::set<std::string> listFiles(const std::string& path) const;
        std::set<std::string> listDirectories(const std::string& path) const;

        virtual RawData* open(const std::string& path) const;

    private:
        void readIndex();
        bool readFileToIndex();

    private:
        ZipTree m_zipTree;
        RawData* m_zipfile;
    };

} // namespace FIFE

#endif