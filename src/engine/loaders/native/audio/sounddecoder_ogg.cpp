// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <limits>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/log/logger.h"

#include "sounddecoder_ogg.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_AUDIO);

    /* OggVorbis Callback functions
     */
    namespace OGG_cb
    {
        static size_t read(void* ptr, size_t size, size_t nmemb, void* datasource)
        {
            auto* rdp            = reinterpret_cast<RawData*>(datasource);
            const size_t restlen = rdp->getDataLength() - rdp->getCurrentIndex();
            const size_t len     = (restlen <= size * nmemb) ? restlen : size * nmemb;
            if (len != 0U) {
                rdp->readInto(reinterpret_cast<uint8_t*>(ptr), len);
            }
            return len;
        }

        static int seek(void* datasource, ogg_int64_t offset, int whence)
        {
            auto* rdp                 = reinterpret_cast<RawData*>(datasource);
            const int64_t data_length = static_cast<int64_t>(rdp->getDataLength());

            switch (whence) {
            case SEEK_SET: {
                if (offset < 0 || offset >= data_length) {
                    return -1;
                }
                rdp->setIndex(static_cast<uint32_t>(offset));
                return 0;
            }
            case SEEK_CUR:
                if (offset < static_cast<ogg_int64_t>(std::numeric_limits<int32_t>::min()) ||
                    offset > static_cast<ogg_int64_t>(std::numeric_limits<int32_t>::max())) {
                    return -1;
                }
                rdp->moveIndex(static_cast<int32_t>(offset));
                return 0;
            case SEEK_END: {
                const int64_t target_index = data_length - 1 + static_cast<int64_t>(offset);
                if (target_index < 0 || target_index >= data_length) {
                    return -1;
                }
                rdp->setIndex(static_cast<uint32_t>(target_index));
                return 0;
            }
            default:
                return -1;
            }
        }

        static int close(void* datasource)
        {
            static_cast<void>(datasource);
            return 0;
        }

        static long tell(void* datasource)
        {
            auto* rdp = reinterpret_cast<RawData*>(datasource);
            return static_cast<long>((*rdp).getCurrentIndex());
        }
    } // namespace OGG_cb

    SoundDecoderOgg::SoundDecoderOgg(RawData* rdp) : m_file(rdp), m_ovf{}
    {

        const ov_callbacks ocb = {OGG_cb::read, OGG_cb::seek, OGG_cb::close, OGG_cb::tell};

        if (0 > ov_open_callbacks(m_file.get(), &m_ovf, nullptr, 0, ocb)) {
            throw InvalidFormat("Error opening OggVorbis file");
        }

        const vorbis_info* vi = ov_info(&m_ovf, -1);
        if (vi == nullptr) {
            throw InvalidFormat("Error fetching OggVorbis info");
        }

        if (ov_seekable(&m_ovf) == 0) {
            throw InvalidFormat("OggVorbis file has to be seekable");
        }

        if (vi->rate < 0) {
            throw InvalidFormat("Invalid negative OggVorbis sample rate");
        }

        m_isstereo                  = vi->channels == 2;
        m_samplerate                = static_cast<uint64_t>(vi->rate);
        m_is8bit                    = false;
        const ogg_int64_t pcm_total = ov_pcm_total(&m_ovf, -1);
        if (pcm_total < 0) {
            throw InvalidFormat("Error fetching decoded OggVorbis length");
        }

        m_declength = (m_isstereo ? 2ULL : 1ULL) * 2ULL * static_cast<uint64_t>(pcm_total);
        m_datasize  = 0;
        m_data      = nullptr;
    }

    SoundDecoderOgg::~SoundDecoderOgg()
    {
        releaseBuffer();
        ov_clear(&m_ovf);
    }

    bool SoundDecoderOgg::decode(uint64_t length)
    {
        int32_t stream = 0;
        int32_t ret    = 0;

        // release buffer and allocate new memory
        releaseBuffer();
        m_data = new char[length];

        // decode the stream
        m_datasize = 0;

        while (length - m_datasize > 0) {
            const uint64_t remaining_bytes = length - m_datasize;
            const int chunk_size           = static_cast<int>(
                std::min<uint64_t>(remaining_bytes, static_cast<uint64_t>(std::numeric_limits<int>::max())));

            ret = ov_read(&m_ovf, m_data + m_datasize, chunk_size, 0, 2, 1, &stream);
            if (ret > 0) {
                m_datasize += static_cast<uint64_t>(ret);
            } else if (ret == OV_HOLE) {
                continue;
            } else if (ret == 0 || ret <= OV_EREAD) {
                break;
            }
        }

        (void)ret;
        return m_datasize == 0;
    }

    bool SoundDecoderOgg::setCursor(uint64_t pos)
    {
        const uint64_t sample_position = pos / ((m_isstereo ? 2ULL : 1ULL) * 2ULL);
        if (sample_position > static_cast<uint64_t>(std::numeric_limits<ogg_int64_t>::max())) {
            return false;
        }

        return ov_pcm_seek(&m_ovf, static_cast<ogg_int64_t>(sample_position)) == 0;
    }

    void SoundDecoderOgg::releaseBuffer()
    {
        if (m_data != nullptr) {
            delete[] m_data;
            m_data = nullptr;
        }
    }
} // namespace FIFE
