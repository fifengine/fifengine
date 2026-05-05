// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_SOUNDCLIP_H_
#define FIFE_SOUNDCLIP_H_

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <memory>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes

#include "sounddecoder.h"
#include "util/resource/resource.h"

namespace FIFE
{

    /** Different types of audio-file positions
     */
    enum SoundPositionType : uint8_t
    {
        SD_SAMPLE_POS,
        SD_TIME_POS,
        SD_BYTE_POS
    };

    struct /*FIFE_API*/ SoundBufferEntry
    {
            ALuint buffers[BUFFER_NUM];
            uint32_t usedbufs;
            uint64_t deccursor;
    };

    /**  Class to handle the buffers of an audio file
     */
    class /*FIFE_API*/ SoundClip : public IResource
    {
        public:
            explicit SoundClip(IResourceLoader* loader = nullptr);
            explicit SoundClip(std::string const & name, IResourceLoader* loader = nullptr);

            ~SoundClip();

            SoundClip(SoundClip const &)            = delete;
            SoundClip& operator=(SoundClip const &) = delete;

            /** Does this SoundClip require a streaming mechanism?
             *
             * @return Returns true if streaming is required, false if not.
             */
            bool isStream() const;

            /** Returns the number of buffers used by the SoundClip
             * (only for non-streaming sound clips)
             *
             * @return Returns the number of buffers.
             */
            uint32_t countBuffers() const;

            /** Returns the array of buffers for queuing
             *
             */
            ALuint* getBuffers(uint32_t streamid = 0) const;

            /** Starts streaming the soundclip
             * @return Returns the streamid
             */
            uint32_t beginStreaming();

            /** Fills the streaming-buffers with initial data
             *
             * @param streamid The stream ID
             */
            void acquireStream(uint32_t streamid);

            /** Sets the stream position
             * @return True if position is invalid (EOF has been reached)
             */
            bool setStreamPos(uint32_t streamid, SoundPositionType type, float value);

            /** Gets the stream position
             */
            float getStreamPos(uint32_t streamid, SoundPositionType type) const;

            /** Refill a processed buffer with new data
             *
             *  @return True if file was EOF
             *  @param streamid The stream ID
             *  @param buffer The OpenAL buffer ID
             */
            bool getStream(uint32_t streamid, ALuint buffer);

            /** Quits Streaming
             */
            void quitStreaming(uint32_t streamid);

            /** Ends streaming, invalidate also the stream id.
             */
            void endStreaming(uint32_t streamid);

            /** Adopts a decoder to use so DONT delete it
             */
            void adobtDecoder(SoundDecoder* decoder);

            /** Sets the decoder to use so DONT delete it before this SoundClip is done with it
             */
            void setDecoder(SoundDecoder* decoder);

            /** Returns the attached decoder
             */
            SoundDecoder* getDecoder() const;

            virtual size_t getSize();

            virtual void load();
            virtual void free();

        private:
            // is stream?
            bool m_isStream;
            // attached decoder
            SoundDecoder* m_decoder;
            // when loadFromDecoder-method is used, decoder shouldn't be deleted
            bool m_deleteDecoder;
            std::vector<SoundBufferEntry*, std::allocator<SoundBufferEntry*>> m_buffervec;

            std::string createUniqueClipName();
    };

    using SoundClipPtr = SharedPtr<SoundClip>;
} // namespace FIFE

#endif
