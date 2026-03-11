// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_VFSSOURCE_H
#define FIFE_VFS_VFSSOURCE_H

// Standard C++ library includes
#include <set>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs.h"

namespace FIFE
{

    class RawData;
    /** VFSSource abstract baseclass
     *
     * VFSSources are used to provide transparent access to diffrent archives/filesystems etc.
     * Examples are VFSHostSystem
     * @see VFS
     * @see VFSSourceProvider
     */
    class VFSSource
    {
    public:
        explicit VFSSource(VFS* vfs);
        virtual ~VFSSource();

        /** check if the given file exists
         *
         * @param file filename to check
         * @return true if it exists, false otherwise
         */
        virtual bool fileExists(const std::string& file) const = 0;

        /** get the VFS this source is associated with.
         */
        VFS* getVFS() const
        {
            return m_vfs;
        }

        /** open a file inside this source
         *
         * @param file the file to open
         * @return a new RawData*
         * @throws CannotOpenFile if the file can't be found
         */
        virtual RawData* open(const std::string& file) const = 0;

        /** list all files in a directory of this source
         *
         * @param path path to list files in
         * Will return an empty list on \b any error
         */
        virtual std::set<std::string> listFiles(const std::string& path) const = 0;

        /** list all directories in a directory of this source
         *
         * @param path path to list directories in
         * Will return an empty list on \b any error
         */
        virtual std::set<std::string> listDirectories(const std::string& path) const = 0;

    protected:
        std::string fixPath(std::string path) const;

    private:
        VFS* m_vfs;
    };

} // namespace FIFE

#endif
