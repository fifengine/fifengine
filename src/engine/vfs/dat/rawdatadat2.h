// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MAP_LOADERS_FALLOUT_RAWDATADAT2_H
#define FIFE_MAP_LOADERS_FALLOUT_RAWDATADAT2_H

// Standard C++ library includes
#include <string>

// Platform specific includes
#include "util/base/fife_stdint.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/raw/rawdatamemsource.h"

#include "rawdatadat2.h"

namespace FIFE
{

    /** A subclass of RawDataMemSource, that fills itself with a FALLOUT1 .DAT file entry
     * @see MFFalloutDAT2
     */
    class RawDataDAT2 : public RawDataMemSource
    {
    public:
        /** The needed information for the extraction.
         */
        struct s_info
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
         * @param info The .DAT file entry, as retrieved by MFFalloutDAT2
         */
        RawDataDAT2(VFS* vfs, const std::string& datfile, const s_info& info);
    };
} // namespace FIFE
#endif