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

#ifndef FIFE_AUDIO_SOURCE_H
#define FIFE_AUDIO_SOURCE_H

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes
#include <AL/al.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "timeevent.h"

#include "buffer.h"

namespace FIFE { namespace audio {

	class Buffer;
	
	/** This class is for playing a positioned sound.
	 * 
	 * To play a sound you allocate an AudioSource and set the position of it with
	 * setPosition() and if it is the case its velocity with setVelocity(). Please
	 * bear in mind that even with a velocity its position won't change over time
	 * by itself. The velocity value is only used for the computation of the 
	 * Doppler effect. The actual data is loaded with loadAudioFile() (which 
	 * doesn't invalidate the already set position and velocity).
	 */
	class Source : private TimeEvent {
		public:
			/** Allocates an OpenAL source.
			 */
			Source();
			
			~Source(); // no need to be virtual?

			/** Loads a audio file.
			 *
			 * @param filename The filename of the to be loaded audio file.
			 * @return true on success, false on failure
			 */
			bool loadFile(const std::string &filename);

			/** Starts playing the associated audio file.
			 */
			void enable();
			
			/** Sets the playing mode to play-in-a-loop mode. 
			 * FIXME: doesn't work on streaming audio files
			 */
			void playLoop() {
				if (m_bufferp && !m_bufferp->isStreaming()) {
					alSourcei(m_source, AL_LOOPING, AL_TRUE);
				}
			}
			
			/** Sets the playing mode to play-once mode.
			 */
			void playOnce() {
				alSourcei(m_source, AL_LOOPING, AL_FALSE); 
			}
			
			/** Stops playing the audio file but don't releases the resources.
			 */
			void disable() {
				setPeriod(-1); alSourceStop(m_source);
			}

			/** Sets the position of the audiosource in the virtual audio space.
			 *
			 * @param x the x coordinate of the audiosource
			 * @param y the y coordiante of the audiosource
			 */
			void setPosition(float x, float y) {
				alSource3f(m_source, AL_POSITION, x, y, 0.0);
			}

			/** Sets the velocity of the AudioSource in the virtual audio space.
			 *
			 * @param x The velocity in the x direction.
			 * @param y The velocity in the y direction.
			 */
			void setVelocity(float x, float y) {
				alSource3f(m_source, AL_VELOCITY, x, y, 0.0);
			}

		private:
			/** Implementation of the pure virtual function from TimeEvent
			 */
			virtual void updateEvent(unsigned long time);
			
			ALuint m_source;
			type_bufptr m_bufferp;

			// The number of finished buffers
			ALint m_bufsdone;
	};
} } //FIFE::audio

#endif /*AUDIOSOURCE_H_*/
