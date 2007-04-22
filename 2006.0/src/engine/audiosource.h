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
#ifndef AUDIOSOURCE_H_
#define AUDIOSOURCE_H_

#include <AL/al.h>
#include <string>
#include <vector>
#include "audiomanager.h"

namespace FIFE {

	class AudioBuffer;
	
	class AudioSource {
		public:
			AudioSource() : m_source(0), m_bufferp() { alGenSources(1, &m_source); }
			~AudioSource();
			
			bool loadAudioFile(const std::string &);
			void freeAudioFile();
			
			void enableAudio();
			void playLoop();
			void playOnce();
			void disableAudio();

			void setPosition(float x, float y);
			void setOrientation(float x, float y);
			void setVelocity(float x, float y);

		private:
            ALuint m_source;
            type_audiobufptr m_bufferp;
	};


	// Allow the compiler to inline trivial functions
	inline void AudioSource::enableAudio() { alSourcePlay(m_source); }
	inline void AudioSource::playLoop() { alSourcei(m_source, AL_LOOPING, AL_TRUE); }
	inline void AudioSource::playOnce() { alSourcei(m_source, AL_LOOPING, AL_FALSE); }
	inline void AudioSource::disableAudio() { alSourceStop(m_source); }
	inline void AudioSource::setPosition(float x, float y) { alSource3f(m_source, AL_POSITION, x, y, 0.0); }
	inline void AudioSource::setOrientation(float x, float y) {
		ALfloat vec[6] = { x, y, 0.0, 0.0, 0.0, 1.0 };
		alSourcefv(m_source, AL_ORIENTATION, vec);
	}
	inline void AudioSource::setVelocity(float x, float y) { alSource3f(m_source, AL_VELOCITY, x, y, 0.0); }
	
}//FIFE

#endif /*AUDIOSOURCE_H_*/
