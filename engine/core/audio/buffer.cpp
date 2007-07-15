/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "buffer.h"
#include "decoder.h"

namespace FIFE { namespace audio {

	namespace buffer_detail {

		void prepareBuffer(ALuint which, type_decptr ad) {
			alBufferData(which, ad->getALFormat(), ad->getBuffer(), 
									 ad->getBufferSize(), ad->getSampleRate());
		}

		class Single : public Buffer {
			public:
				Single(const std::string &name, type_decptr ad);
				virtual ~Single() { alDeleteBuffers(1, &m_buffer); }

				virtual uint8_t countBuffers() { return 1; }
				virtual ALuint  acquireBuffer() { return m_buffer; }
				virtual void    fillBuffer() {}
				virtual ALuint  releaseBuffer() { return 0; }

			private:
				ALuint m_buffer;
		};

		Single::Single(const std::string &name, type_decptr ad) : Buffer(name), m_buffer(0) {
			alGenBuffers(1, &m_buffer);
			ad->decodeAll();
			prepareBuffer(m_buffer, ad);
			m_eof = true;
		}

		class Multi : public Buffer {
			public:
				Multi(const std::string &name, type_decptr ad);
				virtual ~Multi() { alDeleteBuffers(BUFFER_NUM, &m_buffers[0]); }

				virtual uint8_t countBuffers() { return m_usedbufs; }
				virtual ALuint  acquireBuffer();
				virtual void    fillBuffer();
				virtual ALuint  releaseBuffer();

			private:
				type_decptr m_dec;
				ALuint m_buffers[BUFFER_NUM];
				uint8_t m_nextbuf, m_usedbufs, m_bufrdy;
		};

		Multi::Multi(const std::string &name, type_decptr ad) : Buffer(name), m_dec(), m_nextbuf(0), m_usedbufs(BUFFER_NUM) {
			alGenBuffers(BUFFER_NUM, &m_buffers[0]);

			m_stream = true;
			uint8_t i = 0;
			do {
				if (BUFFER_LEN != ad->decodePart(BUFFER_LEN)) {
					m_stream = false;
					m_eof = true;
					m_usedbufs = i+1;
					break;
				}
				prepareBuffer(m_buffers[i], ad);
			} while (++i < BUFFER_NUM);

			if (m_usedbufs == BUFFER_NUM && 
					ad->guessedLength() > BUFFER_LEN * BUFFER_NUM) {
				m_dec = ad;
			}

			m_bufrdy = m_usedbufs;
			m_nextbuf = m_usedbufs - 1;
		}

		ALuint Multi::acquireBuffer() {
			if (m_bufrdy || !m_stream) {
				--m_bufrdy;
				if (++m_nextbuf == m_usedbufs) {
					m_nextbuf = 0;
				}
				return m_buffers[m_nextbuf];
			} else {
				return 0;
			}
		}

		void Multi::fillBuffer() {
			if (m_stream && (m_bufrdy < m_usedbufs)) {
				if (BUFFER_LEN != m_dec->decodePart(BUFFER_LEN)) {
					m_eof = true;
				}
				
				prepareBuffer(releaseBuffer(), m_dec);
				++m_bufrdy;
			}
		}

		ALuint Multi::releaseBuffer() {
			uint8_t rldbuf = (m_nextbuf + 1 + m_bufrdy) % m_usedbufs;
			return m_buffers[rldbuf];
		}

	} // namespace buffer_detail
	
	Buffer* Buffer::create(const std::string &name, type_decptr ad) {
		if (ad->needsStreaming()) {
			return new buffer_detail::Multi(name, ad);
		} else {
			return new buffer_detail::Single(name, ad);
		}
	}

} } // namespace FIFE::audio
