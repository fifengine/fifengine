// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MAP_LOADERS_FALLOUT_DAT2_H
#define FIFE_MAP_LOADERS_FALLOUT_DAT2_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <map>
#include <memory>
#include <set>
#include <string>

#include "util/base/fife_stdint.h"

// 3rd party library includes

// FIFE includes
#include "rawdatadat2.h"
#include "util/time/timer.h"
#include "vfs/vfs.h"
#include "vfs/vfssource.h"

namespace FIFE
{
    class RawData;

    /** VFSource for the Fallout2 DAT file format
     *
     *  Implements a kind of lazy initializing, by reading the file list
     *  in chunks. Behaviour is the same as if it wouldn't do this,
     *  but startup is very fast. But a open/fileExists call with a
     *  filename that doesn't exist, does trigger completely loading
     *  the file entries.
     *
     * @see MFFalloutDAT1
     * @todo @b maybe merge common DAT1/DAT2 code in a common base class
     */
    class /*FIFE_API*/ DAT2 : public VFSSource
    {

        public:
            /** Constructor
             * Create a VFSSource for a Fallout2 DAT file.
             * @param vfs A pointer to the VFS.
             * @param path A Fallout2 DAT file - e.g. master.DAT
             */
            DAT2(VFS* vfs, std::string const & file);

            bool fileExists(std::string const & name) const;
            RawData* open(std::string const & file) const;

            /** Get Information needed to unpack and extract data
             *
             * @see MFFalloutDAT1::getInfo
             */
            RawDataDAT2::s_info const & getInfo(std::string const & name) const;

            std::set<std::string> listFiles(std::string const & pathstr) const;
            std::set<std::string> listDirectories(std::string const & pathstr) const;

        private:
            std::string m_datpath;
            mutable std::unique_ptr<RawData> m_data;
            using type_filelist = std::map<std::string, RawDataDAT2::s_info>;
            mutable type_filelist m_filelist;

            /// number of file entries to read
            mutable uint32_t m_filecount;
            /// current index in file
            mutable uint32_t m_currentIndex;
            /// lazy loading timer
            mutable Timer m_timer;

            /// read a bunch of file entries
            void readFileEntry() const;

            /// find a file entry
            type_filelist::const_iterator findFileEntry(std::string const & path) const;

            std::set<std::string> list(std::string const & pathstr, bool dirs) const;

            // Not copyable
            DAT2(const DAT2&);
            DAT2& operator=(const DAT2&);
    };

} // namespace FIFE

#endif
