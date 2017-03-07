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

#ifndef FIFE_SOUNDEMITTER_H_
#define FIFE_SOUNDEMITTER_H_

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/modelcoords.h"

#include "soundclip.h"

namespace FIFE {

	/** State of the audio file
	 */
	enum SoundStateType {
		SD_UNKNOWN_STATE,
		SD_INITIAL_STATE,
		SD_PLAYING_STATE,
		SD_PAUSED_STATE,
		SD_STOPPED_STATE
	};

	/** Listener interface for Emitter.
	 */
	class SoundEmitterListener {
	public:
		virtual ~SoundEmitterListener() {};
		
		/** Called when a sound finished playing.
		 * @param emitterId The id of emitter.
		 * @param SoundClipId The id of sound clip aka ResourceHandle.
		 */
		virtual void onSoundFinished(uint32_t emitterId, uint32_t soundClipId) = 0;
	};

	class SoundEffect;
	class SoundFilter;
	class SoundManager;

	/** The class for playing audio files
	 */
	class SoundEmitter {
	public:

		SoundEmitter(SoundManager* manager, uint32_t uid);
		~SoundEmitter();

		void setSource(ALuint source);
		ALuint getSource() const;
		bool isActive() const;

		/** Called once a frame from the SoundManager.
		 */
		void update();

		/** Returns the emitter-id
		 */
		uint32_t getId() const;

		/** Sets Positioning-Type
		 * Default is false
		 *
		 * @param relative If set to true, the emitters position will be interpreted relative to the listener object
		 *
		 */
		void setRelativePositioning(bool relative);

		/** Return Positioning-Type
		 */
		bool isRelativePositioning() const;

		/** Sets the distance under which the volume for the SoundEmitter would normally drop by half (before
		 *  being influenced by rolloff factor or max distance. 
		 */
		void setReferenceDistance(float distance);

		/** Return the reference distance.
		 */
		float getReferenceDistance() const;

		/** Sets the max distance used with the Inverse Clamped Distance Model to set the distance where
		 *  there will no longer be any attenuation of the source.
		 */
		void setMaxDistance(float distance);

		/** Return the max distance.
		 */
		float getMaxDistance() const;

		/** Sets the AL_ROLEOFF_FACTOR. Rolloff factor judges the strength of attenuation over distance.
		 *
		 * @param rolloff Rolloff factor. You'll need to do a lot of testing to find a value which suits your needs.
		 */
		void setRolloff(float rolloff);

		/** Return the AL_ROLEOFF_FACTOR. Rolloff factor judges the strength of attenuation over distance.
		 */
		float getRolloff() const;

		/** Sets the sound clip to be used by this emitter.
		 * @param soundClip SoundClipPtr of the sound to be used.
		 */
		void setSoundClip(SoundClipPtr soundClip);

		/** Get the current sound clip used by this emitter.
		 *  @return A SoundClipPtr of the sound clip.
		 */
		SoundClipPtr getSoundClip();

		/** Sets the sound clip to be used by this emitter.
		 * @param name The name of the sound to be used.
		 */
		void setSoundClip(const std::string& name);

		/** Reset the emitter, free all internal buffers
		 *
		 * @param defaultall If set to true, emitter position, velocity, gain and type will be set to the default values
		 */
		void reset(bool defaultall = false);

		/** Releases the emitter
		 */
		void release();

		/** Sets the playing mode
		 */
		void setLooping(bool loop);

		/** Return playing mode
		 */
		bool isLooping() const;

		/** Plays the associated audio file.
		 */
		void play();

		/** Plays the associated audio file with fade in and / or out.
		 */
		void play(float inTime, float outTime);

		/** Stops playing the audio file and rewinds to the beginning.
		 */
		void stop();

		/** Stops playing the audio file after time with fade out.
		 */
		void stop(float time);

		/** Pauses playing the audio file.
		 */
		void pause();

		/** Rewinds the associated audio file.
		 */
		void rewind();

		/** Sets the gain of the emitter
		 *
		 * @param gain The gain value. 0=silence ... 1.0=normal loudness.
		 */
		void setGain(float gain);

		/** Returns the gain of the emitter
		 *
		 * @return The gain value. 0=silence ... 1.0=normal loudness.
		 */
		float getGain() const;

		/** Sets the max. gain of the emitter
		 *
		 * @param gain The gain value. 0=silence ... 1.0=normal loudness.
		 */
		void setMaxGain(float gain);

		/** Returns the max. gain of the emitter
		 *
		 * @return The gain value. 0=silence ... 1.0=normal loudness.
		 */
		float getMaxGain() const;

		/** Sets the min. gain of the emitter
		 *
		 * @param gain The gain value. 0=silence ... 1.0=normal loudness.
		 */
		void setMinGain(float gain);

		/** Returns the min. gain of the emitter
		 *
		 * @return The gain value. 0=silence ... 1.0=normal loudness.
		 */
		float getMinGain() const;

		/** Tests if the audio data is stereo data or mono.
		 *
		 * @return Returns true if the audio data is stereo, false if mono.
		 */
		bool isStereo();

		/** Returns the bit resolution
		 */
		int16_t getBitResolution();

		/** Returns the sample rate
		 */
		uint64_t getSampleRate();

		/** Returns the length of the decoded length in bytes
		 */
		uint64_t getDecodedLength();

		/** Returns the duration of the sound clip in milliseconds
		 */
		uint64_t getDuration();

		/** Returns timestamp of the last play start in milliseconds
		 */
		uint32_t getPlayTimestamp();

		/** Returns true if clip is finished.
		 */
		bool isFinished();

		/** Sets the cursor position in the audio file
		 */
		void setCursor(SoundPositionType type, float value);

		/** Returns the cursor position in the audio file
		 */
		float getCursor(SoundPositionType type);

		/** Sets the position of the SoundEmitter in the virtual audio space.
		 */
		void setPosition(const AudioSpaceCoordinate& position);

		/** Return the position of the SoundEmitter in the virtual audio space.
		 */
		AudioSpaceCoordinate getPosition() const;

		/** Return if it is a positional SoundEmitter.
		 */
		bool isPosition() const;

		/** Sets the direction of the SoundEmitter in the virtual audio space.
		 */
		void setDirection(const AudioSpaceCoordinate& direction);

		/** Return the direction of the SoundEmitter in the virtual audio space.
		 */
		AudioSpaceCoordinate getDirection() const;

		/** Sets pitch multiplier. Can only be positiv.
		 */
		void setPitch(float pitch);

		/** Return pitch multiplier. Can only be positiv.
		 */
		float getPitch() const;

		/** Sets the velocity of the SoundEmitter in the virtual audio space.
		 */
		void setVelocity(const AudioSpaceCoordinate& velocity);

		/** Return the velocity of the SoundEmitter in the virtual audio space.
		 */
		AudioSpaceCoordinate getVelocity() const;

		/** Sets inner angle of the sound cone, in degrees. Default 360
		 */
		void setConeInnerAngle(float inner);

		/** Return inner angle of the sound cone, in degrees.
		 */
		float getConeInnerAngle() const;

		/** Sets outer angle of the sound cone, in degrees. Default 360
		 */
		void setConeOuterAngle(float outer);

		/** Return outer angle of the sound cone, in degrees.
		 */
		float getConeOuterAngle() const;

		/** Sets the gain when outside the oriented cone.
		 */
		void setConeOuterGain(float gain);

		/** Return the gain when outside the oriented cone.
		 */
		float getConeOuterGain() const;

		/** Returns the state of the audio file
		 */
		SoundStateType getState();

		/** Sets the group name.
		 */
		void setGroup(const std::string& group);

		/** Return the group name.
		 */
		const std::string& getGroup();

		/** Adds effect. Used from SoundEffectManager.
		 */
		void addEffect(SoundEffect* effect);

		/** Removes effect. Used from SoundEffectManager.
		 */
		void removeEffect(SoundEffect* effect);

		/** Return the number of effects. Used from SoundEffectManager.
		 */
		uint8_t getEffectCount();

		/** Return the number of the given effect. Used from SoundEffectManager.
		 */
		uint8_t getEffectNumber(SoundEffect* effect);

		/** Sets the direct filter. Used from SoundEffectManager.
		 */
		void setDirectFilter(SoundFilter* filter);

		/** Return the direct filter. Used from SoundEffectManager.
		 */
		SoundFilter* getDirectFilter();

		/** Activates effects if the Emitter got the openAL-source.
		 */
		void activateEffects();

		/** Deactivates effects if the Emitter loses the openAL-source.
		*/
		void deactivateEffects();

		/** Adds new SoundEmitter listener
		 * @param listener to add
		 */
		void addListener(SoundEmitterListener* listener);

		/** Removes associated SoundEmitter listener
		 * @param listener to remove
		 */
		void removeListener(SoundEmitterListener* listener);

	private:
		/** Internal function to attach a SoundClip to the source
		 */
		void attachSoundClip();

		/** Internal function to detach a SoundClip from the source
		 */
		void detachSoundClip();

		/** Updates OpenAL with collected data.
		 */
		void syncData();

		/** Updates fade in and out.
		 */
		void checkFade();

		/** Resets collected data to defaults.
		 */
		void resetInternData();

		/** Calls the Listeners if a sound finished
		 */
		void callOnSoundFinished();

		//! Access to the SoundManager
		SoundManager* m_manager;
		//! The openAL-source
		ALuint m_source;
		//! Applied direct sound filter
		SoundFilter* m_directFilter;
		//! The attached sound clip
		SoundClipPtr m_soundClip;
		//! Id of the attached sound clip
		uint32_t m_soundClipId;
		//! The id of the stream
		uint32_t m_streamId;
		//! The emitter-id
		uint32_t m_emitterId;

		//! buffers OpenAL data
		struct internData {
			float volume;
			float maxVolume;
			float minVolume;
			float refDistance;
			float maxDistance;
			float rolloff;
			float pitch;
			float coneInnerAngle;
			float coneOuterAngle;
			float coneOuterGain;
			AudioSpaceCoordinate position;
			AudioSpaceCoordinate direction;
			AudioSpaceCoordinate velocity;
			uint32_t playTimestamp;
			SoundStateType soundState;
			bool loop;
			bool relative;
		} m_internData;
		//! vector that indicates updates in internData
		//std::vector<bool> m_updateData;

		//! the group name
		std::string m_group;
		//! saves sample offset for played stream parts
		float m_samplesOffset;
		//! is active
		bool m_active;
		//! fade in clip
		bool m_fadeIn;
		//! fade out clip
		bool m_fadeOut;
		//! original gain
		float m_origGain;
		
		uint32_t m_fadeInStartTimestamp;
		uint32_t m_fadeInEndTimestamp;
		uint32_t m_fadeOutStartTimestamp;
		uint32_t m_fadeOutEndTimestamp;
		//! holds pointer to applied SoundEffects
		std::vector<SoundEffect*> m_effects;
		//! listeners for sound related events
		std::vector<SoundEmitterListener*> m_listeners;
	};
}

#endif
