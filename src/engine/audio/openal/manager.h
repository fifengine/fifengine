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

#ifndef FIFE_AUDIO_MANAGER_H
#define FIFE_AUDIO_MANAGER_H

// Standard C++ library includes
#include <string>
#include <vector>

// Platform specific includes
#include "util/fifeint.h"

// 3rd party library includes
#include <AL/al.h>
#include <AL/alc.h>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "singleton.h"

#include "buffer.h"
#include "source.h"

namespace FIFE { namespace audio {

	/** This class manages the base audio system.
	 */
	class Manager : public DynamicSingleton<Manager> {
		public:
			/** Initialises the audio system.
			 */
			Manager();
			
			~Manager() {
				delete m_bgsound; // added this (zahlman)
				//m_bufvec.clear(); <-- useless. Won't delete pointed-at things,
				//and the vector destructor will clean up the vector's memory anyway.
				if (!m_sound_disabled) {
					//alcMakeContextCurrent(NULL);
					// I think I read somewhere that you should NOT do above call on exit
					alcDestroyContext(m_context);
					alcCloseDevice(m_device);
				}
			}

			/** Returns a pointer to an AudioBuffer that can be used to acquire the 
			 * OpenAL buffers.
			 * 
			 * @param filename The name of the audio file that shall be loaded.
			 */
			type_bufptr getBufferFromFile(const std::string &filename);

			/** Sets the background sound.
			 * 
			 * @param file The filename of the background audio file. Use "" to turn
			 * it off.
			 */
			void setAmbientSound(const std::string &file);

			/** Sets the volume by changed the gain of the listener.
			 * 
			 * @param vol The volume value. 0=silence ... 1.0=normal loudness.
			 */
			static void setVolume(float vol) { alListenerf(AL_GAIN, vol); }

			/** Mutes the sound system.
			 */
			static void mute() { 
				alGetListenerf(AL_GAIN, &m_savedvolume); 
				setVolume(0);
			}
			
			/** Unmutes the sounds to volume before mute() was called.
			 */
			static void unmute() { setVolume(m_savedvolume); }
			
			/** Changes the position of the listener (alter ego).
			 * 
			 * @param x The position on the abscissa of the listener.
			 * @param y The position on the ordinate of the listener.
			 */
			static void changeListenerPosition(float x, float y) {
				alListener3f(AL_POSITION, x, y, 0);
			}
			
			/** Changes the orientation of the listener (alter ego).
			 * 
			 * @param x The position on the abscissa to which listener is directed.
			 * @param y The position on the ordinate to which listener is directed.
			 */
			static void changeListenerOrientation(float x, float y) {
				// We move along the xy plane and look along the x axis.
				ALfloat vec[6] = { x, y, 0.0, 0.0, 0.0, 1.0};
				alListenerfv(AL_ORIENTATION, vec); 
			}

		private:
			typedef std::vector<boost::weak_ptr<Buffer> > type_bufvec;

			bool         m_sound_disabled;

			type_bufptr  m_ad;
			
			// AudioSource for the background sound.
			Source*      m_bgsound;

			ALCcontext*  m_context;
			ALCdevice*   m_device;
			type_bufvec  m_bufvec;

			static float m_savedvolume;
	};
} } //FIFE::audio
#endif
