// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "movieaudiodecoder.h"

// Standard C++ library includes
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

// Platform specific includes

// 3rd party library includes
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

// FIFE includes
#include "modules.h"
#include "moviesource.h"
#include "util/base/exception.h"
#include "util/log/logger.h"

namespace FIFE
{
    namespace
    {
        [[maybe_unused]] Logger& _log()
        {
            static Logger log(LM_MOVIE);
            return log;
        }

        /** Output format: what SoundDecoder::getALFormat() expects for stereo 16-bit. */
        int const OUT_CHANNELS         = 2;
        int const OUT_BYTES_PER_SAMPLE = 2;

        uint64_t pcmBytesForSamples(uint64_t samples)
        {
            return samples * static_cast<uint64_t>(OUT_CHANNELS) * static_cast<uint64_t>(OUT_BYTES_PER_SAMPLE);
        }

        uint64_t samplesForPcmBytes(uint64_t bytes)
        {
            return bytes / (static_cast<uint64_t>(OUT_CHANNELS) * static_cast<uint64_t>(OUT_BYTES_PER_SAMPLE));
        }
    } // namespace

    struct MovieAudioDecoder::Impl
    {
            std::shared_ptr<MovieSource> source;
            std::string path;
            AVFormatContext* format{nullptr};
            AVCodecContext* codec{nullptr};
            AVFrame* decoded{nullptr};
            AVPacket* packet{nullptr};
            SwrContext* resampler{nullptr};
            AVChannelLayout outLayout{};
            int audioStream{-1};
            std::vector<uint8_t> buffer;
            std::vector<uint8_t> out;
            uint64_t totalSamples{0};
            uint64_t drainedSamples{0};
            bool draining{false};
            bool flushed{false};

            ~Impl()
            {
                if (resampler != nullptr) {
                    swr_free(&resampler);
                }
                if (packet != nullptr) {
                    av_packet_free(&packet);
                }
                if (decoded != nullptr) {
                    av_frame_free(&decoded);
                }
                if (codec != nullptr) {
                    avcodec_free_context(&codec);
                }
                if (format != nullptr) {
                    avformat_close_input(&format);
                }
                av_channel_layout_uninit(&outLayout);
            }

            void open(std::shared_ptr<MovieSource> const & src, std::string const & p)
            {
                source = src;
                path   = p;
                source->openFormatContext(format, path);
                if (format == nullptr) {
                    throw InvalidFormat("could not open movie audio: " + path);
                }

                audioStream = av_find_best_stream(format, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
                if (audioStream < 0) {
                    throw InvalidFormat("movie has no audio track: " + path);
                }
                AVStream* stream = format->streams[audioStream];

                AVCodec const * decoder = avcodec_find_decoder(stream->codecpar->codec_id);
                if (decoder == nullptr) {
                    throw InvalidFormat("unsupported audio codec: " + path);
                }
                codec = avcodec_alloc_context3(decoder);
                if (codec == nullptr) {
                    throw InvalidFormat("could not allocate audio decoder: " + path);
                }
                if (avcodec_parameters_to_context(codec, stream->codecpar) < 0) {
                    throw InvalidFormat("bad audio parameters: " + path);
                }
                if (avcodec_open2(codec, decoder, nullptr) < 0) {
                    throw InvalidFormat("could not open audio decoder: " + path);
                }

                av_channel_layout_default(&outLayout, OUT_CHANNELS);
                if (!resetResampler()) {
                    throw InvalidFormat("could not create audio resampler: " + path);
                }

                decoded = av_frame_alloc();
                packet  = av_packet_alloc();
                if (decoded == nullptr || packet == nullptr) {
                    throw InvalidFormat("could not allocate audio frames: " + path);
                }

                // Estimate the sample count from the track duration so
                // getDecodedLength() is available before decoding anything.
                double duration = 0.0;
                if (stream->duration > 0) {
                    duration = static_cast<double>(stream->duration) * av_q2d(stream->time_base);
                } else if (format->duration > 0) {
                    duration = static_cast<double>(format->duration) / static_cast<double>(AV_TIME_BASE);
                }
                totalSamples = static_cast<uint64_t>(duration * static_cast<double>(codec->sample_rate));

                FL_LOG(
                    _log(),
                    std::format(
                        "movie audio {}: {} Hz, {} ch, {:.2f}s", path, codec->sample_rate, OUT_CHANNELS, duration));
            }

            /** Decodes PCM until at least wanted bytes are buffered or the track ends. */
            bool fill(uint64_t wanted)
            {
                while (buffer.size() < wanted) {
                    if (draining) {
                        return true;
                    }

                    int const read = av_read_frame(format, packet);
                    if (read < 0) {
                        if (flushed || avcodec_send_packet(codec, nullptr) < 0) {
                            draining = true;
                            return true;
                        }
                        flushed = true;
                    } else if (packet->stream_index != audioStream) {
                        av_packet_unref(packet);
                        continue;
                    } else if (avcodec_send_packet(codec, packet) < 0) {
                        av_packet_unref(packet);
                        draining = true;
                        return true;
                    } else {
                        av_packet_unref(packet);
                    }

                    int const got = avcodec_receive_frame(codec, decoded);
                    if (got == AVERROR(EAGAIN)) {
                        if (flushed) {
                            // Nothing buffered left; stop rather than spin.
                            draining = true;
                            return true;
                        }
                        continue;
                    }
                    if (got < 0) {
                        draining = true;
                        return true;
                    }

                    if (decoded->nb_samples <= 0) {
                        continue;
                    }

                    // Write into a buffer we own: swr_convert's internal output is
                    // only valid until the next call.
                    int const capacity = swr_get_out_samples(resampler, decoded->nb_samples);
                    if (capacity <= 0) {
                        draining = true;
                        return true;
                    }
                    uint8_t** outPtr = nullptr;
                    int linesize     = 0;
                    if (av_samples_alloc_array_and_samples(
                            &outPtr, &linesize, OUT_CHANNELS, capacity, AV_SAMPLE_FMT_S16, 0) < 0) {
                        draining = true;
                        return true;
                    }

                    int const samples =
                        swr_convert(resampler, outPtr, capacity, decoded->extended_data, decoded->nb_samples);
                    if (samples > 0 && outPtr[0] != nullptr) {
                        size_t const bytes = static_cast<size_t>(samples) * static_cast<size_t>(OUT_CHANNELS) *
                                             static_cast<size_t>(OUT_BYTES_PER_SAMPLE);
                        size_t const offset = buffer.size();
                        buffer.resize(offset + bytes);
                        std::memcpy(buffer.data() + offset, outPtr[0], bytes);
                        drainedSamples += static_cast<uint64_t>(samples);
                    }
                    av_freep(&outPtr[0]);
                }
                return false;
            }

            void discard(uint64_t bytes)
            {
                if (bytes >= buffer.size()) {
                    buffer.clear();
                    return;
                }
                buffer.erase(buffer.begin(), buffer.begin() + static_cast<std::ptrdiff_t>(bytes));
            }

            /** Recreates the resampler, which is the safe way to drop its state. */
            bool resetResampler()
            {
                if (resampler != nullptr) {
                    swr_free(&resampler);
                    resampler = nullptr;
                }
                if (codec->sample_rate <= 0) {
                    return false;
                }
                if (swr_alloc_set_opts2(
                        &resampler,
                        &outLayout,
                        AV_SAMPLE_FMT_S16,
                        codec->sample_rate,
                        &codec->ch_layout,
                        codec->sample_fmt,
                        codec->sample_rate,
                        0,
                        nullptr) < 0 ||
                    swr_init(resampler) < 0) {
                    return false;
                }
                return true;
            }

            /** Restarts decoding at the given byte offset. */
            bool rewindTo(uint64_t pos)
            {
                int64_t const sampleTarget = static_cast<int64_t>(samplesForPcmBytes(pos));
                double const sampleRate    = static_cast<double>(codec->sample_rate);
                if (sampleRate <= 0.0) {
                    return false;
                }

                AVRational const tb = format->streams[audioStream]->time_base;
                int64_t const timeTarget =
                    sampleTarget <= 0 ?
                        0 :
                        static_cast<int64_t>(static_cast<double>(sampleTarget) / sampleRate / av_q2d(tb));

                if (av_seek_frame(format, audioStream, timeTarget, AVSEEK_FLAG_BACKWARD) < 0) {
                    return false;
                }
                avcodec_flush_buffers(codec);
                if (!resetResampler()) {
                    return false;
                }
                draining = false;
                buffer.clear();

                // Decode and discard up to the target, keeping any overrun so the
                // next decode continues from the right place.
                uint64_t emitted = 0;
                while (emitted < static_cast<uint64_t>(sampleTarget)) {
                    uint64_t const remaining = static_cast<uint64_t>(sampleTarget) - emitted;
                    if (fill(pcmBytesForSamples(remaining)) || buffer.empty()) {
                        break;
                    }
                    size_t const consume =
                        std::min<size_t>(buffer.size(), static_cast<size_t>(pcmBytesForSamples(remaining)));
                    buffer.erase(buffer.begin(), buffer.begin() + static_cast<std::ptrdiff_t>(consume));
                    emitted += static_cast<uint64_t>(consume);
                }
                return true;
            }
    };

    MovieAudioDecoder::MovieAudioDecoder(std::shared_ptr<MovieSource> const & source, std::string const & path) :
        m_impl(std::make_unique<Impl>())
    {
        if (!source) {
            throw InvalidFormat("movie audio needs a source: " + path);
        }
        m_impl->open(source, path);

        m_isstereo   = true;
        m_is8bit     = false;
        m_samplerate = static_cast<uint64_t>(m_impl->codec->sample_rate);
        m_duration   = static_cast<double>(m_impl->totalSamples) / static_cast<double>(m_impl->codec->sample_rate);
    }

    MovieAudioDecoder::~MovieAudioDecoder() = default;

    uint64_t MovieAudioDecoder::getDecodedLength() const
    {
        // A rough estimate is enough: it only decides streaming vs buffered, and
        // an under-estimate simply falls back to streaming.
        return std::max<uint64_t>(pcmBytesForSamples(m_impl->totalSamples), 1U);
    }

    bool MovieAudioDecoder::setCursor(uint64_t pos)
    {
        if (pos == m_position) {
            return true;
        }
        // Forward seeks are the common case while streaming, so only rewind when
        // the caller actually asks to go back.
        if (pos > m_position) {
            return true;
        }
        if (!m_impl->rewindTo(pos)) {
            return false;
        }
        m_position = pos;
        return true;
    }

    bool MovieAudioDecoder::decode(uint64_t length)
    {
        // fill() guarantees at least length bytes unless the track ended.
        bool const atEnd = m_impl->fill(length);

        // Hand back exactly what was asked for; decoding overshoots to whole
        // frames, so the surplus stays queued for the next call.
        size_t const take = std::min<size_t>(static_cast<size_t>(length), m_impl->buffer.size());
        m_impl->out.assign(m_impl->buffer.begin(), m_impl->buffer.begin() + static_cast<std::ptrdiff_t>(take));
        m_impl->buffer.erase(m_impl->buffer.begin(), m_impl->buffer.begin() + static_cast<std::ptrdiff_t>(take));

        m_position += take;
        // SoundDecoder contract: true means nothing was produced. Returning the
        // stream-end flag here would make callers drop the final chunk.
        (void)atEnd;
        return take == 0;
    }

    void* MovieAudioDecoder::getBuffer() const
    {
        return m_impl->out.data();
    }

    uint64_t MovieAudioDecoder::getBufferSize()
    {
        return static_cast<uint64_t>(m_impl->out.size());
    }

    void MovieAudioDecoder::releaseBuffer()
    {
        m_impl->out.clear();
    }

} // namespace FIFE
