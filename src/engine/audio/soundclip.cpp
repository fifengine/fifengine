// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes

// Platform specific includes
#include <algorithm>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "loaders/native/audio/ogg_loader.h"
#include "util/base/exception.h"
#include "util/log/logger.h"

#include "soundclip.h"

namespace FIFE
{
    static Logger _log(LM_AUDIO);

    SoundClip::SoundClip(IResourceLoader* loader) :
        IResource(createUniqueClipName(), loader), m_isStream(false), m_decoder(nullptr), m_deleteDecoder(false)
    {
    }

    SoundClip::SoundClip(const std::string& name, IResourceLoader* loader) :
        IResource(name, loader), m_isStream(false), m_decoder(nullptr), m_deleteDecoder(false)
    {
    }

    SoundClip::~SoundClip()
    {
        free();

        // delete decoder
        if (m_deleteDecoder && m_decoder != nullptr) {
            delete m_decoder;
        }
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
                FL_WARN(_log, LMsg() << "No audio-decoder available for file \"" << m_name << "\"!");
                throw InvalidFormat("Error: Ogg loader can't load files without ogg extension");
            }
        }

        assert(m_decoder); // should be set by now

        m_isStream = m_decoder->needsStreaming();

        if (!m_isStream) {
            // only for non-streaming buffers
            auto* ptr = new SoundBufferEntry();

            // iterate the bufs and fill them with data
            for (auto* it = std::begin(ptr->buffers); it != std::end(ptr->buffers); ++it) {
                unsigned int& buffer = *it;

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
                    m_decoder->getBufferSize(),
                    m_decoder->getSampleRate());

                CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error copying data to buffers")

                ptr->usedbufs++;
            }

            m_decoder->releaseBuffer();

            // push the buffer information to the vector
            m_buffervec.push_back(ptr);
        }

        m_state = IResource::RES_LOADED;
    }

    void SoundClip::free()
    {
        if (m_state == IResource::RES_LOADED) {
            if (m_isStream) {
                // erase all elements from the list
                std::vector<SoundBufferEntry*>::iterator it;
                for (it = m_buffervec.begin(); it != m_buffervec.end(); ++it) {
                    if (((*it) != nullptr) && (*it)->buffers[0] != 0) {
                        alDeleteBuffers(BUFFER_NUM, &(*it)->buffers[0]);
                    }
                    delete (*it);
                }
            } else {
                // for non-streaming soundclips
                SoundBufferEntry* ptr = m_buffervec.at(0);
                for (uint32_t i = 0; i < ptr->usedbufs; i++) {
                    alDeleteBuffers(1, &ptr->buffers[i]);
                }
                delete ptr;
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
        return &m_buffervec.at(streamid)->buffers[0];
    }

    uint32_t SoundClip::beginStreaming()
    {
        SoundBufferEntry* ptr = nullptr;
        uint32_t id           = 0;
        for (uint32_t i = 0; i < m_buffervec.size(); i++) {
            if (m_buffervec.at(i) == nullptr) {
                ptr               = new SoundBufferEntry();
                m_buffervec.at(i) = ptr;
                id                = i;
                break;
            }
        }
        // create new sound buffer entry
        if (ptr == nullptr) {
            ptr = new SoundBufferEntry();
            m_buffervec.push_back(ptr);
            id = m_buffervec.size() - 1;
        }

        ptr->usedbufs  = 0;
        ptr->deccursor = 0;
        alGenBuffers(BUFFER_NUM, &ptr->buffers[0]);

        CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error creating streaming-buffers")

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
        case SD_SAMPLE_POS:
            pos = static_cast<uint64_t>(
                (m_decoder->getBitResolution() / 8.0) * (m_decoder->isStereo() ? 2.0 : 1.0) * value);
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
        const uint64_t pos = m_buffervec.at(streamid)->deccursor;
        switch (type) {
        case SD_BYTE_POS:
            return static_cast<float>(pos);
        case SD_SAMPLE_POS:
            return static_cast<float>(pos) /
                   (m_decoder->getBitResolution() / 8.0 * (m_decoder->isStereo() ? 2.0 : 1.0));
        case SD_TIME_POS:
            return static_cast<float>(pos) / (m_decoder->getBitResolution() / 8.0 *
                                              (m_decoder->isStereo() ? 2.0 : 1.0) * m_decoder->getSampleRate());
        }
        return 0.0F;
    }

    void SoundClip::acquireStream(uint32_t streamid)
    {
        const SoundBufferEntry* ptr = m_buffervec.at(streamid);

        const bool reachedEOF = std::ranges::any_of(ptr->buffers, [&](unsigned int buffer) {
            return getStream(streamid, buffer);
        });
        (void)reachedEOF;
    }

    bool SoundClip::getStream(uint32_t streamid, ALuint buffer)
    {
        SoundBufferEntry* ptr = m_buffervec.at(streamid);

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
            m_decoder->getBufferSize(),
            m_decoder->getSampleRate());

        // update cursor
        ptr->deccursor += m_decoder->getBufferSize();

        m_decoder->releaseBuffer();

        CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error catching stream")

        return false;
    }

    void SoundClip::quitStreaming(uint32_t streamid)
    {
        // release the buffers
        SoundBufferEntry* ptr = m_buffervec.at(streamid);
        alDeleteBuffers(BUFFER_NUM, &ptr->buffers[0]);
        ptr->buffers[0] = 0;
    }

    void SoundClip::endStreaming(uint32_t streamid)
    {
        SoundBufferEntry** ptr = &m_buffervec.at(streamid);
        delete *ptr;
        *ptr = nullptr;
    }

    void SoundClip::adobtDecoder(SoundDecoder* decoder)
    {
        m_decoder       = decoder;
        m_deleteDecoder = true;
    }

    void SoundClip::setDecoder(SoundDecoder* decoder)
    {
        m_decoder       = decoder;
        m_deleteDecoder = false;
    }

    SoundDecoder* SoundClip::getDecoder() const
    {
        return m_decoder;
    }

    size_t SoundClip::getSize()
    {
        return 0;
    }

    std::string SoundClip::createUniqueClipName()
    {
        // automated counting for name generation, in case the user doesn't provide a name
        static uint32_t uniqueNumber      = 0;
        static const std::string baseName = "soundclip";

        std::ostringstream oss;
        oss << uniqueNumber << "_" << baseName;

        const std::string name = oss.str();
        ++uniqueNumber;

        return name;
    }
} // namespace FIFE
