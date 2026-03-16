// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_LZSSDECODER_H
#define FIFE_LZSSDECODER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
#include "util/base/fife_stdint.h"
#include "vfs/vfs.h"

namespace FIFE
{
    /** A helper class for unpacking LZSS encoded data
     * @bug just a quick&dirty LZSS implementation, optimized a bit
     *      see revision 719 for the original one and 795 for the broken optimized one
     * @bug the lzss decoder might very well crash on corrupt data.
     */
    class LZSSDecoder
    {
    public:
        /** Constructor
         */
        LZSSDecoder();

        /** Destructor
         */
        ~LZSSDecoder();

        /** Decodes from a file into a pointer.
         * @param input The VFS file to read from
         * @param output The memory location to write to
         * @param outputsize The size of the memory location in byte
         */
        void decode(RawData* input, uint8_t* output, const uint32_t outputsize);

    private:
        uint32_t m_outlen;
        uint32_t m_outindex;
        void LZSSDecode(const uint8_t* in, int64_t len, uint8_t* out);
    };

} // namespace FIFE

#endif