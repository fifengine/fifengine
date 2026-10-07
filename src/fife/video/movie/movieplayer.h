// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_MOVIE_MOVIEPLAYER_H
#define FIFE_VIDEO_MOVIE_MOVIEPLAYER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "audio/soundclip.h"
#include "video/image.h"

namespace FIFE
{
    class MovieAudioDecoder;
    class MovieAudioLoader;
    class MovieDecoder;

    /** Plays a movie into a persistent image, one frame at a time.
     *
     * The decoder only moves forward, so a frame whose timestamp is still in the
     * future is held back and used on the next update instead of being decoded
     * again.
     */
    class FIFE_API MoviePlayer
    {
        public:
            MoviePlayer();
            MoviePlayer(MoviePlayer const &)            = delete;
            MoviePlayer& operator=(MoviePlayer const &) = delete;
            ~MoviePlayer();

            /** Loads a movie and resets playback.
             *
             * @param path VFS path of the movie file
             * @throws NotFound if the file cannot be read
             * @throws InvalidFormat if it is not a movie this engine understands
             */
            void load(std::string const & path);

            /** Releases the decoder and the frame image. */
            void unload();

            /** True once load() has succeeded and unload() has not been called. */
            bool isLoaded() const;

            /** Restarts playback from the beginning. */
            void rewind();

            /** Stops playback and rewinds. */
            void stop();

            /** Advances playback to the given position.
             *
             * @param currentTime seconds since play() was called
             */
            void update(double currentTime);

            /** Restarts playback from the beginning.
             *
             * @param currentTime accepted for symmetry with update(), but ignored:
             * the start time is taken from the first update() instead, because
             * callers commonly read their clock after calling play().
             */
            void play(double currentTime);

            /** Loops back to the start when the movie ends. Off by default. */
            void setLooping(bool looping);
            bool isLooping() const;

            /** True once the movie has played to the end without looping. */
            bool isFinished() const;

            /** Image holding the current frame, to be rendered or blitted.
             *
             * Stays valid between updates. Null until the first frame is decoded.
             */
            ImagePtr getFrameImage() const;

            uint32_t getWidth() const;
            uint32_t getHeight() const;

            /** Length of the movie in seconds. Zero if not loaded. */
            double getDuration() const;

            /** Timestamp of the frame currently held, in seconds.
             *
             * Changes only when update() decoded a new frame, so a caller can use
             * it to avoid rebuilding per-frame objects.
             */
            double getFrameTimestamp() const;

            /** True if the container has a usable audio track. */
            bool hasAudio() const;

            /** Sound clip for the audio track, ready to hand to a SoundEmitter.
             *
             * Built on first use and kept alive by this player, because the clip
             * outlives the loader that installed its decoder. Null when the movie
             * has no audio.
             */
            SoundClipPtr getSoundClip();

        private:
            void upload(uint8_t const * rgba);

            std::unique_ptr<MovieDecoder> m_decoder;
            std::unique_ptr<MovieAudioDecoder> m_audiodecoder;
            std::unique_ptr<MovieAudioLoader> m_audioloader;
            SoundClipPtr m_soundclip;
            std::string m_path;
            bool m_hasaudio{false};
            ImagePtr m_frame;
            std::vector<uint8_t> m_pending;
            double m_duration{0.0};
            double m_startTime{0.0};
            double m_frameTime{0.0};
            double m_pendingTime{0.0};
            bool m_started{false};
            bool m_hasPending{false};
            bool m_hasFrame{false};
            bool m_loaded{false};
            bool m_looping{false};
            bool m_finished{false};
    };
} // namespace FIFE

#endif
