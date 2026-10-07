// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_MOVIE_MOVIEAUDIODECODER_H
#define FIFE_VIDEO_MOVIE_MOVIEAUDIODECODER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "audio/sounddecoder.h"

namespace FIFE
{
    class MovieSource;

    /** Decodes the audio track of a movie container to interleaved stereo PCM.
     *
     * Output is always stereo signed 16-bit, which is what SoundDecoder's
     * getALFormat() maps onto. Implements the pull interface the OpenAL
     * streaming path in SoundClip expects, so setCursor() must be cheap for
     * forward seeks: the decoder only moves forward.
     */
    class FIFE_API MovieAudioDecoder : public SoundDecoder
    {
        public:
            /** Opens the audio track of an already loaded movie.
             *
             * @param source container bytes, shared with the video decoder
             * @param path VFS path, used for diagnostics
             * @throws InvalidFormat if there is no usable audio track
             */
            MovieAudioDecoder(std::shared_ptr<MovieSource> const & source, std::string const & path);

            MovieAudioDecoder(MovieAudioDecoder const &)            = delete;
            MovieAudioDecoder& operator=(MovieAudioDecoder const &) = delete;

            ~MovieAudioDecoder() override;

            uint64_t getDecodedLength() const override;

            bool setCursor(uint64_t pos) override;

            bool decode(uint64_t length) override;

            void* getBuffer() const override;

            uint64_t getBufferSize() override;

            void releaseBuffer() override;

            /** Position of the next byte decode() will produce. */
            uint64_t getPosition() const
            {
                return m_position;
            }

            /** Length of the audio track in seconds. */
            double getDuration() const
            {
                return m_duration;
            }

        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl;
            uint64_t m_position{0};
            double m_duration{0.0};
    };
} // namespace FIFE

#endif
