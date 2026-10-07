// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "movieplayer.h"

// Standard C++ library includes
#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Platform specific includes

// 3rd party library includes

// FIFE includes
#include "audio/soundclipmanager.h"
#include "modules.h"
#include "movieaudiodecoder.h"
#include "moviedecoder.h"
#include "moviesource.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "video/imagemanager.h"

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
    } // namespace

    /** Installs a MovieAudioDecoder into a SoundClip.
     *
     * Owned by the MoviePlayer, because SoundClip keeps the loader pointer and
     * the clip outlives the load() call.
     */
    class MovieAudioLoader : public IResourceLoader
    {
        public:
            MovieAudioLoader(std::unique_ptr<MovieAudioDecoder> decoder, std::string path) :
                m_decoder(std::move(decoder)), m_path(std::move(path))
            {
            }

            void load(IResource* resource) override
            {
                auto* clip = dynamic_cast<SoundClip*>(resource);
                if (clip == nullptr || !m_decoder) {
                    return;
                }
                FL_LOG(_log(), std::format("installing movie audio decoder for {}", m_path));
                // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
                clip->adobtDecoder(m_decoder.release());
            }

        private:
            std::unique_ptr<MovieAudioDecoder> m_decoder;
            std::string m_path;
    };

    MoviePlayer::MoviePlayer() = default;

    MoviePlayer::~MoviePlayer()
    {
        unload();
    }

    void MoviePlayer::load(std::string const & path)
    {
        unload();

        // One read of the file, shared by both decoders.
        auto source = std::make_shared<MovieSource>(path);
        m_decoder   = std::make_unique<MovieDecoder>(path, source);
        m_duration  = m_decoder->getDuration();

        // Audio is optional: a movie without a usable track still plays.
        m_hasaudio = false;
        try {
            m_audiodecoder = std::make_unique<MovieAudioDecoder>(source, path);
            m_hasaudio     = true;
        } catch (InvalidFormat const & e) {
            FL_LOG(_log(), std::format("{} has no usable audio track: {}", path, e.what()));
        }

        m_frame  = ImageManager::instance()->loadBlank(m_decoder->getWidth(), m_decoder->getHeight());
        m_path   = path;
        m_loaded = true;

        FL_LOG(
            _log(),
            std::format(
                "loading movie {} ({}x{}, {:.2f}s)", path, m_decoder->getWidth(), m_decoder->getHeight(), m_duration));

        rewind();
    }

    void MoviePlayer::unload()
    {
        m_soundclip.reset();
        m_audioloader.reset();
        m_audiodecoder.reset();
        m_hasaudio = false;
        m_path.clear();
        m_decoder.reset();
        m_frame.reset();
        m_pending.clear();
        m_pending.shrink_to_fit();
        m_duration    = 0.0;
        m_startTime   = 0.0;
        m_started     = false;
        m_frameTime   = 0.0;
        m_pendingTime = 0.0;
        m_hasPending  = false;
        m_hasFrame    = false;
        m_loaded      = false;
        m_finished    = false;
    }

    bool MoviePlayer::isLoaded() const
    {
        return m_loaded;
    }

    void MoviePlayer::rewind()
    {
        if (!m_loaded) {
            return;
        }
        m_decoder->seek(0.0);
        m_startTime   = 0.0;
        m_started     = false;
        m_frameTime   = 0.0;
        m_pendingTime = 0.0;
        m_hasPending  = false;
        m_hasFrame    = false;
        m_finished    = false;
        m_pending.clear();
    }

    void MoviePlayer::stop()
    {
        rewind();
    }

    void MoviePlayer::play(double currentTime)
    {
        (void)currentTime;

        if (!m_loaded) {
            return;
        }
        rewind();
    }

    void MoviePlayer::setLooping(bool looping)
    {
        m_looping = looping;
    }

    bool MoviePlayer::isLooping() const
    {
        return m_looping;
    }

    bool MoviePlayer::isFinished() const
    {
        return m_finished;
    }

    ImagePtr MoviePlayer::getFrameImage() const
    {
        return m_frame;
    }

    uint32_t MoviePlayer::getWidth() const
    {
        return m_loaded ? m_decoder->getWidth() : 0U;
    }

    uint32_t MoviePlayer::getHeight() const
    {
        return m_loaded ? m_decoder->getHeight() : 0U;
    }

    double MoviePlayer::getDuration() const
    {
        return m_duration;
    }

    double MoviePlayer::getFrameTimestamp() const
    {
        return m_frameTime;
    }

    bool MoviePlayer::hasAudio() const
    {
        return m_hasaudio;
    }

    SoundClipPtr MoviePlayer::getSoundClip()
    {
        if (!m_soundclip && m_audiodecoder) {
            m_audioloader = std::make_unique<MovieAudioLoader>(std::move(m_audiodecoder), m_path);
            m_soundclip   = SoundClipManager::instance()->create("movie_audio_" + m_path, m_audioloader.get());
            if (m_soundclip->getState() != IResource::RES_LOADED) {
                m_soundclip->load();
            }
            FL_LOG(_log(), std::format("movie audio ready: {}", m_path));
        }
        return m_soundclip;
    }

    void MoviePlayer::upload(uint8_t const * rgba)
    {
        if (rgba == nullptr || !m_frame) {
            return;
        }
        m_frame->updateTexture(rgba, m_decoder->getWidth(), m_decoder->getHeight());
        m_hasFrame = true;
    }

    void MoviePlayer::update(double currentTime)
    {
        if (!m_loaded || m_decoder == nullptr) {
            return;
        }

        // Rebase on the first update instead of trusting the timestamp handed to
        // play(): callers often call it before their clock has been read, which
        // would make the movie look like it started in the far past.
        if (!m_started) {
            m_startTime = currentTime;
            m_started   = true;
        }

        double target = currentTime - m_startTime;
        if (target < 0.0) {
            target = 0.0;
        }

        if (m_duration > 0.0 && target >= m_duration) {
            if (!m_looping) {
                m_finished = true;
                return;
            }
            target      = 0.0;
            m_startTime = currentTime;
            m_decoder->seek(0.0);
            m_frameTime  = 0.0;
            m_hasPending = false;
            m_pending.clear();
        }

        // Playing backwards, or seeking before the current frame: rewind.
        if (target < m_frameTime) {
            m_decoder->seek(target);
            m_frameTime  = 0.0;
            m_hasPending = false;
            m_pending.clear();
        }

        while (true) {
            if (m_hasPending && target >= m_pendingTime) {
                upload(m_pending.data());
                m_frameTime  = m_pendingTime;
                m_hasPending = false;
                m_pending.clear();
                continue;
            }
            if (m_hasPending) {
                // A frame is already held for a later time; decoding another one
                // now would overwrite it and let the decoder run ahead.
                return;
            }
            if (!m_decoder->nextFrame()) {
                // Ran out of frames before reaching the target.
                if (m_looping && m_duration > 0.0) {
                    target      = 0.0;
                    m_startTime = currentTime;
                    m_decoder->seek(0.0);
                    m_frameTime  = 0.0;
                    m_hasPending = false;
                    m_pending.clear();
                    continue;
                }
                m_finished = true;
                return;
            }

            double const pts = m_decoder->getFrameTimestamp();
            if (pts > target && m_hasFrame) {
                // Still in the future, so hold it for the next update.
                size_t const size =
                    static_cast<size_t>(m_decoder->getWidth()) * static_cast<size_t>(m_decoder->getHeight()) * 4U;
                m_pending.assign(m_decoder->getFrameData(), m_decoder->getFrameData() + size);
                m_pendingTime = pts;
                m_hasPending  = true;
                return;
            }

            upload(m_decoder->getFrameData());
            m_frameTime = pts;
        }
    }
} // namespace FIFE
