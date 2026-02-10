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

// Standard C++ library includes
#include <algorithm>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"
#include "util/base/exception.h"

#include "soundeffect.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_AUDIO);


	SoundEffect::SoundEffect() :
		m_effect(0),
		m_slot(0),
		m_effectType(SE_EFFECT_NULL),
		m_enabled(false),
		m_filter(NULL) {

		alGenEffects(1, &m_effect);
	}

	SoundEffect::~SoundEffect() {
		alDeleteEffects(1, &m_effect);
	}

	ALuint SoundEffect::getEffectId() const {
		return m_effect;
	}

	void SoundEffect::setSlotId(ALuint slot) {
		m_slot = slot;
	}

	ALuint SoundEffect::getSlotId() {
		return m_slot;
	}

	SoundEffectType SoundEffect::getEffectType() const {
		return m_effectType;
	}

	void SoundEffect::setEnabled(bool enabled) {
		m_enabled = enabled;
	}

	bool SoundEffect::isEnabled() const {
		return m_enabled;
	}

	void SoundEffect::setFilter(SoundFilter* filter) {
		m_filter = filter;
	}

	SoundFilter* SoundEffect::getFilter() {
		return m_filter;
	}


	Reverb::Reverb() :
		m_density(1.0f),
		m_diffusion(1.0f),
		m_gain(0.32f),
		m_gainHf(0.89f),
		m_decayTime(1.49f),
		m_decayHfRatio(0.83f),
		m_reflectionsGain(0.05f),
		m_reflectionsDelay(0.007f),
		m_lateReverbGain(1.26f),
		m_lateReverbDelay(0.011f),
		m_airAbsorptionGainHf(0.994f),
		m_roomRolloffFactor(0.0f),
		m_decayHfLimit(true) {

		m_effectType = SE_EFFECT_REVERB;
		alEffecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
	}

	void Reverb::setDensity(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0f);
		m_density = value;
		alEffectf(m_effect, AL_REVERB_DENSITY, m_density);
	}

	float Reverb::getDensity() const {
		return m_density;
	}

	void Reverb::setDiffusion(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0f);
		m_diffusion = value;
		alEffectf(m_effect, AL_REVERB_DIFFUSION, m_diffusion);
	}

	float Reverb::getDiffusion() const {
		return m_diffusion;
	}

	void Reverb::setGain(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0f);
		m_gain = value;
		alEffectf(m_effect, AL_REVERB_GAIN, m_gain);
	}

	float Reverb::getGain() const {
		return m_gain;
	}

	void Reverb::setGainHf(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0f);
		m_gainHf = value;
		alEffectf(m_effect, AL_REVERB_GAINHF, m_gainHf);
	}

	float Reverb::getGainHf() const {
		return m_gainHf;
	}

	void Reverb::setDecayTime(float value) {
		value = std::min(value, 20.0f);
		value = std::max(value, 0.1f);
		m_decayTime = value;
		alEffectf(m_effect, AL_REVERB_DECAY_TIME, m_decayTime);
	}

	float Reverb::getDecayTime() const {
		return m_decayTime;
	}

	void Reverb::setDecayHfRatio(float value) {
		value = std::min(value, 2.0f);
		value = std::max(value, 0.1f);
		m_decayHfRatio = value;
		alEffectf(m_effect, AL_REVERB_DECAY_HFRATIO, m_decayHfRatio);
	}

	float Reverb::getDecayHfRatio() const {
		return m_decayHfRatio;
	}

	void Reverb::setReflectionsGain(float value) {
		value = std::min(value, 3.16f);
		value = std::max(value, 0.0f);
		m_reflectionsGain = value;
		alEffectf(m_effect, AL_REVERB_REFLECTIONS_GAIN, m_reflectionsGain);
	}

	float Reverb::getReflectionsGain() const {
		return m_reflectionsGain;
	}

	void Reverb::setReflectionsDelay(float value) {
		value = std::min(value, 0.3f);
		value = std::max(value, 0.0f);
		m_reflectionsDelay = value;
		alEffectf(m_effect, AL_REVERB_REFLECTIONS_DELAY, m_reflectionsDelay);
	}

	float Reverb::getReflectionsDelay() const {
		return m_reflectionsDelay;
	}

	void Reverb::setLateReverbGain(float value) {
		value = std::min(value, 10.0f);
		value = std::max(value, 0.0f);
		m_lateReverbGain = value;
		alEffectf(m_effect, AL_REVERB_LATE_REVERB_GAIN, m_lateReverbGain);
	}

	float Reverb::getLateReverbGain() const {
		return m_lateReverbGain;
	}

	void Reverb::setLateReverbDelay(float value) {
		value = std::min(value, 0.1f);
		value = std::max(value, 0.0f);
		m_reflectionsDelay = value;
		alEffectf(m_effect, AL_REVERB_LATE_REVERB_DELAY, m_reflectionsDelay);
	}

	float Reverb::getLateReverbDelay() const {
		return m_reflectionsDelay;
	}

	void Reverb::setAirAbsorptionGainHf(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.892f);
		m_airAbsorptionGainHf = value;
		alEffectf(m_effect, AL_REVERB_AIR_ABSORPTION_GAINHF, m_airAbsorptionGainHf);
	}

	float Reverb::getAirAbsorptionGainHf() const {
		return m_airAbsorptionGainHf;
	}

	void Reverb::setRoomRolloffFactor(float value) {
		value = std::min(value, 10.0f);
		value = std::max(value, 0.0f);
		m_roomRolloffFactor = value;
		alEffectf(m_effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, m_roomRolloffFactor);
	}

	float Reverb::getRoomRolloffFactor() const {
		return m_roomRolloffFactor;
	}

	void Reverb::setDecayHfLimit(bool value) {
		m_decayHfLimit = value;
		alEffecti(m_effect, AL_REVERB_DECAY_HFLIMIT, m_decayHfLimit ? AL_TRUE : AL_FALSE);
	}

	bool Reverb::isDecayHfLimit() const {
		return m_decayHfLimit;
	}


	Chorus::Chorus() :
		m_waveformTriangle(true),
		m_phase(90),
		m_rate(1.1f),
		m_depth(0.1f),
		m_feedback(0.25f),
		m_delay(0.016f) {
		m_effectType = SE_EFFECT_CHORUS;
		alEffecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_CHORUS);
	}

	void Chorus::setWaveformTriangle(bool value) {
		m_waveformTriangle = value;
		if (m_waveformTriangle) {
			alEffecti(m_effect, AL_CHORUS_WAVEFORM, AL_CHORUS_WAVEFORM_TRIANGLE);
		} else {
			alEffecti(m_effect, AL_CHORUS_WAVEFORM, AL_CHORUS_WAVEFORM_SINUSOID);
		}
	}

	bool Chorus::isWaveformTriangle() const {
		return m_waveformTriangle;
	}

	void Chorus::setPhase(int32_t value) {
		value = std::min(value, 180);
		value = std::max(value, -180);
		m_phase = value;
		alEffecti(m_effect, AL_CHORUS_PHASE, m_phase);
	}

	int32_t Chorus::getPhase() const {
		return m_phase;
	}

	void Chorus::setRate(float value) {
		value = std::min(value, 10.0f);
		value = std::max(value, 0.0f);
		m_rate = value;
		alEffectf(m_effect, AL_CHORUS_RATE, m_rate);
	}

	float Chorus::getRate() const {
		return m_rate;
	}

	void Chorus::setDepth(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0f);
		m_depth = value;
		alEffectf(m_effect, AL_CHORUS_DEPTH, m_depth);
	}

	float Chorus::getDepth() const {
		return m_depth;
	}

	void Chorus::setFeedback(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, -1.0f);
		m_feedback = value;
		alEffectf(m_effect, AL_CHORUS_FEEDBACK, m_feedback);
	}

	float Chorus::getFeedback() const {
		return m_feedback;
	}

	void Chorus::setDelay(float value) {
		value = std::min(value, 0.016f);
		value = std::max(value, 0.0f);
		m_delay = value;
		alEffectf(m_effect, AL_CHORUS_DELAY, m_delay);
	}

	float Chorus::getDelay() const {
		return m_delay;
	}


	Distortion::Distortion() :
		m_edge(0.2f),
		m_gain(0.05f),
		m_lowpassCutoff(8000.0f),
		m_eqCenter(3600.0f),
		m_eqBandwidth(3600.0f) {
		m_effectType = SE_EFFECT_DISTORTION;
		alEffecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_DISTORTION);
	}

	void Distortion::setEdge(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0f);
		m_edge = value;
		alEffectf(m_effect, AL_DISTORTION_EDGE, m_edge);
	}

	float Distortion::getEdge() const {
		return m_edge;
	}

	void Distortion::setGain(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.01f);
		m_gain = value;
		alEffectf(m_effect, AL_DISTORTION_GAIN, m_gain);
	}

	float Distortion::getGain() const {
		return m_gain;
	}

	void Distortion::setLowpassCutoff(float value) {
		value = std::min(value, 24000.0f);
		value = std::max(value, 80.0f);
		m_lowpassCutoff = value;
		alEffectf(m_effect, AL_DISTORTION_LOWPASS_CUTOFF, m_lowpassCutoff);
	}

	float Distortion::getLowpassCutoff() const {
		return m_lowpassCutoff;
	}

	void Distortion::setEqCenter(float value) {
		value = std::min(value, 24000.0f);
		value = std::max(value, 80.0f);
		m_eqCenter = value;
		alEffectf(m_effect, AL_DISTORTION_EQCENTER, m_eqCenter);
	}

	float Distortion::getEqCenter() const {
		return m_eqCenter;
	}

	void Distortion::setEqBandwidth(float value) {
		value = std::min(value, 24000.0f);
		value = std::max(value, 80.0f);
		m_eqBandwidth = value;
		alEffectf(m_effect, AL_DISTORTION_EQBANDWIDTH, m_eqBandwidth);
	}

	float Distortion::getEqBandwidth() const {
		return m_eqBandwidth;
	}


	Echo::Echo() :
		m_delay(0.1f),
		m_lrDelay(0.1f),
		m_damping(0.5f),
		m_feedback(0.5f),
		m_spread(-1.0f) {
		m_effectType = SE_EFFECT_ECHO;
		alEffecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_ECHO);
	}

	void Echo::setDelay(float value) {
		value = std::min(value, 0.207f);
		value = std::max(value, 0.0f);
		m_delay = value;
		alEffectf(m_effect, AL_ECHO_DELAY, m_delay);
	}

	float Echo::getDelay() const {
		return m_delay;
	}

	void Echo::setLrDelay(float value) {
		value = std::min(value, 0.404f);
		value = std::max(value, 0.0f);
		m_lrDelay = value;
		alEffectf(m_effect, AL_ECHO_LRDELAY, m_lrDelay);
	}

	float Echo::getLrDelay() const {
		return m_lrDelay;
	}

	void Echo::setDamping(float value) {
		value = std::min(value, 0.99f);
		value = std::max(value, 0.0f);
		m_damping = value;
		alEffectf(m_effect, AL_ECHO_DAMPING, m_damping);
	}

	float Echo::getDamping() const {
		return m_damping;
	}

	void Echo::setFeedback(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0f);
		m_feedback = value;
		alEffectf(m_effect, AL_ECHO_FEEDBACK, m_feedback);
	}

	float Echo::getFeedback() const {
		return m_feedback;
	}

	void Echo::setSpread(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, -1.0f);
		m_spread = value;
		alEffectf(m_effect, AL_ECHO_SPREAD, m_spread);
	}

	float Echo::getSpread() const {
		return m_spread;
	}


	Flanger::Flanger() :
		m_waveformTriangle(true),
		m_phase(0),
		m_rate(0.27f),
		m_depth(1.0f),
		m_feedback(-0.5f),
		m_delay(0.002f) {
		m_effectType = SE_EFFECT_FLANGER;
		alEffecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_FLANGER);
	}

	void Flanger::setWaveformTriangle(bool value) {
		m_waveformTriangle = value;
		if (m_waveformTriangle) {
			alEffecti(m_effect, AL_FLANGER_WAVEFORM, AL_FLANGER_WAVEFORM_TRIANGLE);
		} else {
			alEffecti(m_effect, AL_FLANGER_WAVEFORM, AL_FLANGER_WAVEFORM_SINUSOID);
		}
	}

	bool Flanger::isWaveformTriangle() const {
		return m_waveformTriangle;
	}

	void Flanger::setPhase(int32_t value) {
		value = std::min(value, 180);
		value = std::max(value, -180);
		m_phase = value;
		alEffecti(m_effect, AL_FLANGER_PHASE, m_phase);
	}

	int32_t Flanger::getPhase() const {
		return m_phase;
	}

	void Flanger::setRate(float value) {
		value = std::min(value, 10.0f);
		value = std::max(value, 0.0f);
		m_rate = value;
		alEffectf(m_effect, AL_FLANGER_RATE, m_rate);
	}

	float Flanger::getRate() const {
		return m_rate;
	}

	void Flanger::setDepth(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0f);
		m_depth = value;
		alEffectf(m_effect, AL_FLANGER_DEPTH, m_depth);
	}

	float Flanger::getDepth() const {
		return m_depth;
	}

	void Flanger::setFeedback(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, -1.0f);
		m_feedback = value;
		alEffectf(m_effect, AL_FLANGER_FEEDBACK, m_feedback);
	}

	float Flanger::getFeedback() const {
		return m_feedback;
	}

	void Flanger::setDelay(float value) {
		value = std::min(value, 0.004f);
		value = std::max(value, 0.0f);
		m_delay = value;
		alEffectf(m_effect, AL_FLANGER_DELAY, m_delay);
	}

	float Flanger::getDelay() const {
		return m_delay;
	}


	FrequencyShifter::FrequencyShifter() :
		m_frequency(0.0f),
		m_leftDirection(0),
		m_rightDirection(0) {
		m_effectType = SE_EFFECT_FREQUENCY_SHIFTER;
		alEffecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_FREQUENCY_SHIFTER);
	}

	void FrequencyShifter::setFrequency(float value) {
		value = std::min(value, 24000.0f);
		value = std::max(value, 0.0f);
		m_frequency = value;
		alEffectf(m_effect, AL_FREQUENCY_SHIFTER_FREQUENCY, m_frequency);
	}

	float FrequencyShifter::getFrequency() const {
		return m_frequency;
	}

	void FrequencyShifter::setLeftDirection(uint8_t value) {
		value = std::min(value, uint8_t(2));
		value = std::max(value, uint8_t(0));
		m_leftDirection = value;
		alEffecti(m_effect, AL_FREQUENCY_SHIFTER_LEFT_DIRECTION, m_leftDirection);
	}

	uint8_t FrequencyShifter::getLeftDirection() const {
		return m_leftDirection;
	}

	void FrequencyShifter::setRightDirection(uint8_t value) {
		value = std::min(value, uint8_t(2));
		value = std::max(value, uint8_t(0));
		m_rightDirection = value;
		alEffecti(m_effect, AL_FREQUENCY_SHIFTER_RIGHT_DIRECTION, m_rightDirection);
	}

	uint8_t FrequencyShifter::getRightDirection() const {
		return m_rightDirection;
	}


	VocalMorpher::VocalMorpher() :
		m_phonemeA(0),
		m_phonemeB(10),
		m_phonemeCoarseA(0),
		m_phonemeCoarseB(0),
		m_waveform(0),
		m_rate(1.41f) {
		m_effectType = SE_EFFECT_VOCAL_MORPHER;
		alEffecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_VOCAL_MORPHER);
	}

	void VocalMorpher::setPhonemeA(uint16_t value) {
		value = std::min(value, uint16_t(29));
		value = std::max(value, uint16_t(0));
		m_phonemeA = value;
		alEffecti(m_effect, AL_VOCAL_MORPHER_PHONEMEA, m_phonemeA);
	}

	uint16_t VocalMorpher::getPhonemeA() const {
		return m_phonemeA;
	}

	void VocalMorpher::setPhonemeB(uint16_t value) {
		value = std::min(value, uint16_t(29));
		value = std::max(value, uint16_t(0));
		m_phonemeB = value;
		alEffecti(m_effect, AL_VOCAL_MORPHER_PHONEMEB, m_phonemeB);
	}

	uint16_t VocalMorpher::getPhonemeB() const {
		return m_phonemeB;
	}

	void VocalMorpher::setPhonemeCoarseA(int16_t value) {
		value = std::min(value, int16_t(24));
		value = std::max(value, int16_t(-24));
		m_phonemeCoarseA = value;
		alEffecti(m_effect, AL_VOCAL_MORPHER_PHONEMEA_COARSE_TUNING, m_phonemeCoarseA);
	}

	int16_t VocalMorpher::getPhonemeCoarseA() const {
		return m_phonemeCoarseA;
	}

	void VocalMorpher::setPhonemeCoarseB(int16_t value) {
		value = std::min(value, int16_t(24));
		value = std::max(value, int16_t(-24));
		m_phonemeCoarseB = value;
		alEffecti(m_effect, AL_VOCAL_MORPHER_PHONEMEB_COARSE_TUNING, m_phonemeCoarseB);
	}

	int16_t VocalMorpher::getPhonemeCoarseB() const {
		return m_phonemeCoarseB;
	}

	void VocalMorpher::setWaveform(uint8_t value) {
		value = std::min(value, uint8_t(2));
		value = std::max(value, uint8_t(0));
		m_waveform = value;
		alEffecti(m_effect, AL_VOCAL_MORPHER_WAVEFORM, m_waveform);
	}

	uint8_t VocalMorpher::getWaveform() const {
		return m_waveform;
	}

	void VocalMorpher::setRate(float value) {
		value = std::min(value, 10.0f);
		value = std::max(value, 0.0f);
		m_rate = value;
		alEffectf(m_effect, AL_VOCAL_MORPHER_RATE, m_rate);
	}

	float VocalMorpher::getRate() const {
		return m_rate;
	}


	PitchShifter::PitchShifter() :
		m_coarseTune(12),
		m_fineTune(0) {
		m_effectType = SE_EFFECT_PITCH_SHIFTER;
		alEffecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_PITCH_SHIFTER);
	}

	void PitchShifter::setCoarseTune(int16_t value) {
		value = std::min(value, int16_t(12));
		value = std::max(value, int16_t(-12));
		m_coarseTune = value;
		alEffecti(m_effect, AL_PITCH_SHIFTER_COARSE_TUNE, m_coarseTune);
	}

	int16_t PitchShifter::getCoarseTune() const {
		return m_coarseTune;
	}

	void PitchShifter::setFineTune(int16_t value) {
		value = std::min(value, int16_t(50));
		value = std::max(value, int16_t(-50));
		m_fineTune = value;
		alEffecti(m_effect, AL_PITCH_SHIFTER_FINE_TUNE, m_fineTune);
	}

	int16_t PitchShifter::getFineTune() const {
		return m_fineTune;
	}


	RingModulator::RingModulator() :
		m_frequency(440.0f),
		m_highpassCutoff(800.0f),
		m_waveform(0) {
		m_effectType = SE_EFFECT_RING_MODULATOR;
		alEffecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_RING_MODULATOR);
	}

	void RingModulator::setFrequency(float value) {
		value = std::min(value, 8000.0f);
		value = std::max(value, 0.0f);
		m_frequency = value;
		alEffectf(m_effect, AL_RING_MODULATOR_FREQUENCY, m_frequency);
	}

	float RingModulator::getFrequency() const {
		return m_frequency;
	}

	void RingModulator::setHighpassCutoff(float value) {
		value = std::min(value, 24000.0f);
		value = std::max(value, 0.0f);
		m_highpassCutoff = value;
		alEffectf(m_effect, AL_RING_MODULATOR_HIGHPASS_CUTOFF, m_highpassCutoff);
	}

	float RingModulator::getHighpassCutoff() const {
		return m_highpassCutoff;
	}

	void RingModulator::setWaveform(uint8_t value) {
		value = std::min(value, uint8_t(2));
		value = std::max(value, uint8_t(0));
		m_waveform = value;
		alEffecti(m_effect, AL_RING_MODULATOR_WAVEFORM, m_waveform);
	}

	uint8_t RingModulator::getWaveform() const {
		return m_waveform;
	}


	Autowah::Autowah() :
		m_attackTime(0.06f),
		m_releaseTime(0.06f),
		m_resonance(1000.0f),
		m_peakGain(11.22f) {
		m_effectType = SE_EFFECT_AUTOWAH;
		alEffecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_AUTOWAH);
	}

	void Autowah::setAttackTime(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0001f);
		m_attackTime = value;
		alEffectf(m_effect, AL_AUTOWAH_ATTACK_TIME, m_attackTime);
	}

	float Autowah::getAttackTime() const {
		return m_attackTime;
	}

	void Autowah::setReleaseTime(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0001f);
		m_releaseTime = value;
		alEffectf(m_effect, AL_AUTOWAH_RELEASE_TIME, m_releaseTime);
	}

	float Autowah::getReleaseTime() const {
		return m_releaseTime;
	}

	void Autowah::setResonance(float value) {
		value = std::min(value, 1000.0f);
		value = std::max(value, 2.0f);
		m_resonance = value;
		alEffectf(m_effect, AL_AUTOWAH_RESONANCE, m_resonance);
	}

	float Autowah::getResonance() const {
		return m_resonance;
	}

	void Autowah::setPeakGain(float value) {
		value = std::min(value, 31621.0f);
		value = std::max(value, 0.00003f);
		m_peakGain = value;
		alEffectf(m_effect, AL_AUTOWAH_PEAK_GAIN, m_peakGain);
	}

	float Autowah::getPeakGain() const {
		return m_peakGain;
	}


	Compressor::Compressor() :
		m_active(true) {
		m_effectType = SE_EFFECT_COMPRESSOR;
		alEffecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_COMPRESSOR);
	}

	void Compressor::setCompressor(bool value) {
		m_active = value;
		alEffecti(m_effect, AL_COMPRESSOR_ONOFF, m_active ? AL_TRUE : AL_FALSE);
	}

	bool Compressor::isCompressor() const {
		return m_active;
	}


	Equalizer::Equalizer() :
		m_lowGain(1.0f),
		m_lowCutoff(200.0f),
		m_mid1Gain(1.0f),
		m_mid1Center(500.0f),
		m_mid1Width(1.0f),
		m_mid2Gain(1.0f),
		m_mid2Center(3000.0f),
		m_mid2Width(1.0f),
		m_highGain(1.0f),
		m_highCutoff(6000.0f) {
		m_effectType = SE_EFFECT_EQUALIZER;
		alEffecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);
	}

	void Equalizer::setLowGain(float value) {
		value = std::min(value, 7.943f);
		value = std::max(value, 0.126f);
		m_lowGain = value;
		alEffectf(m_effect, AL_EQUALIZER_LOW_GAIN, m_lowGain);
	}

	float Equalizer::getLowGain() const {
		return m_lowGain;
	}

	void Equalizer::setLowCutoff(float value) {
		value = std::min(value, 800.0f);
		value = std::max(value, 50.0f);
		m_lowCutoff = value;
		alEffectf(m_effect, AL_EQUALIZER_LOW_CUTOFF, m_lowCutoff);
	}

	float Equalizer::getLowCutoff() const {
		return m_lowCutoff;
	}

	void Equalizer::setMid1Gain(float value) {
		value = std::min(value, 7.943f);
		value = std::max(value, 0.126f);
		m_mid1Gain = value;
		alEffectf(m_effect, AL_EQUALIZER_MID1_GAIN, m_mid1Gain);
	}

	float Equalizer::getMid1Gain() const {
		return m_mid1Gain;
	}

	void Equalizer::setMid1Center(float value) {
		value = std::min(value, 3000.0f);
		value = std::max(value, 200.0f);
		m_mid1Center = value;
		alEffectf(m_effect, AL_EQUALIZER_MID1_CENTER, m_mid1Center);
	}

	float Equalizer::getMid1Center() const {
		return m_mid1Center;
	}

	void Equalizer::setMid1Width(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.01f);
		m_mid1Width = value;
		alEffectf(m_effect, AL_EQUALIZER_MID1_WIDTH, m_mid1Width);
	}

	float Equalizer::getMid1Width() const {
		return m_mid1Width;
	}

	void Equalizer::setMid2Gain(float value) {
		value = std::min(value, 7.943f);
		value = std::max(value, 0.126f);
		m_mid2Gain = value;
		alEffectf(m_effect, AL_EQUALIZER_MID2_GAIN, m_mid2Gain);
	}

	float Equalizer::getMid2Gain() const {
		return m_mid2Gain;
	}

	void Equalizer::setMid2Center(float value) {
		value = std::min(value, 8000.0f);
		value = std::max(value, 1000.0f);
		m_mid2Center = value;
		alEffectf(m_effect, AL_EQUALIZER_MID2_CENTER, m_mid2Center);
	}

	float Equalizer::getMid2Center() const {
		return m_mid2Center;
	}

	void Equalizer::setMid2Width(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.01f);
		m_mid2Width = value;
		alEffectf(m_effect, AL_EQUALIZER_MID2_WIDTH, m_mid2Width);
	}

	float Equalizer::getMid2Width() const {
		return m_mid2Width;
	}

	void Equalizer::setHighGain(float value) {
		value = std::min(value, 7.943f);
		value = std::max(value, 0.126f);
		m_highGain = value;
		alEffectf(m_effect, AL_EQUALIZER_HIGH_GAIN, m_highGain);
	}

	float Equalizer::getHighGain() const {
		return m_highGain;
	}

	void Equalizer::setHighCutoff(float value) {
		value = std::min(value, 16000.0f);
		value = std::max(value, 4000.0f);
		m_highCutoff = value;
		alEffectf(m_effect, AL_EQUALIZER_HIGH_CUTOFF, m_highCutoff);
	}

	float Equalizer::getHighCutoff() const {
		return m_highCutoff;
	}


	EaxReverb::EaxReverb() :
		m_density(1.0f),
		m_diffusion(1.0f),
		m_gain(0.32f),
		m_gainHf(0.89f),
		m_gainLf(0.0f),
		m_decayTime(1.49f),
		m_decayHfRatio(0.83f),
		m_decayLfRatio(1.0f),
		m_reflectionsGain(0.05f),
		m_reflectionsDelay(0.007f),
		m_reflectionsPan(AudioSpaceCoordinate(0.0, 0.0, 0.0)),
		m_lateReverbGain(1.26f),
		m_lateReverbDelay(0.011f),
		m_lateReverbPan(AudioSpaceCoordinate(0.0, 0.0, 0.0)),
		m_echoTime(0.25f),
		m_echoDepth(0.0f),
		m_modulationTime(0.25f),
		m_modulationDepth(0.0f),
		m_airAbsorptionGainHf(0.994f),
		m_hfReference(5000.0f),
		m_lfReference(250.0f),
		m_roomRolloffFactor(0.0f),
		m_decayHfLimit(true) {

		m_effectType = SE_EFFECT_EAXREVERB;
		alEffecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
	}

	void EaxReverb::setDensity(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0f);
		m_density = value;
		alEffectf(m_effect, AL_EAXREVERB_DENSITY, m_density);
	}

	float EaxReverb::getDensity() const {
		return m_density;
	}

	void EaxReverb::setDiffusion(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0f);
		m_diffusion = value;
		alEffectf(m_effect, AL_EAXREVERB_DIFFUSION, m_diffusion);
	}

	float EaxReverb::getDiffusion() const {
		return m_diffusion;
	}

	void EaxReverb::setGain(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0f);
		m_gain = value;
		alEffectf(m_effect, AL_EAXREVERB_GAIN, m_gain);
	}

	float EaxReverb::getGain() const {
		return m_gain;
	}

	void EaxReverb::setGainHf(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0f);
		m_gainHf = value;
		alEffectf(m_effect, AL_EAXREVERB_GAINHF, m_gainHf);
	}

	float EaxReverb::getGainHf() const {
		return m_gainHf;
	}

	void EaxReverb::setGainLf(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0f);
		m_gainLf = value;
		alEffectf(m_effect, AL_EAXREVERB_GAINLF, m_gainLf);
	}

	float EaxReverb::getGainLf() const {
		return m_gainLf;
	}

	void EaxReverb::setDecayTime(float value) {
		value = std::min(value, 20.0f);
		value = std::max(value, 0.1f);
		m_decayTime = value;
		alEffectf(m_effect, AL_EAXREVERB_DECAY_TIME, m_decayTime);
	}

	float EaxReverb::getDecayTime() const {
		return m_decayTime;
	}

	void EaxReverb::setDecayHfRatio(float value) {
		value = std::min(value, 2.0f);
		value = std::max(value, 0.1f);
		m_decayHfRatio = value;
		alEffectf(m_effect, AL_EAXREVERB_DECAY_HFRATIO, m_decayHfRatio);
	}

	float EaxReverb::getDecayHfRatio() const {
		return m_decayHfRatio;
	}

	void EaxReverb::setDecayLfRatio(float value) {
		value = std::min(value, 2.0f);
		value = std::max(value, 0.1f);
		m_decayLfRatio = value;
		alEffectf(m_effect, AL_EAXREVERB_DECAY_LFRATIO, m_decayLfRatio);
	}

	float EaxReverb::getDecayLfRatio() const {
		return m_decayLfRatio;
	}

	void EaxReverb::setReflectionsGain(float value) {
		value = std::min(value, 3.16f);
		value = std::max(value, 0.0f);
		m_reflectionsGain = value;
		alEffectf(m_effect, AL_EAXREVERB_REFLECTIONS_GAIN, m_reflectionsGain);
	}

	float EaxReverb::getReflectionsGain() const {
		return m_reflectionsGain;
	}

	void EaxReverb::setReflectionsDelay(float value) {
		value = std::min(value, 0.3f);
		value = std::max(value, 0.0f);
		m_reflectionsDelay = value;
		alEffectf(m_effect, AL_EAXREVERB_REFLECTIONS_DELAY, m_reflectionsDelay);
	}

	float EaxReverb::getReflectionsDelay() const {
		return m_reflectionsDelay;
	}

	void EaxReverb::setReflectionsPan(const AudioSpaceCoordinate& coordinate) {
		m_reflectionsPan.x = std::min(coordinate.x, double(1.0));
		m_reflectionsPan.x = std::max(coordinate.x, double(-1.0));
		m_reflectionsPan.y = std::min(coordinate.y, double(1.0));
		m_reflectionsPan.y = std::max(coordinate.y, double(-1.0));
		m_reflectionsPan.z = std::min(coordinate.z, double(1.0));
		m_reflectionsPan.z = std::max(coordinate.z, double(-1.0));

		ALfloat vec[3] = { static_cast<ALfloat>(m_reflectionsPan.x),
			static_cast<ALfloat>(m_reflectionsPan.y), static_cast<ALfloat>(m_reflectionsPan.z)};
		alEffectfv(m_effect, AL_EAXREVERB_REFLECTIONS_PAN, vec);
	}
	
	AudioSpaceCoordinate EaxReverb::getReflectionsPan() const {
		return m_reflectionsPan;
	}

	void EaxReverb::setLateReverbGain(float value) {
		value = std::min(value, 10.0f);
		value = std::max(value, 0.0f);
		m_lateReverbGain = value;
		alEffectf(m_effect, AL_EAXREVERB_LATE_REVERB_GAIN, m_lateReverbGain);
	}

	float EaxReverb::getLateReverbGain() const {
		return m_lateReverbGain;
	}

	void EaxReverb::setLateReverbDelay(float value) {
		value = std::min(value, 0.1f);
		value = std::max(value, 0.0f);
		m_reflectionsDelay = value;
		alEffectf(m_effect, AL_EAXREVERB_LATE_REVERB_DELAY, m_reflectionsDelay);
	}

	float EaxReverb::getLateReverbDelay() const {
		return m_reflectionsDelay;
	}

	void EaxReverb::setLateReverbPan(const AudioSpaceCoordinate& coordinate) {
		m_lateReverbPan.x = std::min(coordinate.x, double(1.0));
		m_lateReverbPan.x = std::max(coordinate.x, double(-1.0));
		m_lateReverbPan.y = std::min(coordinate.y, double(1.0));
		m_lateReverbPan.y = std::max(coordinate.y, double(-1.0));
		m_lateReverbPan.z = std::min(coordinate.z, double(1.0));
		m_lateReverbPan.z = std::max(coordinate.z, double(-1.0));

		ALfloat vec[3] = { static_cast<ALfloat>(m_lateReverbPan.x),
			static_cast<ALfloat>(m_lateReverbPan.y), static_cast<ALfloat>(m_lateReverbPan.z) };
		alEffectfv(m_effect, AL_EAXREVERB_LATE_REVERB_PAN, vec);
	}

	AudioSpaceCoordinate EaxReverb::getLateReverbPan() const {
		return m_lateReverbPan;
	}

	void EaxReverb::setEchoTime(float value) {
		value = std::min(value, 0.25f);
		value = std::max(value, 0.075f);
		m_echoTime = value;
		alEffectf(m_effect, AL_EAXREVERB_ECHO_TIME, m_echoTime);
	}

	float EaxReverb::getEchoTime() const {
		return m_echoTime;
	}

	void EaxReverb::setEchoDepth(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0f);
		m_echoDepth = value;
		alEffectf(m_effect, AL_EAXREVERB_ECHO_DEPTH, m_echoDepth);
	}

	float EaxReverb::getEchoDepth() const {
		return m_echoDepth;
	}

	void EaxReverb::setModulationTime(float value) {
		value = std::min(value, 4.0f);
		value = std::max(value, 0.04f);
		m_modulationTime = value;
		alEffectf(m_effect, AL_EAXREVERB_MODULATION_TIME, m_modulationTime);
	}

	float EaxReverb::getModulationTime() const {
		return m_modulationTime;
	}

	void EaxReverb::setModulationDepth(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.0f);
		m_modulationDepth = value;
		alEffectf(m_effect, AL_EAXREVERB_MODULATION_DEPTH, m_modulationDepth);
	}

	float EaxReverb::getModulationDepth() const {
		return m_modulationDepth;
	}

	void EaxReverb::setAirAbsorptionGainHf(float value) {
		value = std::min(value, 1.0f);
		value = std::max(value, 0.892f);
		m_airAbsorptionGainHf = value;
		alEffectf(m_effect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, m_airAbsorptionGainHf);
	}

	float EaxReverb::getAirAbsorptionGainHf() const {
		return m_airAbsorptionGainHf;
	}

	void EaxReverb::setHfReference(float value) {
		value = std::min(value, 20000.0f);
		value = std::max(value, 1000.0f);
		m_hfReference = value;
		alEffectf(m_effect, AL_EAXREVERB_HFREFERENCE, m_hfReference);
	}

	float EaxReverb::getHfReference() const {
		return m_hfReference;
	}

	void EaxReverb::setLfReference(float value) {
		value = std::min(value, 1000.0f);
		value = std::max(value, 20.0f);
		m_lfReference = value;
		alEffectf(m_effect, AL_EAXREVERB_LFREFERENCE, m_lfReference);
	}

	float EaxReverb::getLfReference() const {
		return m_lfReference;
	}

	void EaxReverb::setRoomRolloffFactor(float value) {
		value = std::min(value, 10.0f);
		value = std::max(value, 0.0f);
		m_roomRolloffFactor = value;
		alEffectf(m_effect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, m_roomRolloffFactor);
	}

	float EaxReverb::getRoomRolloffFactor() const {
		return m_roomRolloffFactor;
	}

	void EaxReverb::setDecayHfLimit(bool value) {
		m_decayHfLimit = value;
		alEffecti(m_effect, AL_EAXREVERB_DECAY_HFLIMIT, m_decayHfLimit ? AL_TRUE : AL_FALSE);
	}

	bool EaxReverb::isDecayHfLimit() const {
		return m_decayHfLimit;
	}

	void EaxReverb::loadPreset(const EFXEAXREVERBPROPERTIES& prop) {
		// reflections and reverb pans are always 0 vectors, skip it
		setDensity(prop.flDensity);
		setDiffusion(prop.flDiffusion);
		setGain(prop.flGain);
		setGainHf(prop.flGainHF);
		setGainLf(prop.flGainLF);
		setDecayTime(prop.flDecayTime);
		setDecayHfRatio(prop.flDecayHFRatio);
		setDecayLfRatio(prop.flDecayLFRatio);
		setReflectionsGain(prop.flReflectionsGain);
		setReflectionsDelay(prop.flReflectionsDelay);
		setLateReverbGain(prop.flLateReverbGain);
		setLateReverbDelay(prop.flLateReverbDelay);
		setEchoTime(prop.flEchoTime);
		setEchoDepth(prop.flEchoDepth);
		setModulationTime(prop.flModulationTime);
		setModulationDepth(prop.flModulationDepth);
		setAirAbsorptionGainHf(prop.flAirAbsorptionGainHF);
		setHfReference(prop.flHFReference);
		setLfReference(prop.flLFReference);
		setRoomRolloffFactor(prop.flRoomRolloffFactor);
		setDecayHfLimit(prop.iDecayHFLimit ? true : false);
	}

} //FIFE
