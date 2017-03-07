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

#ifndef FIFE_ACTIONAUDIO_H
#define FIFE_ACTIONAUDIO_H

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/modelcoords.h"

namespace FIFE {

	/** The class for holding audio data per Action.
	 */
	class ActionAudio {
	public:

		ActionAudio();
		~ActionAudio();

		/** Sets the name of the sound file
		 */
		void setSoundFileName(const std::string& name);

		/** Return the name of the sound file
		 */
		const std::string& getSoundFileName() const;

		/** Sets the name of the group to which the emitter is added.
		 */
		void setGroupName(const std::string& name);

		/** Return the name of the group to which the emitter is added.
		 */
		const std::string& getGroupName() const;

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

		/** Sets pitch multiplier. Can only be positiv.
		 */
		void setPitch(float pitch);

		/** Return pitch multiplier. Can only be positiv.
		 */
		float getPitch() const;

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

		/** Sets the velocity of the SoundEmitter in the virtual audio space.
		 */
		void setVelocity(const AudioSpaceCoordinate& velocity);

		/** Return the velocity of the SoundEmitter in the virtual audio space.
		 */
		const AudioSpaceCoordinate& getVelocity() const;

		/** Sets the playing mode
		 */
		void setLooping(bool loop);

		/** Return playing mode
		 */
		bool isLooping() const;

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

		/** Sets if the sound should use the instance direction
		 */
		void setDirection(bool direction);

		/** Return true if the Instance direction is used, false for undirected
		 */
		bool isDirection() const;
	
	private:
		std::string m_name;
		std::string m_group;
		float m_volume;
		float m_maxVolume;
		float m_minVolume;
		float m_refDistance;
		float m_maxDistance;
		float m_rolloff;
		float m_pitch;
		float m_coneInnerAngle;
		float m_coneOuterAngle;
		float m_coneOuterGain;
		AudioSpaceCoordinate m_velocity;
		bool m_looping;
		bool m_positioning;
		bool m_direction;

	};
}

#endif