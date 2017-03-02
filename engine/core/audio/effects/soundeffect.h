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

#ifndef FIFE_SOUNDEFFECT_H
#define FIFE_SOUNDEFFECT_H

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/modelcoords.h"

#include "audio/fife_openal.h"
#include "audio/soundconfig.h"

namespace FIFE {

	class SoundFilter;

	class SoundEffect {
	public:
		SoundEffect();
		virtual ~SoundEffect();

		ALuint getEffectId() const;

		void setSlotId(ALuint slot);
		ALuint getSlotId();

		SoundEffectType getEffectType() const;

		void setEnabled(bool enabled);
		bool isEnabled() const;

		void setFilter(SoundFilter* filter);
		SoundFilter* getFilter();

	protected:
		//! Effect object id
		ALuint m_effect;
		//! Effect slot id;
		ALuint m_slot;
		//! Sound effect type
		SoundEffectType m_effectType;
		//! Effect enabled
		bool m_enabled;
		//! Additional filter effect
		SoundFilter* m_filter;
	};

	class Reverb : public SoundEffect {
	public:
		Reverb();

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
		float m_density;
		float m_diffusion;
		float m_gain;
		float m_gainHf;
		float m_decayTime;
		float m_decayHfRatio;
		float m_reflectionsGain;
		float m_reflectionsDelay;
		float m_lateReverbGain;
		float m_lateReverbDelay;
		float m_airAbsorptionGainHf;
		float m_roomRolloffFactor;
		bool m_decayHfLimit;
	};

	class Chorus : public SoundEffect {
	public:
		Chorus();

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
		bool m_waveformTriangle;
		int32_t m_phase;
		float m_rate;
		float m_depth;
		float m_feedback;
		float m_delay;
	};

	class Distortion : public SoundEffect {
	public:
		Distortion();

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
		float m_edge;
		float m_gain;
		float m_lowpassCutoff;
		float m_eqCenter;
		float m_eqBandwidth;
	};

	class Echo : public SoundEffect {
	public:
		Echo();

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
		float m_delay;
		float m_lrDelay;
		float m_damping;
		float m_feedback;
		float m_spread;
	};

	class Flanger : public SoundEffect {
	public:
		Flanger();

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
		bool m_waveformTriangle;
		int32_t m_phase;
		float m_rate;
		float m_depth;
		float m_feedback;
		float m_delay;
	};

	class FrequencyShifter : public SoundEffect {
	public:
		FrequencyShifter();

		void setFrequency(float value);
		float getFrequency() const;
		void setLeftDirection(uint8_t value);
		uint8_t getLeftDirection() const;
		void setRightDirection(uint8_t value);
		uint8_t getRightDirection() const;

	private:
		float m_frequency;
		uint8_t m_leftDirection;
		uint8_t m_rightDirection;
	};

	class VocalMorpher : public SoundEffect {
	public:
		VocalMorpher();

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
		uint16_t m_phonemeA;
		uint16_t m_phonemeB;
		int16_t m_phonemeCoarseA;
		int16_t m_phonemeCoarseB;
		uint8_t m_waveform;
		float m_rate;
	};

	class PitchShifter : public SoundEffect {
	public:
		PitchShifter();

		void setCoarseTune(int16_t value);
		int16_t getCoarseTune() const;
		void setFineTune(int16_t value);
		int16_t getFineTune() const;

	private:
		int16_t m_coarseTune;
		int16_t m_fineTune;
	};

	class RingModulator : public SoundEffect {
	public:
		RingModulator();

		void setFrequency(float value);
		float getFrequency() const;
		void setHighpassCutoff(float value);
		float getHighpassCutoff() const;
		void setWaveform(uint8_t value);
		uint8_t getWaveform() const;

	private:
		float m_frequency;
		float m_highpassCutoff;
		uint8_t m_waveform;
	};

	class Autowah : public SoundEffect {
	public:
		Autowah();

		void setAttackTime(float value);
		float getAttackTime() const;
		void setReleaseTime(float value);
		float getReleaseTime() const;
		void setResonance(float value);
		float getResonance() const;
		void setPeakGain(float value);
		float getPeakGain() const;

	private:
		float m_attackTime;
		float m_releaseTime;
		float m_resonance;
		float m_peakGain;
	};

	class Compressor : public SoundEffect {
	public:
		Compressor();

		void setCompressor(bool value);
		bool isCompressor() const;

	private:
		bool m_active;
	};

	class Equalizer : public SoundEffect {
	public:
		Equalizer();

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
		float m_lowGain;
		float m_lowCutoff;
		float m_mid1Gain;
		float m_mid1Center;
		float m_mid1Width;
		float m_mid2Gain;
		float m_mid2Center;
		float m_mid2Width;
		float m_highGain;
		float m_highCutoff;
	};


	class EaxReverb : public SoundEffect {
	public:
		EaxReverb();

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

		void loadPreset(const EFXEAXREVERBPROPERTIES& prop);
	private:
		float m_density;
		float m_diffusion;
		float m_gain;
		float m_gainHf;
		float m_gainLf;
		float m_decayTime;
		float m_decayHfRatio;
		float m_decayLfRatio;
		float m_reflectionsGain;
		float m_reflectionsDelay;
		AudioSpaceCoordinate m_reflectionsPan;
		float m_lateReverbGain;
		float m_lateReverbDelay;
		AudioSpaceCoordinate m_lateReverbPan;
		float m_echoTime;
		float m_echoDepth;
		float m_modulationTime;
		float m_modulationDepth;
		float m_airAbsorptionGainHf;
		float m_hfReference;
		float m_lfReference;
		float m_roomRolloffFactor;
		bool m_decayHfLimit;
	};
}
#endif
