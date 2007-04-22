/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#include <vorbisfile.h>

#include "audiomanager.h"
#include "audioconfig.h"
#include "audiodecoder.h"
#include "exception.h"

#include <stdio.h>
#include <SDL_timer.h>

#include <iostream>

namespace FIFE {

	typedef ::boost::shared_ptr<AudioDecoder> audiodec_sp;

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

	int thread_ambientsound(void *data) {
		Audio_Threaddata *asd = reinterpret_cast<Audio_Threaddata *> (data);
		
		for (int i=0; i<asd->m_ad->countBuffers(); ++i) {
			ALuint buf = asd->m_ad->acquireBuffer();
			alSourceQueueBuffers(*(asd->m_src), 1, &buf);
		}
		
		if (AL_NO_ERROR != alGetError()) {
		        std::cerr << "Couldn't queue buffers" << std::endl;
		        return -1;
		} 
		
		alSourcePlay(*(asd->m_src));
        SDL_Delay(5000);		

		ALint procs, old_prc=0;

		do {
	        alGetSourcei(*(asd->m_src), AL_BUFFERS_PROCESSED, &procs);
			if (procs>old_prc) {
				ALuint buf = asd->m_ad->releaseBuffer();
				ALuint buf2;
				alSourceUnqueueBuffers(*(asd->m_src), 1, &buf2);
				asd->m_ad->fillBuffer();
				buf = asd->m_ad->acquireBuffer();
				alSourceQueueBuffers(*(asd->m_src), 1, &buf);
			} else {
				old_prc=procs;
			}
			
			SDL_Delay(5000);
		} while (!asd->m_stopthread);
		
		alSourceStop(*(asd->m_src));
		return 0;
	}

	void AudioManager::setAmbientSound(const std::string &name) {
		if (m_thread.m_thread) {
			m_thread.m_stopthread = true;
			SDL_WaitThread(m_thread.m_thread, NULL);
			m_thread.m_thread = NULL;
		}

		if (name == "" )
			return;	

		m_ad = getBufferFromFile(name);
		
		if (m_ad->isStreaming()) {
			m_thread.m_stopthread = false;
			m_thread.m_ad = m_ad;
			alSourcei(m_source, AL_LOOPING, AL_FALSE);
			m_thread.m_thread = SDL_CreateThread(thread_ambientsound, &m_thread);
		} else {
			if (m_ad->countBuffers() == 1) {
				alSourcei(m_source, AL_BUFFER, m_ad->acquireBuffer());
			} else {
				for (int i=0; i<m_ad->countBuffers(); ++i) {
					ALuint x = m_ad->acquireBuffer();
					alSourceQueueBuffers (m_source, 1, &x);
				}
			}
			alSourcei(m_source, AL_LOOPING, AL_TRUE);
			alSourcePlay(m_source);
		}
	}

	type_audiobufptr AudioManager::getBufferFromFile(const std::string &name) {
		// Check if we have a buffer with this file already
		std::vector<boost::weak_ptr<AudioBuffer> >::const_iterator end = m_audiobufvec.end();
		for (std::vector<boost::weak_ptr<AudioBuffer> >::iterator i = m_audiobufvec.begin(); i != end; ++i) {
			type_audiobufptr locked = (*i).lock();
			if (locked) {
				if (locked->m_name == name) {
					if (locked->isStreaming()) // We cannot reuse a streaming AudioBuffer
						continue;
					return locked;
				}
			} else {
				m_audiobufvec.erase(i);
			}
		}
		
		// The file is either not reusable or not in memory. We have to create a 
		// new AudioBuffer.
		audiodec_sp ad(AudioDecoder::createDecoder(name));
		if (!ad) {
			std::cerr << "Unknown music format" << std::endl;
			return type_audiobufptr();
		}

		type_audiobufptr asp;
		
		if (ad->needsStreaming()) {
			asp.reset(new AudioBuffer_multibuf(name, ad));
		} else {
			asp.reset(new AudioBuffer_singlebuf(name, ad));
		}

		::boost::weak_ptr<AudioBuffer> wp(asp);
		m_audiobufvec.push_back(wp);
		
		return asp;
	}

	float AudioManager::m_savedvolume = 1.0; 

	AudioManager::AudioManager() : m_audiocontext(NULL), m_audiodevice(alcOpenDevice(NULL)) {
		if (!m_audiodevice) {
			// Error
			throw Exception("Couldn't open audio device");
		}

		m_audiocontext = alcCreateContext(m_audiodevice, NULL);

		if (alcGetError(m_audiodevice) != ALC_NO_ERROR || !m_audiocontext) {
			// Error
			throw Exception("Couldn't create audio context");
		}

		alcMakeContextCurrent(m_audiocontext);
	
		if (alcGetError(m_audiodevice) != ALC_NO_ERROR) {
			// Error
			throw Exception("Couldn't change current audio context");
		}
	
		// We move along the x-y-plane and look along the x axis.
		ALfloat orient[6] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
		alListenerfv(AL_ORIENTATION, orient);

		alGenSources(1, &m_source);
		alSourcei(m_source, AL_SOURCE_RELATIVE, AL_TRUE);
		
		m_thread.m_thread=0;
		m_thread.m_stopthread=false;
		m_thread.m_src=&m_source;
	}

	AudioManager::~AudioManager() {
		m_audiobufvec.clear();
    
		alcMakeContextCurrent(NULL);
		alcDestroyContext(m_audiocontext);
		alcCloseDevice(m_audiodevice);
	}

}//FIFE
