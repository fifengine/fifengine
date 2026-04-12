// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MAP_LOADERS_FALLOUT_DAT2_H
#define FIFE_MAP_LOADERS_FALLOUT_DAT2_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include "util/base/fife_stdint.h"
#include <map>
#include <memory>
#include <set>
#include <string>

// 3rd party library includes

// FIFE includes
#include "util/time/timer.h"
#include "vfs/vfs.h"
#include "vfs/vfssource.h"

#include "rawdatadat2.h"

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
        DAT2(VFS* vfs, const std::string& file);

        bool fileExists(const std::string& name) const;
        RawData* open(const std::string& file) const;

        /** Get Information needed to unpack and extract data
         *
         * @see MFFalloutDAT1::getInfo
         */
        const RawDataDAT2::s_info& getInfo(const std::string& name) const;

        std::set<std::string> listFiles(const std::string& pathstr) const;
        std::set<std::string> listDirectories(const std::string& pathstr) const;

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
        type_filelist::const_iterator findFileEntry(const std::string& path) const;

        std::set<std::string> list(const std::string& pathstr, bool dirs) const;

        // Not copyable
        DAT2(const DAT2&);
        DAT2& operator=(const DAT2&);
    };

} // namespace FIFE

#endif
