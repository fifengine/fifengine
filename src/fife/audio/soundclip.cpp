// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "soundclip.h"

// Standard C++ library includes
#include <cassert>
#include <format>
#include <memory>
#include <utility>

// Platform specific includes
#include <algorithm>
#include <iterator>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "loaders/native/audio/ogg_loader.h"
#include "util/base/exception.h"
#include "util/log/logger.h"

namespace FIFE
{
    // Use `FIFE_API` from platform.h on declarations; don't define local FIFE_API_IMPL here.

    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_AUDIO);
            return log;
        }
    } // namespace

    namespace
    {
        [[nodiscard]] ALsizei toOpenALSize(uint64_t const value)
        {
            assert(value <= static_cast<uint64_t>(std::numeric_limits<ALsizei>::max()));
            return static_cast<ALsizei>(value);
        }

        [[nodiscard]] uint32_t toStreamId(std::size_t const value)
        {
            assert(value <= static_cast<std::size_t>(std::numeric_limits<uint32_t>::max()));
            return static_cast<uint32_t>(value);
        }

        [[nodiscard]] double bytesPerSampleFrame(SoundDecoder const * decoder)
        {
            return (static_cast<double>(decoder->getBitResolution()) / 8.0) * (decoder->isStereo() ? 2.0 : 1.0);
        }
    } // namespace

    SoundClip::SoundClip(IResourceLoader* loader) : IResource(createUniqueClipName(), loader), m_isStream(false)
    {
    }

    SoundClip::SoundClip(std::string const & name, IResourceLoader* loader) : IResource(name, loader), m_isStream(false)
    {
    }

    SoundClip::~SoundClip()
    {
        SoundClip::free();
    }

    void SoundClip::load()
    {
        if (m_loader != nullptr) {
            m_loader->load(this);
        } else { // no loader specified so find one to use
            if (m_name.find(".ogg", m_name.size() - 4) != std::string::npos) {
                OggLoader loader;
                loader.load(this);
            } else {
                FL_WARN(_log(), std::format("No audio-decoder available for file \"{}\"!", m_name));
                throw InvalidFormat("Error: Ogg loader can't load files without ogg extension");
            }
        }

        assert(m_decoder); // should be set by now

        m_isStream = m_decoder->needsStreaming();

        if (!m_isStream) {
            // only for non-streaming buffers
            auto ptr = std::make_unique<SoundBufferEntry>();

            // iterate the bufs and fill them with data
            for (unsigned int& buffer : ptr->buffers) {

                if (m_decoder->decode(BUFFER_LEN)) {
                    // EOF or error
                    break;
                }

                // generate buffer and fill it with data
                alGenBuffers(1, &buffer);

                alBufferData(
                    buffer,
                    m_decoder->getALFormat(),
                    m_decoder->getBuffer(),
                    toOpenALSize(m_decoder->getBufferSize()),
                    toOpenALSize(m_decoder->getSampleRate()));

                CHECK_OPENAL_LOG(_log(), LogManager::LEVEL_ERROR, "error copying data to buffers")

                ptr->usedbufs++;
            }

            m_decoder->releaseBuffer();

            // push the buffer information to the vector
            m_buffervec.push_back(std::move(ptr));
        }

        m_state = IResource::RES_LOADED;
    }

    void SoundClip::free()
    {
        if (m_state == IResource::RES_LOADED) {
            if (m_isStream) {
                // erase all elements from the list
                for (auto& buf : m_buffervec) {
                    if (buf && buf->buffers.at(0) != 0) {
                        alDeleteBuffers(BUFFER_NUM, buf->buffers.data());
                    }
                }
            } else {
                // for non-streaming soundclips
                SoundBufferEntry const * ptr = m_buffervec.at(0).get();
                for (uint32_t i = 0; i < ptr->usedbufs; i++) {
                    alDeleteBuffers(1, &ptr->buffers.at(i));
                }
            }
            m_buffervec.clear();
        }
        m_state = IResource::RES_NOT_LOADED;
    }

    bool SoundClip::isStream() const
    {
        return m_isStream;
    }

    uint32_t SoundClip::countBuffers() const
    {
        return m_buffervec.at(0)->usedbufs;
    }

    ALuint* SoundClip::getBuffers(uint32_t streamid) const
    {
        return m_buffervec.at(streamid)->buffers.data();
    }

    uint32_t SoundClip::beginStreaming()
    {
        SoundBufferEntry* ptr = nullptr;
        uint32_t id           = 0;
        for (uint32_t i = 0; i < m_buffervec.size(); i++) {
            if (m_buffervec.at(i) == nullptr) {
                auto newEntry     = std::make_unique<SoundBufferEntry>();
                ptr               = newEntry.get();
                m_buffervec.at(i) = std::move(newEntry);
                id                = i;
                break;
            }
        }
        // create new sound buffer entry
        if (ptr == nullptr) {
            auto newEntry = std::make_unique<SoundBufferEntry>();
            ptr           = newEntry.get();
            m_buffervec.push_back(std::move(newEntry));
            id = toStreamId(m_buffervec.size() - 1);
        }

        ptr->usedbufs  = 0;
        ptr->deccursor = 0;
        alGenBuffers(BUFFER_NUM, ptr->buffers.data());

        CHECK_OPENAL_LOG(_log(), LogManager::LEVEL_ERROR, "error creating streaming-buffers")

        return id;
    }

    bool SoundClip::setStreamPos(uint32_t streamid, SoundPositionType type, float value)
    {
        uint64_t pos = 0;
        // convert position to bytes
        switch (type) {
        case SD_BYTE_POS:
            pos = static_cast<uint64_t>(value);
            break;
        case SD_TIME_POS:
            value /= static_cast<float>(m_decoder->getSampleRate());
            [[fallthrough]];
        case SD_SAMPLE_POS:
            pos = static_cast<uint64_t>(bytesPerSampleFrame(m_decoder.get()) * static_cast<double>(value));
            break;
        }

        if (pos > m_decoder->getDecodedLength()) {
            // EOF!
            m_buffervec.at(streamid)->deccursor = m_decoder->getDecodedLength();
            return true;
        }

        // set cursor position
        m_buffervec.at(streamid)->deccursor = pos;
        return false;
    }

    float SoundClip::getStreamPos(uint32_t streamid, SoundPositionType type) const
    {
        uint64_t const pos = m_buffervec.at(streamid)->deccursor;
        switch (type) {
        case SD_BYTE_POS:
            return static_cast<float>(pos);
        case SD_SAMPLE_POS:
            return static_cast<float>(static_cast<double>(pos) / bytesPerSampleFrame(m_decoder.get()));
        case SD_TIME_POS:
            return static_cast<float>(
                static_cast<double>(pos) /
                (bytesPerSampleFrame(m_decoder.get()) * static_cast<double>(m_decoder->getSampleRate())));
        }
        return 0.0F;
    }

    void SoundClip::acquireStream(uint32_t streamid)
    {
        SoundBufferEntry const * ptr = m_buffervec.at(streamid).get();

        bool const reachedEOF =
            std::ranges::any_of(ptr->buffers, [&](unsigned int buffer) { // NOLINT(cppcoreguidelines-init-variables)
                return getStream(streamid, buffer);
            });
        (void)reachedEOF;
    }

    bool SoundClip::getStream(uint32_t streamid, ALuint buffer)
    {
        SoundBufferEntry* ptr = m_buffervec.at(streamid).get();

        if (ptr->deccursor >= m_decoder->getDecodedLength()) {
            // EOF!
            return true;
        }

        // set cursor of decoder
        if (!m_decoder->setCursor(ptr->deccursor)) {
            return true;
        }

        // Error while decoding file?
        if (m_decoder->decode(BUFFER_LEN)) {
            throw Exception("error while reading from audio file");
        }

        // fill the buffer with data
        alBufferData(
            buffer,
            m_decoder->getALFormat(),
            m_decoder->getBuffer(),
            toOpenALSize(m_decoder->getBufferSize()),
            toOpenALSize(m_decoder->getSampleRate()));

        // update cursor
        ptr->deccursor += m_decoder->getBufferSize();

        m_decoder->releaseBuffer();

        CHECK_OPENAL_LOG(_log(), LogManager::LEVEL_ERROR, "error catching stream")

        return false;
    }

    void SoundClip::quitStreaming(uint32_t streamid)
    {
        // release the buffers
        SoundBufferEntry* ptr = m_buffervec.at(streamid).get();
        alDeleteBuffers(BUFFER_NUM, ptr->buffers.data());
        ptr->buffers.at(0) = 0;
    }

    void SoundClip::endStreaming(uint32_t streamid)
    {
        m_buffervec.at(streamid).reset();
    }

    void SoundClip::adobtDecoder(SoundDecoder* decoder) // cppcheck-suppress constParameterPointer
    {
        m_decoder.reset(decoder);
    }

    void SoundClip::setDecoder(SoundDecoder* decoder) // cppcheck-suppress constParameterPointer
    {
        m_decoder.reset(decoder);
    }

    SoundDecoder* SoundClip::getDecoder() const
    {
        return m_decoder.get();
    }

    size_t SoundClip::getSize()
    {
        return 0;
    }

    std::string SoundClip::createUniqueClipName()
    {
        // automated counting for name generation, in case the user doesn't provide a name
        static uint32_t uniqueNumber      = 0;
        static std::string const baseName = "soundclip";

        std::ostringstream oss;
        oss << uniqueNumber << "_" << baseName;

        std::string const name = oss.str();
        ++uniqueNumber;

        return name;
    }
} // namespace FIFE
