// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VFS_VFSSOURCE_H
#define FIFE_VFS_VFSSOURCE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <set>
#include <string>

// 3rd party library includes

// FIFE includes
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
    class /*FIFE_API*/ VFSSource
    {
        public:
            explicit VFSSource(VFS* vfs);
            virtual ~VFSSource();

            /** check if the given file exists
             *
             * @param file filename to check
             * @return true if it exists, false otherwise
             */
            virtual bool fileExists(std::string const & file) const = 0;

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
            virtual RawData* open(std::string const & file) const = 0;

            /** list all files in a directory of this source
             *
             * @param path path to list files in
             * Will return an empty list on \b any error
             */
            virtual std::set<std::string> listFiles(std::string const & path) const = 0;

            /** list all directories in a directory of this source
             *
             * @param path path to list directories in
             * Will return an empty list on \b any error
             */
            virtual std::set<std::string> listDirectories(std::string const & path) const = 0;

        protected:
            std::string fixPath(std::string path) const;

        private:
            VFS* m_vfs;
    };

} // namespace FIFE

#endif
