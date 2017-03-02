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

%module soundemitter
%{
#include "audio/soundemitter.h"
%}

%include "model/metamodel/modelcoords.i"

namespace FIFE {

	%feature("director") SoundEmitterListener;
	class SoundEmitterListener {
	public:
		virtual ~SoundEmitterListener() {};
		virtual void onSoundFinished(uint32_t emitterId, uint32_t soundClipId) = 0;
	};

	enum SoundPositionType {
		SD_SAMPLE_POS,
		SD_TIME_POS,
		SD_BYTE_POS
	};

	enum SoundStateType {
		SD_UNKNOWN_STATE,
		SD_INITIAL_STATE,
		SD_PLAYING_STATE,
		SD_PAUSED_STATE,
		SD_STOPPED_STATE
	};

	class SoundDecoder;
	class SoundManager;

	class SoundEmitter {
	public:
		SoundEmitter(SoundManager* manager, uint32_t uid);
		~SoundEmitter();

		uint32_t getId() const;

		void setSoundClip(SoundClipPtr soundClip);
		SoundClipPtr getSoundClip();
		void reset(bool defaultall = false);
		void release();

		void play();
		void playWithFadeIn(float time);
		void pause();
		void stop();
		void stopWithFadeOut(float time);
		void rewind();

		void setGain(float gain);
		float getGain() const;
		void setMaxGain(float gain);
		float getMaxGain() const;
		void setMinGain(float gain);
		float getMinGain() const;

		void setLooping(bool loop);
		bool isLooping() const;
		void setRolloff(float rolloff);
		float getRolloff() const;
		void setPositioning(bool relative);
		bool isPositioning() const;
		void setPosition(const AudioSpaceCoordinate& position);
		AudioSpaceCoordinate getPosition() const;
		void setReferenceDistance(float distance);
		float getReferenceDistance() const;
		void setMaxDistance(float distance);
		float getMaxDistance() const;
		void setDirection(const AudioSpaceCoordinate& direction);
		AudioSpaceCoordinate getDirection() const;
		void setVelocity(const AudioSpaceCoordinate& velocity);
		AudioSpaceCoordinate getVelocity() const;
		void setPitch(float pitch);
		float getPitch() const;

		void setConeInnerAngle(float inner);
		float getConeInnerAngle() const;
		void setConeOuterAngle(float outer);
		float getConeOuterAngle() const;
		void setConeOuterGain(float gain);
		float getConeOuterGain() const;

		bool isStereo();
		int16_t getBitResolution();
		uint64_t getSampleRate();
		uint64_t getDecodedLength();
		uint64_t getDuration();

		void setCursor(SoundPositionType type, float value);
		float getCursor(SoundPositionType type);

		SoundStateType getState();

		void addListener(SoundEmitterListener* listener);
		void removeListener(SoundEmitterListener* listener);
	};
}

