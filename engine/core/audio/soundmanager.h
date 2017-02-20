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
#include <queue>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/modelcoords.h"
#include "util/base/singleton.h"

#include "fife_openal.h"
#include "soundconfig.h"

namespace FIFE {

	/** Distance model from OpenAL
	 */
	enum SoundDistanceModelType {
		SD_DISTANCE_NONE,
		SD_DISTANCE_INVERSE,
		SD_DISTANCE_INVERSE_CLAMPED,
		SD_DISTANCE_LINEAR,
		SD_DISTANCE_LINEAR_CLAMPED,
		SD_DISTANCE_EXPONENT,
		SD_DISTANCE_EXPONENT_CLAMPED
	};

	class SoundEmitter;

	class SoundManager : public DynamicSingleton<SoundManager> {
	public:

		SoundManager();

		~SoundManager();

		/** Initializes the audio system
		 */
		void init();

		/** Returns true if audio module is active
		*/
		bool isActive() const;

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

		/** Returns a pointer to an allocated emitter-instance
		 *
		 * @param name The name of the SoundClip.
		 */
		SoundEmitter* createEmitter(const std::string& name);

		/** Release an emitter-instance given by emitter-id
		 *
		 * @param emitterId The id of the Emitter.
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

		/** Sets the distance model.
		 */
		void setDistanceModel(SoundDistanceModelType model);

		/** Return the distance mode.
		 */
		SoundDistanceModelType getDistanceModel() const;

		/** Sets the position of the listener (alter ego).
		 */
		void setListenerPosition(const AudioSpaceCoordinate& position);

		/** Return the position of the listener (alter ego).
		 */
		AudioSpaceCoordinate getListenerPosition() const;

		/** Sets the orientation of the listener (alter ego).
		 */
		void setListenerOrientation(const AudioSpaceCoordinate& orientation);

		/** Return the orientation of the listener (alter ego).
		 */
		AudioSpaceCoordinate getListenerOrientation() const;

		/** Sets the velocity of the listener (alter ego).
		 */
		void setListenerVelocity(const AudioSpaceCoordinate& velocity);

		/** Return the velocity of the listener (alter ego).
		 */
		AudioSpaceCoordinate getListenerVelocity() const;

		/** Sets factor for doppler effect.
		 */
		void setDopplerFactor(float factor);

		/** Return factor for doppler effect.
		 */
		float getDopplerFactor() const;

		/** Sets the maximal listener distance.
		 *  If it is larger the emitter turns off, or on if it is smaller.
		 */
		void setListenerMaxDistance(float distance);

		/** Return the maximal listener distance.
		 */
		float getListenerMaxDistance() const;

		void requestSource(SoundEmitter* emitter);
		void releaseSource(SoundEmitter* emitter);

	private:
		bool isInRange(SoundEmitter* emitter) const;

		//! emitter-vector, holds all emitters
		std::vector<SoundEmitter*> m_emitterVec;
		//! OpenAL context
		ALCcontext* m_context;
		//! OpenAL device
		ALCdevice* m_device;
		//! volume before mute() was called
		float m_muteVol;
		//! volume to support setVolume-calls before initialization
		float m_volume;
		//! distance that removes a active Emitter
		float m_maxDistance;
		//! Selected distance model
		SoundDistanceModelType m_distanceModel;

		//! Holds handles for sources
		ALuint m_sources[MAX_SOURCES];
		//! Maximal created sources, can be different to MAX_SOURCES
		uint16_t m_createdSources;
		//! Holds free handles for sources
		std::queue<ALuint> m_freeSources;
		//! Map that holds active Emitters together with the used source handle
		std::map<SoundEmitter*, ALuint> m_activeEmitters;
		//! List that holds Emitters in the queue
		std::list<SoundEmitter*> m_waitingEmitters;

		//! A map that holds the groups together with the appended emitters.
		std::map<std::string, std::vector<SoundEmitter*> > m_groups;
	};
}
#endif
