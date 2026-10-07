// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "moviedecoder.h"

// Standard C++ library includes
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <format>
#include <memory>
#include <string>
#include <vector>

// Platform specific includes

// 3rd party library includes
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libswscale/swscale.h>
}

// FIFE includes
#include "modules.h"
#include "moviesource.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"

namespace FIFE
{
    namespace
    {
        /** Logger to use for this source file.
         *  @relates Logger
         */
        [[maybe_unused]] Logger& _log()
        {
            static Logger log(LM_MOVIE);
            return log;
        }

        [[noreturn]] void fail(char const * stage, char const * detail)
        {
            throw InvalidFormat(std::string("movie ") + stage + " failed: " + detail);
        }
    } // namespace

    /** Holds every libav handle so the public header stays free of ffmpeg types. */
    class MovieDecoder_Impl
    {
        public:
            std::shared_ptr<MovieSource> source;
            AVFormatContext* format{nullptr};
            AVCodecContext* codec{nullptr};
            AVStream* stream{nullptr};
            AVFrame* decoded{nullptr};
            AVFrame* rgba{nullptr};
            AVPacket* packet{nullptr};
            SwsContext* scaler{nullptr};
            double timeBase{0.0};
            double frameRate{0.0};
            int32_t videoStream{-1};
            bool draining{false};
            double timestamp{0.0};

            ~MovieDecoder_Impl()
            {
                if (scaler != nullptr) {
                    sws_freeContext(scaler);
                }
                if (packet != nullptr) {
                    av_packet_free(&packet);
                }
                if (decoded != nullptr) {
                    av_frame_free(&decoded);
                }
                if (rgba != nullptr) {
                    av_frame_free(&rgba);
                }
                if (codec != nullptr) {
                    avcodec_free_context(&codec);
                }
                // Also frees the AVIOContext and its buffer.
                if (format != nullptr) {
                    avformat_close_input(&format);
                }
            }

            std::shared_ptr<MovieSource> getSource() const
            {
                return source;
            }

            void open(std::string const & path, std::shared_ptr<MovieSource> const & shared)
            {
                source = shared != nullptr ? shared : std::make_shared<MovieSource>(path);

                // MovieSource owns the AVIOContext once the format context is open.
                source->openFormatContext(format, path);
                if (format == nullptr) {
                    fail("open", path.c_str());
                }

                videoStream = av_find_best_stream(format, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
                if (videoStream < 0) {
                    fail("no video stream", path.c_str());
                }
                stream = format->streams[videoStream];

                AVCodec const * decoder = avcodec_find_decoder(stream->codecpar->codec_id);
                if (decoder == nullptr) {
                    fail("unsupported codec", path.c_str());
                }
                codec = avcodec_alloc_context3(decoder);
                if (codec == nullptr) {
                    fail("allocate decoder", path.c_str());
                }
                if (avcodec_parameters_to_context(codec, stream->codecpar) < 0) {
                    fail("decoder parameters", path.c_str());
                }
                if (avcodec_open2(codec, decoder, nullptr) < 0) {
                    fail("open decoder", path.c_str());
                }

                // time_base is the tick rate of the timestamps, not the frame period: WebM
                // uses 1/1000, so the frame rate has to come from the stream.
                timeBase              = av_q2d(stream->time_base);
                AVRational const rate = stream->avg_frame_rate;
                if (rate.num > 0 && rate.den > 0) {
                    frameRate = av_q2d(rate);
                } else {
                    frameRate = av_q2d(av_guess_frame_rate(format, stream, nullptr));
                }

                decoded = av_frame_alloc();
                packet  = av_packet_alloc();
                if (decoded == nullptr || packet == nullptr) {
                    throw InvalidFormat("could not allocate movie frames");
                }

                rgba = av_frame_alloc();
                if (rgba == nullptr) {
                    throw InvalidFormat("could not allocate movie frame");
                }
                rgba->format = AV_PIX_FMT_RGBA;
                rgba->width  = codec->width;
                rgba->height = codec->height;
                if (av_image_alloc(rgba->data, rgba->linesize, codec->width, codec->height, AV_PIX_FMT_RGBA, 32) < 0) {
                    fail("allocate pixel buffer", path.c_str());
                }

                scaler = sws_getContext(
                    codec->width,
                    codec->height,
                    codec->pix_fmt,
                    codec->width,
                    codec->height,
                    AV_PIX_FMT_RGBA,
                    SWS_BILINEAR,
                    nullptr,
                    nullptr,
                    nullptr);
                if (scaler == nullptr) {
                    fail("create scaler", path.c_str());
                }

                FL_LOG(
                    _log(),
                    std::format(
                        "opened movie {} ({}x{}, {:.5f}s timebase)", path, codec->width, codec->height, timeBase));
            }

            uint32_t width() const
            {
                return codec != nullptr ? static_cast<uint32_t>(codec->width) : 0U;
            }

            uint32_t height() const
            {
                return codec != nullptr ? static_cast<uint32_t>(codec->height) : 0U;
            }

            double duration() const
            {
                if (stream != nullptr && stream->duration > 0) {
                    return static_cast<double>(stream->duration) * timeBase;
                }
                if (format != nullptr && format->duration > 0) {
                    return static_cast<double>(format->duration) / static_cast<double>(AV_TIME_BASE);
                }
                return 0.0;
            }

            bool nextFrame()
            {
                for (;;) {
                    if (draining) {
                        return false;
                    }
                    int const read = av_read_frame(format, packet);
                    if (read < 0) {
                        // Flush the decoder so trailing frames are not lost.
                        draining = true;
                        if (avcodec_send_packet(codec, nullptr) < 0) {
                            return false;
                        }
                    } else if (packet->stream_index != videoStream) {
                        av_packet_unref(packet);
                        continue;
                    } else if (avcodec_send_packet(codec, packet) < 0) {
                        av_packet_unref(packet);
                        continue;
                    } else {
                        av_packet_unref(packet);
                    }

                    int const got = avcodec_receive_frame(codec, decoded);
                    if (got == AVERROR(EAGAIN)) {
                        continue;
                    }
                    if (got < 0) {
                        return false;
                    }

                    timestamp = static_cast<double>(decoded->best_effort_timestamp) * timeBase;
                    sws_scale(scaler, decoded->data, decoded->linesize, 0, codec->height, rgba->data, rgba->linesize);
                    return true;
                }
            }

            bool seekTo(double seconds)
            {
                if (seconds <= 0.0) {
                    if (av_seek_frame(format, videoStream, 0, AVSEEK_FLAG_BACKWARD) < 0) {
                        return false;
                    }
                } else {
                    int64_t const target = static_cast<int64_t>(seconds / timeBase + 0.5);
                    if (av_seek_frame(format, videoStream, target, AVSEEK_FLAG_BACKWARD) < 0) {
                        return false;
                    }
                }
                avcodec_flush_buffers(codec);
                draining = false;
                return true;
            }
    };

    MovieDecoder::MovieDecoder(std::string const & path) : MovieDecoder(path, nullptr)
    {
    }

    MovieDecoder::MovieDecoder(std::string const & path, std::shared_ptr<MovieSource> const & source) :
        m_impl(std::make_unique<MovieDecoder_Impl>())
    {
        m_impl->open(path, source);
    }

    std::shared_ptr<MovieSource> MovieDecoder::getSource() const
    {
        return m_impl->getSource();
    }

    MovieDecoder::~MovieDecoder() = default;

    uint32_t MovieDecoder::getWidth() const
    {
        return m_impl->width();
    }

    uint32_t MovieDecoder::getHeight() const
    {
        return m_impl->height();
    }

    double MovieDecoder::getFrameRate() const
    {
        return m_impl->frameRate;
    }

    double MovieDecoder::getDuration() const
    {
        return m_impl->duration();
    }

    bool MovieDecoder::nextFrame()
    {
        return m_impl->nextFrame();
    }

    bool MovieDecoder::seek(double seconds)
    {
        return m_impl->seekTo(seconds);
    }

    double MovieDecoder::getFrameTimestamp() const
    {
        return m_impl->timestamp;
    }

    uint8_t const * MovieDecoder::getFrameData() const
    {
        if (m_impl->rgba == nullptr || m_impl->rgba->data[0] == nullptr) {
            return nullptr;
        }
        return m_impl->rgba->data[0];
    }
} // namespace FIFE
