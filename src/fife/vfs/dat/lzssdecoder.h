// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_LZSSDECODER_H
#define FIFE_LZSSDECODER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
// 3rd party library includes

// FIFE includes
#include "util/base/fife_stdint.h"
#include "vfs/vfs.h"

namespace FIFE
{
    /**
     * The LZSSDecoder class is responsible for unpacking LZSS encoded data.
     *
     * Implements the Fallout 1 LZSS decompression algorithm.
     * Block headers are signed 16-bit big-endian integers:
     *   N == 0: end of stream
     *   N < 0:  raw block, copy -N bytes as-is
     *   N > 0:  LZSS compressed block, next N bytes are compressed
     */
    class FIFE_API LZSSDecoder
    {
        public:
            /**
             * Constructor
             */
            LZSSDecoder();

            /**
             * Destructor
             */
            ~LZSSDecoder() = default;

            /**
             * Decodes from a file into a pointer.
             *
             * @param input The VFS file to read from
             * @param output The memory location to write to
             * @param outputsize The size of the memory location in byte
             */
            void decode(RawData* input, uint8_t* output, uint32_t outputsize);

        private:
            uint32_t m_outlen;
            uint32_t m_outindex;
            void LZSSDecode(uint8_t const * in, int64_t len, uint8_t* out);
    };

} // namespace FIFE

#endif
