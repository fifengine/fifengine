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
#ifndef AUDIOMANAGER_HH_
#define AUDIOMANAGER_HH_

#include <string>
#include <vector>
#include <stdint.h>

#include <SDL.h>

#include <AL/al.h>
#include <AL/alc.h>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "singleton.h"
#include "audiobuffer.h"
#include "audiosource.h"

namespace FIFE {

	/** This class manages the base audio system.
	 */
	class AudioManager : public DynamicSingleton<AudioManager> {
		public:
			/** Initialises the audio system.
			 */
			AudioManager();
			
			virtual ~AudioManager();

			/** Returns a pointer to an AudioBuffer that can be used to acquire the OpenAL buffers.
			 * 
			 * @param filename The name of the audio file that shall be loaded.
			 */
			type_audiobufptr getBufferFromFile(const std::string &filename);

			/** Sets the background sound.
			 * 
			 * @param file The filename of the background audio file. Use "" to turn it off.
			 */
			void setAmbientSound(const std::string &file);

			/** Sets the volume by changed the gain of the listener.
			 * 
			 * @param vol The volume value. 0=silence ... 1.0=normal loudness.
			 */
			static void setVolume(float vol);
			
			/** Mutes the sound system.
			 */
			static void mute();
			
			/** Unmutes the sounds to volume before mute() was called.
			 */
			static void unmute();
			
			/** Changes the position of the listener (alter ego).
			 * 
			 * @param x The position on the abscissa of the listener.
			 * @param y The position on the ordinate of the listener.
			 */
			static void changeListenerPosition(float x, float y);
			
			/** Changes the orientation of the listener (alter ego).
			 * 
			 * @param x The position on the abscissa to which listener is directed.
			 * @param y The position on the ordinate to which listener is directed.
			 */
			static void changeListenerOrientation(float x, float y);

		private:
			bool m_sound_disabled;

			type_audiobufptr m_ad;
			
			// AudioSource for the background sound.
			AudioSource *m_bgsound;

			ALCcontext *m_audiocontext;
			ALCdevice  *m_audiodevice;
            std::vector<boost::weak_ptr<AudioBuffer> > m_audiobufvec;

            static float m_savedvolume;
	};
	
	// Allow the compiler to inline trivial functions
	inline void AudioManager::setVolume(float vol) { alListenerf(AL_GAIN, vol); }
	inline void AudioManager::mute() { alGetListenerf(AL_GAIN, &m_savedvolume); setVolume(0); }
	inline void AudioManager::unmute() { setVolume(m_savedvolume); }
	inline void AudioManager::changeListenerPosition(float x, float y) { alListener3f(AL_POSITION, x, y, 0); }
	inline void AudioManager::changeListenerOrientation(float x, float y) {
		// We move along the x-y-plane and look along the x axis.
		ALfloat vec[6] = { x, y, 0.0, 0.0, 0.0, 1.0};
		alListenerfv(AL_ORIENTATION, vec); 
	}
	
}//FIFE
#endif
