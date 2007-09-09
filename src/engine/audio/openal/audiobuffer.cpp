/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#include "audiodecoder.h"
#include "audiobuffer.h"

namespace FIFE {
	
	class AudioBuffer_singlebuf : public AudioBuffer {
		public:
			AudioBuffer_singlebuf(const std::string &name, audiodec_sp ad);
			virtual ~AudioBuffer_singlebuf() { alDeleteBuffers(1, &m_buffer); }

            virtual uint8_t countBuffers() { return 1; }
            virtual ALuint  acquireBuffer() { return m_buffer; }
            virtual void    fillBuffer() { }
            virtual ALuint  releaseBuffer() { return 0; }

		private:
			ALuint m_buffer;
	};

	AudioBuffer_singlebuf::AudioBuffer_singlebuf(const std::string &name, audiodec_sp ad) : AudioBuffer(name), m_buffer(0) {
		alGenBuffers(1, &m_buffer);
		ad->decodeAll();
		alBufferData(m_buffer, ad->getALFormat(), ad->getBuffer(), ad->getBufferSize(), ad->getSampleRate());
		m_eof = true;
	}

	class AudioBuffer_multibuf : public AudioBuffer {
		public:
			AudioBuffer_multibuf(const std::string &name, audiodec_sp ad);
			virtual ~AudioBuffer_multibuf();

            virtual uint8_t countBuffers() { return m_usedbufs; }
            virtual ALuint  acquireBuffer();
            virtual void    fillBuffer();
            virtual ALuint  releaseBuffer();

		private:
			audiodec_sp m_audiodec;
			ALuint m_buffers[AUDIO_BUFFER_NUM];
			uint8_t m_nextbuf, m_usedbufs, m_bufrdy;
	};

	AudioBuffer_multibuf::AudioBuffer_multibuf(const std::string &name, audiodec_sp ad) : AudioBuffer(name), m_audiodec(), m_nextbuf(0), m_usedbufs(AUDIO_BUFFER_NUM) {
		alGenBuffers(AUDIO_BUFFER_NUM, &m_buffers[0]);

		m_stream = true;
		uint8_t i=0;
		do {
			if (AUDIO_BUFFER_LEN != ad->decodePart(AUDIO_BUFFER_LEN)) {
				m_stream = false;
				m_eof = true;
				m_usedbufs = i+1;
				i= AUDIO_BUFFER_LEN; // to end the while loop
			}
			alBufferData(m_buffers[i], ad->getALFormat(), ad->getBuffer(), ad->getBufferSize(), ad->getSampleRate());
		} while(++i < AUDIO_BUFFER_NUM);

		if (m_usedbufs == AUDIO_BUFFER_NUM && ad->guessedLength()>AUDIO_BUFFER_LEN*AUDIO_BUFFER_NUM)
			m_audiodec = ad;

		m_bufrdy = m_usedbufs;
		m_nextbuf = m_usedbufs-1;
	}

	AudioBuffer_multibuf::~AudioBuffer_multibuf() {
		alDeleteBuffers(AUDIO_BUFFER_NUM, &m_buffers[0]);
	}

	ALuint AudioBuffer_multibuf::acquireBuffer() {
		if (m_bufrdy || !m_stream) {
			--m_bufrdy;
			if (++m_nextbuf == m_usedbufs)
				m_nextbuf = 0;
			return m_buffers[m_nextbuf];
		} else {
			return 0;
		}
	}

	void AudioBuffer_multibuf::fillBuffer() {
		if (!m_stream)
			return;

		if (m_bufrdy < m_usedbufs) {
			uint8_t rldbuf = (m_nextbuf+1+m_bufrdy)%m_usedbufs;
			if (AUDIO_BUFFER_LEN != m_audiodec->decodePart(AUDIO_BUFFER_LEN)) {
				m_eof = true;
			}
			alBufferData(m_buffers[rldbuf], m_audiodec->getALFormat(), m_audiodec->getBuffer(), m_audiodec->getBufferSize(), m_audiodec->getSampleRate());
			++m_bufrdy;
		}
	}

	ALuint AudioBuffer_multibuf::releaseBuffer() {
		uint8_t rldbuf= (m_nextbuf+1+m_bufrdy)%m_usedbufs;
		return m_buffers[rldbuf];
	}

	AudioBuffer *AudioBuffer::createAudioBuffer(const std::string &name, audiodec_sp ad) {
		if (ad->needsStreaming()) {
			return new AudioBuffer_multibuf(name, ad);
		} else {
			return new AudioBuffer_singlebuf(name, ad);
		}
	}

} // namespace FIFE
