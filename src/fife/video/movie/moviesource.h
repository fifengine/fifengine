// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_MOVIE_MOVIESOURCE_H
#define FIFE_VIDEO_MOVIE_MOVIESOURCE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes

struct AVFormatContext;

namespace FIFE
{
    /** The bytes of a movie container, read once through the VFS.
     *
     * Sharing one source between the video and audio decoders keeps the file in
     * memory a single time. Each decoder gets its own AVIOContext over these
     * bytes, so their demux cursors stay independent.
     */
    class FIFE_API MovieSource
    {
        public:
            /** Reads a movie through the VFS.
             *
             * @param path VFS path of the movie file
             * @throws NotFound if the file cannot be read
             */
            explicit MovieSource(std::string const & path);

            MovieSource(MovieSource const &)            = delete;
            MovieSource& operator=(MovieSource const &) = delete;
            ~MovieSource();

            /** Size of the container in bytes. */
            int64_t size() const;

            /** Opens a format context over these bytes.
             *
             * The returned context owns its AVIOContext, including the buffer
             * libav allocated for it. The context holds a read cursor that stays
             * valid until this source is destroyed.
             *
             * @param format receives the opened context, nullptr on failure
             * @param path passed to libavformat only for diagnostics
             */
            void openFormatContext(AVFormatContext*& format, std::string const & path) const;

        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl;
    };
} // namespace FIFE

#endif
