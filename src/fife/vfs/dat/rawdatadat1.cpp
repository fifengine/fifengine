// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "rawdatadat1.h"

// Standard C++ library includes
#include <memory>
#include <string>

// 3rd party library includes

// FIFE includes
#include "lzssdecoder.h"
#include "util/base/exception.h"
#include "vfs/raw/rawdata.h"

namespace FIFE
{

    RawDataDAT1::RawDataDAT1(VFS* vfs, std::string const & datfile, s_info const & info) :
        RawDataMemSource(info.unpackedLength)
    {

        std::unique_ptr<RawData> input(vfs->open(datfile));
        input->setIndex(info.offset);

        if (info.type == 0x40) { // compressed
            LZSSDecoder decoder;
            decoder.decode(input.get(), getRawData(), getSize());
        } else {
            input->readInto(getRawData(), getSize());
        }
    }
} // namespace FIFE
