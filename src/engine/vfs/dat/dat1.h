// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MAP_LOADERS_FALLOUT_DAT1_H
#define FIFE_MAP_LOADERS_FALLOUT_DAT1_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <map>
#include <memory>
#include <set>
#include <string>

// 3rd party library includes

// FIFE includes
#include "rawdatadat1.h"
#include "vfs/vfs.h"
#include "vfs/vfssource.h"

namespace FIFE
{

    /** VFSource for the Fallout1 DAT file format.
     */
    class FIFE_API DAT1 : public VFSSource
    {
        public:
            /** Constructor
             * Create a VFSSource for a Fallout1 DAT file.
             * @param vfs A pointer to the VFS.
             * @param file A Fallout1 DAT file - e.g. master.DAT
             */
            DAT1(VFS* vfs, std::string const & file);

            bool fileExists(std::string const & name) const;
            RawData* open(std::string const & file) const;

            /** Get the needed information to unpack and extract a file from the
             * DAT file.
             * Retrieves the Information needed to extract a specific file, this
             * is passed as argument to a mffalloutrawdatadat1 RawMemSource,
             * which itself fills its memory content with the unpacked file.
             *
             *  @throw NotFound
             *  @see MFFalloutRawDataDAT1, RawMemSource
             */
            RawDataDAT1::s_info const & getInfo(std::string const & name) const;

            std::set<std::string> listFiles(std::string const & pathstr) const;
            std::set<std::string> listDirectories(std::string const & pathstr) const;

        private:
            std::string m_datpath;
            std::unique_ptr<RawData> m_data;
            using type_filelist = std::map<std::string, RawDataDAT1::s_info>;
            type_filelist m_filelist;

            std::set<std::string> list(std::string const & pathstr, bool dirs) const;
            std::string readString();

            void loadFileList(std::string const & dirname);

            // Not copyable
            DAT1(const DAT1&);
            DAT1& operator=(const DAT1&);
    };

} // namespace FIFE

#endif
