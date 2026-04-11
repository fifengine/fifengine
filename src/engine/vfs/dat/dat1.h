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
#include "vfs/vfs.h"
#include "vfs/vfssource.h"

#include "rawdatadat1.h"

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
        DAT1(VFS* vfs, const std::string& file);

        bool fileExists(const std::string& name) const;
        RawData* open(const std::string& file) const;

        /** Get the needed information to unpack and extract a file from the
         * DAT file.
         * Retrieves the Information needed to extract a specific file, this
         * is passed as argument to a mffalloutrawdatadat1 RawMemSource,
         * which itself fills its memory content with the unpacked file.
         *
         *  @throw NotFound
         *  @see MFFalloutRawDataDAT1, RawMemSource
         */
        const RawDataDAT1::s_info& getInfo(const std::string& name) const;

        std::set<std::string> listFiles(const std::string& pathstr) const;
        std::set<std::string> listDirectories(const std::string& pathstr) const;

    private:
        std::string m_datpath;
        std::unique_ptr<RawData> m_data;
        using type_filelist = std::map<std::string, RawDataDAT1::s_info>;
        type_filelist m_filelist;

        std::set<std::string> list(const std::string& pathstr, bool dirs) const;
        std::string readString();

        void loadFileList(const std::string& dirname);

        // Not copyable
        DAT1(const DAT1&);
        DAT1& operator=(const DAT1&);
    };

} // namespace FIFE

#endif
