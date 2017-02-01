/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_SOUNDMANAGER_H
#define FIFE_SOUNDMANAGER_H

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "fife_openal.h"

namespace FIFE {
	class SoundEmitter;

	class SoundManager {
	public:

		SoundManager();

		~SoundManager();

		/** Initializes the audio system
		 */
		void init();

		/** Called once a frame and updates the sound objects.
		 */
		void update();

		/** Returns a pointer to an emitter-instance given by emitterId
		 *
		 * @param emitterId The id of the Emitter
		 *
		 */
		SoundEmitter* getEmitter(uint32_t emitterId) const;

		/** Returns a pointer to an allocated emitter-instance
		 */
		SoundEmitter* createEmitter();

		/** Release an emitter-instance given by emitter-id
		 */
		void releaseEmitter(uint32_t emitterId);

		/** Returns an openAL context
		 */
		ALCcontext* getContext() const;

		/** Sets the Master Volume
		 *
		 * @param vol The volume value. 0=silence ... 1.0=normal loudness.
		 */
		void setVolume(float vol);

		/** Return the Master Volume
		*/
		float getVolume() const;

		/** Mute
		 */
		void mute();

		/** Unmutes to volume before mute() was called.
		 */
		void unmute();

		/** Sets the position of the listener (alter ego).
		 */
		void setListenerPosition(float x, float y, float z);

		/** Sets the orientation of the listener (alter ego).
		 */
		void setListenerOrientation(float x, float y, float z);

		/** Sets the velocity of the listener (alter ego).
		 */
		void setListenerVelocity(float x, float y, float z);

		/** Returns true if audio module is active
		 */
		bool isActive() const;

	private:
		// emitter-vector
		std::vector<SoundEmitter*> m_emitterVec;
		// OpenAL context
		ALCcontext* m_context;
		// OpenAL device
		ALCdevice* m_device;
		// volume before mute() was called
		float m_muteVol;
		// volume to support setVolume-calls before initialization
		float m_volume;
	};
}
#endif
