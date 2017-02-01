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
#include <boost/function.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/time/timeevent.h"

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
		 * @param id of emitter.
		 */
		virtual void onSoundFinished(uint32_t id) = 0;
	};

	class SoundManager;

	/** The class for playing audio files
	 */
	class SoundEmitter : private TimeEvent {
	public:
		typedef boost::function0<void> type_callback;

		SoundEmitter(SoundManager* manager, uint32_t uid);
		~SoundEmitter();

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
		void setPositioning(bool relative);

		/** Sets the AL_ROLEOFF_FACTOR. Rolloff factor judges the strength of attenuation over distance.
		 *
		 * @param rolloff Rolloff factor. You'll need to do a lot of testing to find a value which suits your needs.
		 */
		void setRolloff(float rolloff);

		/** Sets the sound clip to be used by this emitter.
		 * @param soundClip SoundClipPtr of the sound to be used.
		 */
		void setSoundClip(SoundClipPtr soundClip);

		/** Get the current sound clip used by this emitter.
		 *  @return A SoundClipPtr of the sound clip.
		 */
		SoundClipPtr getSoundClip();

		/** Sets the callback to use when the STREAM has finished being played.
		 *  NOTE: This only works with streaming audio.
		 *
		 * @param cb function callback
		 */
		void setCallback(const type_callback& cb);

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

		/** Plays the associated audio file.
		 */
		void play();

		/** Stops playing the audio file and rewinds to the beginning
		 */
		void stop();

		/** Pauses playing the audio file
		 */
		void pause();

		/** Sets the gain of the emitter
		 *
		 * @param gain The gain value. 0=silence ... 1.0=normal loudness.
		 */
		void setGain(float gain);

		/** Returns the gain of the emitter
		 *
		 * @return The gain value. 0=silence ... 1.0=normal loudness.
		 */
		float getGain();

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

		/** Sets the cursor position in the audio file
		 */
		void setCursor(SoundPositionType type, float value);

		/** Returns the cursor position in the audio file
		 */
		float getCursor(SoundPositionType type);

		/** Sets the position of the SoundEmitter in the virtual audio space.
		 */
		void setPosition(float x, float y, float z);

		/** Sets the velocity of the SoundEmitter in the virtual audio space.
		 */
		void setVelocity(float x, float y, float z);

		/** Returns the state of the audio file
		 */
		SoundStateType getState();

	private:
		/** Implementation of the pure virtual function from TimeEvent to update streaming
		 */
		virtual void updateEvent(uint32_t time);

		/** Internal function to attach a SoundClip to the source
		 */
		void attachSoundClip();

		// Access to the SoundManager
		SoundManager* m_manager;
		// The openAL-source
		ALuint m_source;
		// The attached sound clip
		SoundClipPtr m_soundClip;
		// Id of the attached sound clip
		uint32_t m_soundClipId;
		// The id of the stream
		uint32_t m_streamId;
		// The emitter-id
		uint32_t m_emitterId;
		// Loop?
		bool m_loop;
		type_callback m_callback;
	};
}

#endif
