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
#ifndef AUDIOSOURCE_H_
#define AUDIOSOURCE_H_

#include <AL/al.h>
#include <string>
#include <vector>

#include "audiobuffer.h"
#include "timeevent.h"

namespace FIFE {

	class AudioBuffer;
	
	/** This class is for playing a positioned sound.
	 * 
	 * To play a sound you allocate an AudioSource and set the position of it with
	 * setPosition() and if it is the case its velocity with setVelocity(). Please
	 * bear in mind that even with a velocity its position won't change over time
	 * by itself. The velocity value is only used for the computation of the Doppler 
	 * effect. The actual data is loaded with loadAudioFile() (which doesn't invalidate
	 * the already set position and velocity).
	 */
	class AudioSource : private TimeEvent {
		public:
			/** Allocates an OpenAL source.
			 */
			AudioSource();
			
			virtual ~AudioSource();

			/** Loads a audio file.
			 *
			 * @param filename The filename of the to be loaded audio file.
			 * @return true on success, false on failure
			 */
			bool loadAudioFile(const std::string &filename);

			void freeAudioFile();
			
			/** Starts playing the associated audio file.
			 */
			void enableAudio();
			
			/** Sets the playing mode to play-in-a-loop mode. 
			 * FIXME: works only on not-streaming audio files
			 */
			void playLoop();

			/** Sets the playing mode to play-once mode.
			 */
			void playOnce();
			
			/** Stops playing the audio file but don't releases the resources.
			 */
			void disableAudio();

			/** Sets the position of the audiosource in the virtual audio space.
			 *
			 * @param x the x coordinate of the audiosource
			 * @param y the y coordiante of the audiosource
			 */
			void setPosition(float x, float y);

			/** Sets the velocity of the AudioSource in the virtual audio space.
			 *
			 * @param x The velocity in the x direction.
			 * @param y The velocity in the y direction.
			 */
			void setVelocity(float x, float y);

		private:
			/** Implementation of the pure virtual function from TimeEvent
			 */
			virtual void updateEvent(unsigned long time);
			
            ALuint m_source;
            type_audiobufptr m_bufferp;
            
            // The number of finished buffers
            ALint m_bufsdone;
	};


	// Allow the compiler to inline trivial functions
	inline void AudioSource::playLoop() { if (m_bufferp && !m_bufferp->isStreaming()) alSourcei(m_source, AL_LOOPING, AL_TRUE); }
	inline void AudioSource::playOnce() { alSourcei(m_source, AL_LOOPING, AL_FALSE); }
	inline void AudioSource::disableAudio() { setPeriod(-1); alSourceStop(m_source); }
	inline void AudioSource::setPosition(float x, float y) { alSource3f(m_source, AL_POSITION, x, y, 0.0); }
	inline void AudioSource::setVelocity(float x, float y) { alSource3f(m_source, AL_VELOCITY, x, y, 0.0); }
	
}//FIFE

#endif /*AUDIOSOURCE_H_*/
