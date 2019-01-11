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

%module soundeffect
%{
#include "audio/effects/soundeffect.h"
%}

%include "model/metamodel/modelcoords.i"

namespace FIFE {

	class SoundFilter;

	enum SoundEffectType {
		SE_EFFECT_NULL,
		SE_EFFECT_REVERB,
		SE_EFFECT_CHORUS,
		SE_EFFECT_DISTORTION,
		SE_EFFECT_ECHO,
		SE_EFFECT_FLANGER,
		SE_EFFECT_FREQUENCY_SHIFTER,
		SE_EFFECT_VOCAL_MORPHER,
		SE_EFFECT_PITCH_SHIFTER,
		SE_EFFECT_RING_MODULATOR,
		SE_EFFECT_AUTOWAH,
		SE_EFFECT_COMPRESSOR,
		SE_EFFECT_EQUALIZER,
		SE_EFFECT_EAXREVERB
	};

	class SoundEffect {
	public:
		SoundEffectType getEffectType() const;
		bool isEnabled() const;
	protected:
		SoundEffect();
	};

	class Reverb : public SoundEffect {
	public:
		void setDensity(float value);
		float getDensity() const;
		void setDiffusion(float value);
		float getDiffusion() const;
		void setGain(float value);
		float getGain() const;
		void setGainHf(float value);
		float getGainHf() const;
		void setDecayTime(float value);
		float getDecayTime() const;
		void setDecayHfRatio(float value);
		float getDecayHfRatio() const;
		void setReflectionsGain(float value);
		float getReflectionsGain() const;
		void setReflectionsDelay(float value);
		float getReflectionsDelay() const;
		void setLateReverbGain(float value);
		float getLateReverbGain() const;
		void setLateReverbDelay(float value);
		float getLateReverbDelay() const;
		void setAirAbsorptionGainHf(float value);
		float getAirAbsorptionGainHf() const;
		void setRoomRolloffFactor(float value);
		float getRoomRolloffFactor() const;
		void setDecayHfLimit(bool value);
		bool isDecayHfLimit() const;
	private:
		Reverb();
	};

	class Chorus : public SoundEffect {
	public:
		void setWaveformTriangle(bool value);
		bool isWaveformTriangle() const;
		void setPhase(int32_t value);
		int32_t getPhase() const;
		void setRate(float value);
		float getRate() const;
		void setDepth(float value);
		float getDepth() const;
		void setFeedback(float value);
		float getFeedback() const;
		void setDelay(float value);
		float getDelay() const;
	private:
		Chorus();
	};

	class Distortion : public SoundEffect {
	public:
		void setEdge(float value);
		float getEdge() const;
		void setGain(float value);
		float getGain() const;
		void setLowpassCutoff(float value);
		float getLowpassCutoff() const;
		void setEqCenter(float value);
		float getEqCenter() const;
		void setEqBandwidth(float value);
		float getEqBandwidth() const;
	private:
		Distortion();
	};

	class Echo : public SoundEffect {
	public:
		void setDelay(float value);
		float getDelay() const;
		void setLrDelay(float value);
		float getLrDelay() const;
		void setDamping(float value);
		float getDamping() const;
		void setFeedback(float value);
		float getFeedback() const;
		void setSpread(float value);
		float getSpread() const;
	private:
		Echo();
	};

	class Flanger : public SoundEffect {
	public:
		void setWaveformTriangle(bool value);
		bool isWaveformTriangle() const;
		void setPhase(int32_t value);
		int32_t getPhase() const;
		void setRate(float value);
		float getRate() const;
		void setDepth(float value);
		float getDepth() const;
		void setFeedback(float value);
		float getFeedback() const;
		void setDelay(float value);
		float getDelay() const;
	private:
		Flanger();
	};

	class FrequencyShifter : public SoundEffect {
	public:
		void setFrequency(float value);
		float getFrequency() const;
		void setLeftDirection(uint8_t value);
		uint8_t getLeftDirection() const;
		void setRightDirection(uint8_t value);
		uint8_t getRightDirection() const;
	private:
		FrequencyShifter();
	};

	class VocalMorpher : public SoundEffect {
	public:
		void setPhonemeA(uint16_t value);
		uint16_t getPhonemeA() const;
		void setPhonemeB(uint16_t value);
		uint16_t getPhonemeB() const;
		void setPhonemeCoarseA(int16_t value);
		int16_t getPhonemeCoarseA() const;
		void setPhonemeCoarseB(int16_t value);
		int16_t getPhonemeCoarseB() const;
		void setWaveform(uint8_t value);
		uint8_t getWaveform() const;
		void setRate(float value);
		float getRate() const;
	private:
		VocalMorpher();
	};

	class PitchShifter : public SoundEffect {
	public:
		void setCoarseTune(int16_t value);
		int16_t getCoarseTune() const;
		void setFineTune(int16_t value);
		int16_t getFineTune() const;
	private:
		PitchShifter();
	};

	class RingModulator : public SoundEffect {
	public:
		void setFrequency(float value);
		float getFrequency() const;
		void setHighpassCutoff(float value);
		float getHighpassCutoff() const;
		void setWaveform(uint8_t value);
		uint8_t getWaveform() const;
	private:
		RingModulator();
	};

	class Autowah : public SoundEffect {
	public:
		void setAttackTime(float value);
		float getAttackTime() const;
		void setReleaseTime(float value);
		float getReleaseTime() const;
		void setResonance(float value);
		float getResonance() const;
		void setPeakGain(float value);
		float getPeakGain() const;
	private:
		Autowah();
	};

	class Compressor : public SoundEffect {
	public:
		void setCompressor(bool value);
		bool isCompressor() const;
	private:
		Compressor();
	};

	class Equalizer : public SoundEffect {
	public:
		void setLowGain(float value);
		float getLowGain() const;
		void setLowCutoff(float value);
		float getLowCutoff() const;
		void setMid1Gain(float value);
		float getMid1Gain() const;
		void setMid1Center(float value);
		float getMid1Center() const;
		void setMid1Width(float value);
		float getMid1Width() const;
		void setMid2Gain(float value);
		float getMid2Gain() const;
		void setMid2Center(float value);
		float getMid2Center() const;
		void setMid2Width(float value);
		float getMid2Width() const;
		void setHighGain(float value);
		float getHighGain() const;
		void setHighCutoff(float value);
		float getHighCutoff() const;
	private:
		Equalizer();
	};

	class EaxReverb : public SoundEffect {
	public:
		void setDensity(float value);
		float getDensity() const;
		void setDiffusion(float value);
		float getDiffusion() const;
		void setGain(float value);
		float getGain() const;
		void setGainHf(float value);
		float getGainHf() const;
		void setGainLf(float value);
		float getGainLf() const;
		void setDecayTime(float value);
		float getDecayTime() const;
		void setDecayHfRatio(float value);
		float getDecayHfRatio() const;
		void setDecayLfRatio(float value);
		float getDecayLfRatio() const;
		void setReflectionsGain(float value);
		float getReflectionsGain() const;
		void setReflectionsDelay(float value);
		float getReflectionsDelay() const;
		void setReflectionsPan(const AudioSpaceCoordinate& coordinate);
		AudioSpaceCoordinate getReflectionsPan() const;
		void setLateReverbGain(float value);
		float getLateReverbGain() const;
		void setLateReverbDelay(float value);
		float getLateReverbDelay() const;
		void setLateReverbPan(const AudioSpaceCoordinate& coordinate);
		AudioSpaceCoordinate getLateReverbPan() const;
		void setEchoTime(float value);
		float getEchoTime() const;
		void setEchoDepth(float value);
		float getEchoDepth() const;
		void setModulationTime(float value);
		float getModulationTime() const;
		void setModulationDepth(float value);
		float getModulationDepth() const;
		void setAirAbsorptionGainHf(float value);
		float getAirAbsorptionGainHf() const;
		void setHfReference(float value);
		float getHfReference() const;
		void setLfReference(float value);
		float getLfReference() const;
		void setRoomRolloffFactor(float value);
		float getRoomRolloffFactor() const;
		void setDecayHfLimit(bool value);
		bool isDecayHfLimit() const;
	private:
		EaxReverb();
	};
}

