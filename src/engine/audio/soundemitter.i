// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

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

	class SoundManager;

	class SoundEmitter {
	public:
		uint32_t getId() const;
		bool isActive() const;
		uint8_t getEffectCount();

		void setSoundClip(const SoundClipPtr& soundClip);
		SoundClipPtr getSoundClip();
		void reset(bool defaultall = false);
		void release();

		void play();
		void play(float inTime, float outTime);
		void pause();
		void stop();
		void stop(float time);
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
		void setRelativePositioning(bool relative);
		bool isRelativePositioning() const;
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

		void setGroup(const std::string& group);
		const std::string& getGroup();

		void addListener(SoundEmitterListener* listener);
		void removeListener(SoundEmitterListener* listener);
	
	private:
		SoundEmitter(SoundManager* manager, uint32_t uid);
	};
}
