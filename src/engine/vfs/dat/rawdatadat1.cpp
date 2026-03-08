// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <memory>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "vfs/raw/rawdata.h"

#include "lzssdecoder.h"
#include "rawdatadat1.h"

namespace FIFE
{

    RawDataDAT1::RawDataDAT1(VFS* vfs, const std::string& datfile, const s_info& info) :
        RawDataMemSource(info.unpackedLength)
    {

        std::unique_ptr<RawData> input(vfs->open(datfile));
        input->setIndex(info.offset);

        if (info.type == 0x40) { // compressed
            LZSSDecoder decoder;
            decoder.decode(input.get(), getRawData(), getSize());
        } else
            input->readInto(getRawData(), getSize());
    }
} // namespace FIFE