// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_SOUNDDECODER_OGG_H
#define FIFE_SOUNDDECODER_OGG_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <memory>
// 3rd party library includes
#include <vorbis/vorbisfile.h>

// FIFE includes
#include "audio/sounddecoder.h"
#include "vfs/raw/rawdata.h"

namespace FIFE
{
    class FIFE_API SoundDecoderOgg : public SoundDecoder
    {
        public:
            explicit SoundDecoderOgg(RawData* rdp);

            SoundDecoderOgg(SoundDecoderOgg const &)            = delete;
            SoundDecoderOgg& operator=(SoundDecoderOgg const &) = delete;

            ~SoundDecoderOgg();

            /** Returns the decoded length of the file in bytes
             */
            uint64_t getDecodedLength() const
            {
                return m_declength;
            }

            /** Sets the current position in the file (in bytes)
             *
             * @return True, if the positioning was successful
             */
            bool setCursor(uint64_t pos);

            /** Request the decoding of the next part of the stream.
             *
             * @param length The length of the decoded part
             * @return 0 (False), if decoding was successful
             */
            bool decode(uint64_t length);

            /** Returns the next decoded buffer.
             *
             * The length of the buffer is returned by getBufferSize().
             */
            void* getBuffer() const
            {
                return m_data;
            }

            /** Returns the byte-size of the buffer returned by getBuffer().
             */
            uint64_t getBufferSize()
            {
                return m_datasize;
            }

            /** Releases the buffer returned by getBuffer()
             */
            void releaseBuffer();

        private:
            std::unique_ptr<RawData> m_file;
            uint64_t m_declength;
            uint64_t m_datasize;
            char* m_data;
            OggVorbis_File m_ovf;
    };
} // namespace FIFE

#endif
