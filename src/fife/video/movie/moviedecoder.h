// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_MOVIE_MOVIEDECODER_H
#define FIFE_VIDEO_MOVIE_MOVIEDECODER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes

namespace FIFE
{
    class MovieDecoder_Impl;
    class MovieSource;

    /** Demuxes a movie container and decodes its video stream to RGBA.
     *
     * Data is read through the VFS, never from the system filesystem. The whole
     * container is held in memory, which suits short cutscene clips.
     *
     * Decoding is synchronous: nextFrame() does the work. See the todo document
     * for the threading question.
     */
    class FIFE_API MovieDecoder
    {
        public:
            /** Opens a movie.
             *
             * @param path VFS path of the movie file
             * @throws NotFound if the file cannot be read
             * @throws InvalidFormat if it is not a movie this decoder understands
             */
            explicit MovieDecoder(std::string const & path);

            /** Opens a movie over an already loaded source.
             *
             * Lets the video and audio decoders share a single read of the file.
             *
             * @param path VFS path, used for diagnostics
             * @param source shared container bytes, loaded from path if null
             */
            MovieDecoder(std::string const & path, std::shared_ptr<MovieSource> const & source);

            /** The container bytes, for sharing with an audio decoder. */
            std::shared_ptr<MovieSource> getSource() const;

            MovieDecoder(MovieDecoder const &)            = delete;
            MovieDecoder& operator=(MovieDecoder const &) = delete;

            ~MovieDecoder();

            /** Width of the video stream in pixels. Zero if not open. */
            uint32_t getWidth() const;

            /** Height of the video stream in pixels. Zero if not open. */
            uint32_t getHeight() const;

            /** Nominal frame rate in frames per second, derived from the time base. */
            double getFrameRate() const;

            /** Length of the video stream in seconds. */
            double getDuration() const;

            /** Decodes the next frame.
             *
             * The decoded pixels are then available via getFrameData().
             *
             * @return false at the end of the stream, true otherwise
             */
            bool nextFrame();

            /** Decodes the frame at or after the given time.
             *
             * Seeks to the preceding keyframe, so decoding then walks forward from
             * there: the first frame after a successful seek is not necessarily at
             * or after the requested time. Callers must skip until the timestamp
             * reaches the target. MoviePlayer already does this.
             *
             * @param seconds position within the video stream
             * @return false if the position could not be reached
             */
            bool seek(double seconds);

            /** Presentation time of the most recently decoded frame, in seconds. */
            double getFrameTimestamp() const;

            /** RGBA pixels of the current frame, getWidth() * getHeight() * 4 bytes.
             *
             * The layout matches the GL_RGBA / GL_UNSIGNED_BYTE uploads FIFE
             * already uses, so no repacking is needed before uploading.
             */
            uint8_t const * getFrameData() const;

        private:
            std::unique_ptr<MovieDecoder_Impl> m_impl;
    };
} // namespace FIFE

#endif
