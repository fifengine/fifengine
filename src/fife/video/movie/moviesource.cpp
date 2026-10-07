// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "moviesource.h"

// Standard C++ library includes
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Platform specific includes

// 3rd party library includes
extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/log.h>
#include <libavutil/mem.h>
}

// FIFE includes
#include "modules.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"

namespace FIFE
{
    namespace
    {
        constexpr int AVIO_BUFFER_SIZE = 32768;

        [[maybe_unused]] Logger& _log()
        {
            static Logger log(LM_MOVIE);
            return log;
        }

        /** Routes libav diagnostics into FIFE's logger.
         *
         * Without this libav writes straight to stderr, which bypasses the log
         * level settings and floods the output when probing a file that turns
         * out not to be a movie.
         */
        void logBridge(void*, int level, char const * format, va_list args)
        {
            if (format == nullptr) {
                return;
            }
            char message[1024];
            int const written = vsnprintf(message, sizeof(message), format, args);
            if (written < 0) {
                return;
            }
            if (level <= AV_LOG_ERROR) {
                FL_ERR(_log(), message);
            } else if (level <= AV_LOG_WARNING) {
                FL_WARN(_log(), message);
            } else {
                FL_DBG(_log(), message);
            }
        }

        void installLogBridge()
        {
            static bool const installed = []() {
                av_log_set_callback(logBridge);
                // Errors and above still reach FIFE; libav's info/debug chatter does not.
                av_log_set_level(AV_LOG_ERROR);
                return true;
            }();
            (void)installed;
        }

        /** Per-decoder read cursor over the shared bytes.
         *
         * Each decoder needs its own position, since the video and audio
         * demuxers advance independently.
         */
        struct AvioCursor
        {
                std::vector<uint8_t> const * bytes;
                int64_t pos{0};
        };

        int readPacket(void* opaque, uint8_t* buf, int bufSize)
        {
            auto* cursor       = static_cast<AvioCursor*>(opaque);
            int64_t const size = static_cast<int64_t>(cursor->bytes->size());
            if (cursor->pos >= size) {
                return AVERROR_EOF;
            }
            int64_t const count = std::min(size - cursor->pos, static_cast<int64_t>(bufSize));
            std::memcpy(buf, cursor->bytes->data() + cursor->pos, static_cast<size_t>(count));
            cursor->pos += count;
            return static_cast<int>(count);
        }

        int64_t seek(void* opaque, int64_t offset, int whence)
        {
            auto* cursor       = static_cast<AvioCursor*>(opaque);
            int64_t const size = static_cast<int64_t>(cursor->bytes->size());

            if (whence == AVSEEK_SIZE) {
                return size;
            }
            int64_t target = 0;
            if (whence == SEEK_SET) {
                target = offset;
            } else if (whence == SEEK_CUR) {
                target = cursor->pos + offset;
            } else {
                target = size + offset;
            }
            if (target < 0 || target > size) {
                return -1;
            }
            cursor->pos = target;
            return target;
        }
    } // namespace

    struct MovieSource::Impl
    {
            std::vector<uint8_t> bytes;
            // One cursor per openFormatContext call, owned here so it outlives
            // the decoders that reference it.
            std::vector<std::unique_ptr<AvioCursor>> cursors;
    };

    MovieSource::MovieSource(std::string const & path) : m_impl(std::make_unique<Impl>())
    {
        // Must happen before any libav call, so diagnostics do not leak to stderr.
        installLogBridge();

        // VFS::open throws NotFound when the file is missing.
        std::unique_ptr<RawData> data = VFS::instance()->open(path);
        m_impl->bytes                 = data->getDataInBytes();
        if (m_impl->bytes.empty()) {
            throw InvalidFormat("movie is empty: " + path);
        }
    }

    MovieSource::~MovieSource() = default;

    int64_t MovieSource::size() const
    {
        return static_cast<int64_t>(m_impl->bytes.size());
    }

    void MovieSource::openFormatContext(AVFormatContext*& format, std::string const & path) const
    {
        format = avformat_alloc_context();
        if (format == nullptr) {
            throw InvalidFormat("could not allocate movie format context");
        }

        auto cursor    = std::make_unique<AvioCursor>();
        cursor->bytes  = &m_impl->bytes;
        void* userdata = cursor.get();

        // avio_alloc_context takes ownership of the buffer and frees it when the
        // context is closed, so it must come from av_malloc.
        auto* buffer = static_cast<uint8_t*>(av_malloc(AVIO_BUFFER_SIZE));
        if (buffer == nullptr) {
            avformat_free_context(format);
            format = nullptr;
            throw InvalidFormat("could not allocate movie IO buffer");
        }
        AVIOContext* avio = avio_alloc_context(buffer, AVIO_BUFFER_SIZE, 0, userdata, readPacket, nullptr, seek);
        if (avio == nullptr) {
            avformat_free_context(format);
            format = nullptr;
            throw InvalidFormat("could not allocate movie IO context");
        }
        // Keep the cursor alive for as long as this source exists.
        m_impl->cursors.push_back(std::move(cursor));
        format->pb = avio;

        if (avformat_open_input(&format, path.c_str(), nullptr, nullptr) < 0) {
            if (format != nullptr) {
                avformat_close_input(&format);
            } else {
                av_freep(&avio->buffer);
                avio_context_free(&avio);
            }
            throw InvalidFormat("could not open movie: " + path);
        }

        if (avformat_find_stream_info(format, nullptr) < 0) {
            avformat_close_input(&format);
            throw InvalidFormat("could not read movie stream info: " + path);
        }
    }
} // namespace FIFE
