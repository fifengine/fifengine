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
#ifndef AUDIOMANAGER_HH_
#define AUDIOMANAGER_HH_

#include <string>
#include <vector>
#include <stdint.h>

#include <SDL.h>
#include <SDL_thread.h>

#include <AL/al.h>
#include <AL/alc.h>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "singleton.h"

namespace FIFE {

	class AudioBuffer {
		public:
			AudioBuffer(const std::string &name) : m_name(name), m_stream(false), m_eof(false) {}
			virtual ~AudioBuffer() { }
            
            bool isStreaming() { return m_stream; }
            bool isEOF() { return m_eof; }
            
            virtual uint8_t countBuffers()=0;
            virtual ALuint  acquireBuffer()=0;
            virtual void    fillBuffer()=0;
            virtual ALuint  releaseBuffer()=0;
            
            const std::string m_name;
			
		protected:
			bool m_stream, m_eof;
	};

	typedef ::boost::shared_ptr<AudioBuffer> type_audiobufptr;

	struct Audio_Threaddata {
		SDL_Thread *m_thread;
		bool m_stopthread;
		ALuint *m_src;
		type_audiobufptr m_ad;
	};

	class AudioManager : public Singleton<AudioManager> {
		public:
			type_audiobufptr getBufferFromFile(const std::string &);

			void setAmbientSound(const std::string &);
			
			static void setVolume(float vol);
			static void mute();
			static void unmute();

		private:
			SINGLEFRIEND(AudioManager);
			
			ALuint m_source;
			type_audiobufptr m_ad;
			Audio_Threaddata m_thread;
			
			ALCcontext *m_audiocontext;
			ALCdevice  *m_audiodevice;
            std::vector<boost::weak_ptr<AudioBuffer> > m_audiobufvec;
            
            static float m_savedvolume;
	};

	inline void AudioManager::setVolume(float vol) { alListenerf(AL_GAIN, vol); }
	inline void AudioManager::mute() { alGetListenerf(AL_GAIN, &m_savedvolume); setVolume(0); }
	inline void AudioManager::unmute() { setVolume(m_savedvolume); }

}//FIFE
#endif
