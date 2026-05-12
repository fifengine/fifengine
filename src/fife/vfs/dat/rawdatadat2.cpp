// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "rawdatadat2.h"

// Standard C++ library includes
#include <algorithm>
#include <memory>
#include <string>

// 3rd party library includes
#include <zlib.h>

// FIFE includes
#include "util/base/exception.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"

namespace FIFE
{

    RawDataDAT2::RawDataDAT2(VFS* vfs, std::string const & datfile, s_info const & info) :
        RawDataMemSource(info.unpackedLength)
    {

        std::unique_ptr<RawData> input(vfs->open(datfile));
        input->setIndex(info.offset);

        if (info.type == 1) { // compressed
            std::unique_ptr<uint8_t[]> const compressed(new uint8_t[info.packedLength]);
            input->readInto(compressed.get(), info.packedLength);

            uLongf dstlen = info.unpackedLength;
            if (uncompress(getRawData(), &dstlen, compressed.get(), info.packedLength) != Z_OK ||
                dstlen != info.unpackedLength) {
                throw InvalidFormat("failed to decompress " + info.name + " (inside: " + datfile + ")");
            }
        } else {
            input->readInto(getRawData(), info.unpackedLength);
        }
    }

} // namespace FIFE
