// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "util/base/exception.h"
#include "vfs/raw/rawdata.h"

#include "lzssdecoder.h"

// just a quick&dirty wrapper around anchorites implementation - needs to be replaced with our own LZSS implementation
namespace FIFE
{

    LZSSDecoder::LZSSDecoder() : m_outlen(0), m_outindex(0) { }
    LZSSDecoder::~LZSSDecoder() = default;

    void LZSSDecoder::decode(RawData* input, uint8_t* output, const uint32_t outputsize)
    {

        m_outindex = 0;
        m_outlen   = outputsize;

        while (m_outindex < outputsize) {
            uint16_t blockdesc   = input->read16Big();
            uint16_t bytesToRead = blockdesc & 0x7fff;

            if ((blockdesc & 0x8000) != 0) { // uncompressed
                input->readInto(output + m_outindex, bytesToRead);
                m_outindex += bytesToRead;
            } else {
                // Allocate +2 bytes so that on corrupt data the LZSS
                // decoder won't crash the input buffer.
                std::vector<uint8_t> indata(bytesToRead + 2);
                input->readInto(&indata[0], bytesToRead);
                LZSSDecode(&indata[0], bytesToRead, output);
                // Note outindex is advanced inside LZSSDecode.
            }
        }
    }

    void LZSSDecoder::LZSSDecode(uint8_t* in, int64_t len, uint8_t* out)
    {
        constexpr int64_t kRingBufferSize        = 4096;
        constexpr int64_t kMatchLengthUpperLimit = 18;
        constexpr int64_t kThreshold             = 2;

        char buffer[kRingBufferSize + kMatchLengthUpperLimit - 1];
        int32_t ibuf = 0;
        int32_t c    = 0;

        int32_t i = 0;
        int32_t j = 0;
        int32_t k = 0;
        int32_t r = 0;

        uint32_t flags = 0;

        for (i = 0; i < kRingBufferSize - kMatchLengthUpperLimit; i++) {
            buffer[i] = ' ';
        }

        r     = kRingBufferSize - kMatchLengthUpperLimit;
        flags = 0;
        while (ibuf < len) {
            flags >>= 1;
            if ((flags & 256) == 0) {
                c     = in[ibuf++];
                flags = c | 0xff00; /* uses higher byte cleverly to count eight */
            }

            if ((flags & 1) != 0U) {
                c                 = in[ibuf++];
                out[m_outindex++] = c;

                buffer[r++] = c;
                r &= (kRingBufferSize - 1);
            } else {
                i = in[ibuf++];
                j = in[ibuf++];

                i |= ((j & 0xf0) << 4);
                j = (j & 0x0f) + kThreshold;

                for (k = 0; k <= j; k++) {
                    c = static_cast<unsigned char>(buffer[(i + k) & (kRingBufferSize - 1)]);

                    out[m_outindex++] = c;

                    buffer[r++] = c;
                    r &= (kRingBufferSize - 1);
                }
            }
        }
    }

} // namespace FIFE
