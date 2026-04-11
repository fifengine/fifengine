// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MAP_LOADERS_FALLOUT_RAWDATADAT1_H
#define FIFE_MAP_LOADERS_FALLOUT_RAWDATADAT1_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "vfs/raw/rawdatamemsource.h"
#include "vfs/vfs.h"

namespace FIFE
{

    /** A subclass of RawDataMemSource, that fills itself with a FALLOUT1 .DAT file entry
     * @see MFFalloutDAT1
     */
    class FIFE_API RawDataDAT1 : public RawDataMemSource
    {
    public:
        /** The needed information for the extraction.
         */
        struct FIFE_API s_info
        {
            std::string name;
            uint32_t packedLength;
            uint32_t unpackedLength;
            uint32_t offset;
            uint32_t type;
        };

        /** Constructor
         * @param vfs The vfs to open with.
         * @param datfile The .DAT archive - e.g. master.DAT
         * @param info The .DAT file entry, as retrieved by MFFalloutDAT1
         */
        RawDataDAT1(VFS* vfs, const std::string& datfile, const s_info& info);

        RawDataDAT1(const RawDataDAT1&)            = delete;
        RawDataDAT1& operator=(const RawDataDAT1&) = delete;
    };

} // namespace FIFE
#endif
