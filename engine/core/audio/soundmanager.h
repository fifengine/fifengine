/***************************************************************************
 *   Copyright (C) 2005-2019 by the FIFE team                              *
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

	/** SoundManager state
	 */
	enum SoundManagerState {
		SM_STATE_INACTIV,
		SM_STATE_PLAY,
		SM_STATE_PAUSE,
		SM_STATE_STOP
	};

	class SoundEffectManager;
	class SoundEffect;
	class SoundFilter;
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

		/** Plays all SoundEmitters.
		 */
		void play();

		/** Pauses all SoundEmitters.
		 */
		void pause();

		/** Stops all SoundEmitters.
		 */
		void stop();

		/** Rewinds all SoundEmitters.
		 */
		void rewind();

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

		/** Release given emitter
		 *
		 * @param emitter The emitter-instance to destroy.
		 */
		void deleteEmitter(SoundEmitter* emitter);

		/** Release the source handle
		 *
		 * @param emitter The emitter-instance.
		 */
		void releaseSource(SoundEmitter* emitter);

		/** Creates SoundEffect of the specific type.
		 * @param type See SoundEffectType
		 */
		SoundEffect* createSoundEffect(SoundEffectType type);

		/** Creates EaxReverb SoundEffect and loads the specific preset type.
		 * @param type See SoundEffectPreset
		 */
		SoundEffect* createSoundEffectPreset(SoundEffectPreset type);

		/** Deletes given SoundEffect.
		 * Deactivates for SoundEmitters are automatically done.
		 */
		void deleteSoundEffect(SoundEffect* effect);

		/** Enables given SoundEffect.
		 */
		void enableSoundEffect(SoundEffect* effect);

		/** Disables given SoundEffect.
		 */
		void disableSoundEffect(SoundEffect* effect);

		/** Adds given SoundEmitter to the specific SoundEffect.
		 * Note: A SoundEmitter can only have a limited number of effects, usually 4.
		 */
		void addEmitterToSoundEffect(SoundEffect* effect, SoundEmitter* emitter);

		/** Removes given SoundEmitter from the specific SoundEffect.
		 */
		void removeEmitterFromSoundEffect(SoundEffect* effect, SoundEmitter* emitter);

		/** Adds given SoundFilter to the SoundEffect.
		 * Note: A SoundEffect can only have one SoundFilter.
		 */
		void addSoundFilterToSoundEffect(SoundEffect* effect, SoundFilter* filter);

		/** Removes given SoundFilter from the SoundEffect.
		 */
		void removeSoundFilterFromSoundEffect(SoundEffect* effect, SoundFilter* filter);

		/** Internal function to do the OpenAL calls to activate the SoundEffect for the SoundEmitter.
		 */
		void activateEffect(SoundEffect* effect, SoundEmitter* emitter);

		/** Internal function to do the OpenAL calls to deactivate the SoundEffect for the SoundEmitter.
		 */
		void deactivateEffect(SoundEffect* effect, SoundEmitter* emitter);

		/** Creates SoundFilter of the specific type.
		 * @param type See SoundFilterType
		 */
		SoundFilter* createSoundFilter(SoundFilterType type);

		/** Deletes given SoundFilter.
		 * Deactivates for SoundEmitters and Effects are automatically done.
		 */
		void deleteSoundFilter(SoundFilter* filter);

		/** Enables given direct SoundFilter. Only SoundFilters for SoundEmitters are meant.
		 */
		void enableDirectSoundFilter(SoundFilter* filter);

		/** Disables given SoundFilter. Only SoundFilters for SoundEmitters are meant.
		 */
		void disableDirectSoundFilter(SoundFilter* filter);

		/** Adds given SoundEmitter to the specific direct SoundFilter
		 * Note: A SoundEmitter can only have one direct filter.
		 */
		void addEmitterToDirectSoundFilter(SoundFilter* filter, SoundEmitter* emitter);

		/** Removes given SoundEmitter from the specific direct SoundFilter.
		 */
		void removeEmitterFromDirectSoundFilter(SoundFilter* filter, SoundEmitter* emitter);

		/** Internal function to do the OpenAL calls to activate the SoundFilter for the SoundEmitter.
		 */
		void activateFilter(SoundFilter* filter, SoundEmitter* emitter);

		/** Internal function to do the OpenAL calls to deactivate the SoundFilter for the SoundEmitter.
		 */
		void deactivateFilter(SoundFilter* filter, SoundEmitter* emitter);

		/** Adds the emitter to group.
		 *  Called from the emitter after a setGroup() call.
		 *
		 * @param emitter The emitter-instance.
		 */
		void addToGroup(SoundEmitter* emitter);
		
		/** Removes the emitter from group.
		 *  Called from the emitter if new group is different as the old.
		 *
		 * @param emitter The emitter-instance.
		 */
		void removeFromGroup(SoundEmitter* emitter);

		/** Remove group and resets the group for the affected emitters.
		 *
		 * @param group The group name.
		 */
		void removeGroup(const std::string& group);

		/** Remove all groups and resets the group of affected emitters.
		 */
		void removeAllGroups();

		/** Plays all emitters of the group.
		 *
		 * @param group The group name.
		 */
		void play(const std::string& group);

		/** Pauses all emitters of the group.
		 *
		 * @param group The group name.
		 */
		void pause(const std::string& group);

		/** Stops all emitters of the group.
		 *
		 * @param group The group name.
		 */
		void stop(const std::string& group);

		/** Rewinds all emitters of the group.
		 *
		 * @param group The group name.
		 */
		void rewind(const std::string& group);

		/** Sets gain for all emitters of the group.
		 *
		 * @param group The group name.
		 * @param gain The gain value.
		 */
		void setGain(const std::string& group, float gain);

		/** Sets max gain for all emitters of the group.
		 *
		 * @param group The group name.
		 * @param gain The max gain value.
		 */
		void setMaxGain(const std::string& group, float gain);

		/** Sets min gain for all emitters of the group.
		 *
		 * @param group The group name.
		 * @param gain The min gain value.
		 */
		void setMinGain(const std::string& group, float gain);

	private:
		typedef std::map<std::string, std::vector<SoundEmitter*> > EmitterGroups;
		typedef EmitterGroups::iterator EmitterGroupsIterator;

		/** Sets the source handle
		 *
		 * @param emitter The Emitter pointer.
		 */
		void setEmitterSource(SoundEmitter* emitter);

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
		//! State of the SoundManager
		SoundManagerState m_state;

		//! Holds handles for sources
		ALuint m_sources[MAX_SOURCES];
		//! Maximal created sources, can be different to MAX_SOURCES
		uint16_t m_createdSources;
		//! Holds free handles for sources
		std::queue<ALuint> m_freeSources;
		//! Map that holds active Emitters together with the used source handle
		std::map<SoundEmitter*, ALuint> m_activeEmitters;

		SoundEffectManager* m_effectManager;

		//! A map that holds the groups together with the appended emitters.
		EmitterGroups m_groups;
	};
}
#endif
