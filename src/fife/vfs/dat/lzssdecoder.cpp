// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "lzssdecoder.h"
#include "util/base/exception.h"
#include "vfs/raw/rawdata.h"

namespace FIFE
{

    LZSSDecoder::LZSSDecoder() : m_outlen(0), m_outindex(0)
    {
    }

    void LZSSDecoder::decode(RawData* input, uint8_t* output, uint32_t const outputsize)
    {

        m_outindex = 0;
        m_outlen   = outputsize;

        while (m_outindex < outputsize) {
            // Block header: signed 16-bit big-endian integer N.
            //   N == 0: end of stream
            //   N < 0:  raw block, copy -N bytes as-is
            //   N > 0:  LZSS compressed block, next N bytes are compressed
            int16_t const n = static_cast<int16_t>(input->read16Big());

            if (n == 0) {
                break;
            }

            if (n < 0) {
                uint16_t const rawSize = static_cast<uint16_t>(-n);
                input->readInto(output + m_outindex, rawSize);
                m_outindex += rawSize;
            } else {
                // Allocate +2 bytes so that on corrupt data the LZSS
                // decoder won't crash the input buffer.
                std::vector<uint8_t> indata(static_cast<size_t>(n) + 2U);
                input->readInto(indata.data(), static_cast<uint32_t>(n));
                LZSSDecode(indata.data(), n, output);
                // Note outindex is advanced inside LZSSDecode.
            }
        }
    }

    void LZSSDecoder::LZSSDecode(uint8_t const * in, int64_t len, uint8_t* out)
    {
        constexpr int64_t kRingBufferSize        = 4096;
        constexpr int64_t kMatchLengthUpperLimit = 18;
        constexpr int64_t kThreshold             = 2;

        std::array<char, kRingBufferSize + kMatchLengthUpperLimit - 1> buffer{};
        int32_t ibuf = 0;
        int32_t c    = 0;

        int32_t i = 0;
        int32_t j = 0;
        int32_t k = 0;
        int32_t r = 0;

        uint32_t flags = 0;

        for (i = 0; i < kRingBufferSize - kMatchLengthUpperLimit; i++) {
            buffer[static_cast<size_t>(i)] = ' ';
        }

        r     = kRingBufferSize - kMatchLengthUpperLimit;
        flags = 0;
        while (ibuf < len) {
            flags >>= 1;
            if ((flags & 256) == 0) {
                c     = in[ibuf++];
                flags = static_cast<uint32_t>(c | 0xff00); /* uses higher byte cleverly to count eight */
            }

            if ((flags & 1) != 0U) {
                c                 = in[ibuf++];
                out[m_outindex++] = static_cast<uint8_t>(c);

                buffer[static_cast<size_t>(r++)] = static_cast<char>(c);
                r &= (kRingBufferSize - 1);
            } else {
                i = in[ibuf++];
                j = in[ibuf++];

                i |= ((j & 0xf0) << 4);
                j = (j & 0x0f) + kThreshold;

                for (k = 0; k <= j; k++) {
                    c = static_cast<unsigned char>(buffer[(i + k) & (kRingBufferSize - 1)]);

                    out[m_outindex++] = static_cast<uint8_t>(c);

                    buffer[static_cast<size_t>(r++)] = static_cast<char>(c);
                    r &= (kRingBufferSize - 1);
                }
            }
        }
    }

} // namespace FIFE
