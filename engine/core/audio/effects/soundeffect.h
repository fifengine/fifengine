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

	/** Base class for Efx sound effects.
	 */
	class SoundEffect {
	public:
		/** Constructor
		 */
		SoundEffect();

		/** Destructor
		 */
		virtual ~SoundEffect();

		/** Return the OpenAL effect handle.
		 */
		ALuint getEffectId() const;

		/** Sets the OpenAL auxiliary slot handle.
		 */
		void setSlotId(ALuint slot);

		/** Return the OpenAL auxiliary slot handle.
		 */
		ALuint getSlotId();

		/** Return sound effect type.
		 * @see SoundEffectType
		 */
		SoundEffectType getEffectType() const;

		/** Enables or disables the effect.
		 * @param enabled A bool to indicate if the effect is enabled or disabled.
		 */
		void setEnabled(bool enabled);

		/** Return true if the effect is enabled, false otherwise.
		 */
		bool isEnabled() const;

		/** Sets the additional sound filter.
		 * @param filter The SoundFilter or NULL.
		 */
		void setFilter(SoundFilter* filter);

		/** Return sound filter or NULL.
		 */
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

	/**  The environmental reverberation effect.
	 */
	class Reverb : public SoundEffect {
	public:
		/** Constructor
		 */
		Reverb();

		/** Sets density, controls the coloration of the late reverb.
		 * Lowering the value adds more coloration.
		 * @param value Range 0.0 to 1.0, default 1.0.
		 */
		void setDensity(float value);

		/** Return density value.
		 */
		float getDensity() const;

		/** Sets diffusion, controls the echo density in the reverberation decay. 
		 * @param value Range 0.0 to 1.0, default 1.0.
		 */
		void setDiffusion(float value);

		/** Return diffusion value.
		 */
		float getDiffusion() const;

		/** Sets gain, is the master volume control for the reflected sound.
		 * @param value Range 0.0 to 1.0, default 0.32.
		 */
		void setGain(float value);

		/** Return gain value.
		 */
		float getGain() const;

		/** Sets gain hf, tweaks reflected sound by attenuating it at high frequencies.
		 * @param value Range 0.0 to 1.0, default 0.89.
		 */
		void setGainHf(float value);

		/** Return hf gain.
		 */
		float getGainHf() const;

		/** The reverberation decay time.
		 * @param value Range 0.0 to 20.0, default 1.49 seconds.
		 */
		void setDecayTime(float value);
		
		/** Return decay time.
		 */
		float getDecayTime() const;

		/** Sets decay hf ratio, the spectral quality of the Decay Time parameter. It
		 * is the ratio of high-frequency decay time relative to the time set by Decay Time.
		 * @param value Range 0.1 to 2.0, default 0.83.
		 */
		void setDecayHfRatio(float value);

		/** Return decay hf ratio.
		 */
		float getDecayHfRatio() const;

		/** Sets reflections gain, the overall amount of initial reflections
		 * relative to the Gain property.
		 * @param value Range 0.0 to 3.16, default 0.05.
		 */
		void setReflectionsGain(float value);

		/** Return reflections gain.
		 */
		float getReflectionsGain() const;

		/** Sets the reflections delay, the amount of delay between the arrival time of
		 * the direct path from the source to the first reflection from the source.
		 * @param value Range 0.0 to 0.3, default 0.007.
		 */
		void setReflectionsDelay(float value);

		/** Return reflections delay.
		 */
		float getReflectionsDelay() const;

		/** Sets late reverb gain, the overall amount of later reverberation relative to gain.
		 * @param value Range 0.0 to 10.0, default 1.26.
		 */
		void setLateReverbGain(float value);

		/** Return late reverb gain.
		 */
		float getLateReverbGain() const;

		/** Sets late reverb delay, the begin time of the late reverberation relative
		 * to the time of the initial reflection.
		 * @param value Range 0.0 to 0.1, default 0.011 seconds.
		 */
		void setLateReverbDelay(float value);

		/** Return late reverb delay.
		 */
		float getLateReverbDelay() const;

		/** Sets air absorption gain hf, the distance-dependent attenuation at
		 * high frequencies caused by the propagation medium.
		 * @param value Range 0.892 to 1.0, default 0.994.
		 */
		void setAirAbsorptionGainHf(float value);

		/** Return air absorption gain hf.
		 */
		float getAirAbsorptionGainHf() const;

		/** Sets room rolloff factor, similar to rolloff factor it attenuate the reflected sound
		 * (containing both reflections and reverberation) according to source-listener distance.
		 * @param value Range 0.0 to 10.0, default 0.0.
		 */
		void setRoomRolloffFactor(float value);

		/** Return room rolloff factor.
		 */
		float getRoomRolloffFactor() const;

		/** If decay hf limit is enabled, the high-frequency decay time automatically stays
		 * below a limit value that’s derived from the setting of Air Absorption HF.
		 * @param value True or false. Default is true.
		 */
		void setDecayHfLimit(bool value);

		/** Return if decay hf limit is enabled.
		 */
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

	/** The chorus effect essentially replays the input audio accompanied by another
	 * slightly delayed version of the signal, creating a ‘doubling’ effect.
	 */
	class Chorus : public SoundEffect {
	public:
		/** Constructor
		 */
		Chorus();

		/** Sets the waveform to triangle or sinus. Controls delay time of delayed signals.
		 * @param value True for triangle, false for sinus. Default is true.
		 */
		void setWaveformTriangle(bool value);

		/** Return if wavefrom is triangle.
		 */
		bool isWaveformTriangle() const;

		/** Sets the phase, difference between the left and right LFO’s.
		 * @param value Range -180 to 180, default 90.
		 */
		void setPhase(int32_t value);

		/** Return the phase.
		 */
		int32_t getPhase() const;

		/** Sets the rate, the modulation rate of the LFO that controls
		 * the delay time of the delayed signals.
		 * @param value Range 0.0 to 10.0, default 1.1.
		 */
		void setRate(float value);

		/** Return the rate.
		 */
		float getRate() const;

		/** Sets the depth, the amount by which the delay time is modulated by the LFO.
		 * @param value Range 0.0 to 1.0, default 0.1.
		 */
		void setDepth(float value);

		/** Return the depth.
		 */
		float getDepth() const;

		/** Sets the feedback, the amount of processed signal that is
		 * fed back to the input of the chorus effect.
		 * @param value Range -1.0 to 1.0, default 0.25.
		 */
		void setFeedback(float value);

		/** Return feedback.
		 */
		float getFeedback() const;

		/** Sets the delay, the average amount of time the sample is delayed before
		 * it is played back.
		 * @param value Range 0.0 to 0.016, default 0.016.
		 */
		void setDelay(float value);

		/** Return delay.
		 */
		float getDelay() const;

	private:
		bool m_waveformTriangle;
		int32_t m_phase;
		float m_rate;
		float m_depth;
		float m_feedback;
		float m_delay;
	};

	/** The distortion effect simulates turning up (overdriving) the gain stage
	 * on a guitar amplifier or adding a distortion pedal to an instrument’s output.
	 */
	class Distortion : public SoundEffect {
	public:
		/** Constructor
		 */
		Distortion();

		/** Sets edge, the shape of the distortion.
		 * @param value Range 0.0 to 1.0, default 0.2.
		 */
		void setEdge(float value);

		/** Return edge.
		 */
		float getEdge() const;

		/** Sets gain, to attenuate the distorted sound.
		 * @param value Range 0.01 to 1.0, default 0.05.
		 */
		void setGain(float value);

		/** Return gain.
		 */
		float getGain() const;

		/** Sets lowpass cutoff, to limit the amount of high frequency signal
		 * feeding into the distortion effect.
		 * @param value Range 80.0 to 24000.0, default 8000.0.
		 */
		void setLowpassCutoff(float value);

		/** Return lowpass cutoff.
		 */
		float getLowpassCutoff() const;

		/** Sets EQ center, the frequency at which the post-distortion
		 * attenuation (Distortion Gain) is active.
		 * @param value Range 80.0 to 24000.0, default 3600.0.
		 */
		void setEqCenter(float value);

		/** Return EQ center.
		 */
		float getEqCenter() const;

		/** Sets EQ bandwidth, the bandwidth of the post-distortion attenuation.
		 * @param value Range 80.0 to 24000.0, default 3600.0.
		 */
		void setEqBandwidth(float value);

		/** Return EQ bandwidth.
		 */
		float getEqBandwidth() const;

	private:
		float m_edge;
		float m_gain;
		float m_lowpassCutoff;
		float m_eqCenter;
		float m_eqBandwidth;
	};

	/** The echo effect generates discrete, delayed instances of the input signal.
	 * The amount of delay and feedback is controllable.
	 */
	class Echo : public SoundEffect {
	public:
		/** Constructor
		 */
		Echo();

		/** Sets the delay between the original sound and the first ‘tap’, or echo instance.
		 * @param value Range 0.0 to 0.207, default 0.1.
		 */
		void setDelay(float value);

		/** Return delay.
		 */
		float getDelay() const;

		/** Sets the delay between the first ‘tap’ and the second ‘tap’.
		 * @param value Range 0.0 to 0.404, default 0.1.
		 */
		void setLrDelay(float value);

		/** Return LR delay.
		 */
		float getLrDelay() const;

		/** Sets damping, the amount of high frequency damping applied to each echo.
		 * @param value Range 0.0 to 0.99, default 0.5.
		 */
		void setDamping(float value);

		/** Return damping.
		 */
		float getDamping() const;

		/** Sets feedback, the amount of feedback the output signal fed back into the input.
		 * @param value Range 0.0 to 1.0, default 0.5.
		 */
		void setFeedback(float value);

		/** Return feedback.
		 */
		float getFeedback() const;

		/** Sets spread, that defines how hard panned the individual echoes are.
		 * @param value Range -1.0 to 1.0, default -1.0.
		 */
		void setSpread(float value);

		/** Return spread.
		 */
		float getSpread() const;

	private:
		float m_delay;
		float m_lrDelay;
		float m_damping;
		float m_feedback;
		float m_spread;
	};

	/** The flanger effect creates a “tearing” or “whooshing” sound.
	 */
	class Flanger : public SoundEffect {
	public:
		/** Constructor
		 */
		Flanger();

		/** Sets waveform to triangle or sinus. Controls the amount of the
		 * delay of the sampled signal.
		 * @param value If true triangle is used otherwise sinus, default is true.
		 */
		void setWaveformTriangle(bool value);

		/** Return if waveform is triangle or sinus.
		 */
		bool isWaveformTriangle() const;

		/** Sets phase, difference between the left and right LFO’s.
		 * At zero degrees the two LFOs are synchronized.
		 * @param value Range -180 to 180, default 0.
		 */
		void setPhase(int32_t value);

		/** Return phase.
		 */
		int32_t getPhase() const;

		/** Sets rate, the number of times per second the LFO
		 * controlling the amount of delay repeats.
		 * @param value Range 0.0 to 10.0, default 0.27.
		 */
		void setRate(float value);

		/** Return rate.
		 */
		float getRate() const;

		/** Sets depth, the ratio by which the delay time is modulated by the LFO.
		 * @param value Range 0.0 to 1.0, default 1.0.
		 */
		void setDepth(float value);

		/** Return depth.
		 */
		float getDepth() const;

		/** Sets feedback, the amount of the output signal level fed
		 * back into the effect’s input.
		 * @param value Range -1.0 to 1.0, default -0.5.
		 */
		void setFeedback(float value);

		/** Return feedback.
		 */
		float getFeedback() const;

		/** Sets delay, the average amount of time the sample is delayed
		 * before it is played back.
		 * @param value Range 0.0 to 0.004, default 0.002.
		 */
		void setDelay(float value);

		/** Return delay.
		 */
		float getDelay() const;

	private:
		bool m_waveformTriangle;
		int32_t m_phase;
		float m_rate;
		float m_depth;
		float m_feedback;
		float m_delay;
	};

	/** The frequency shifter is a single-sideband modulator, which translates all
	 * the component frequencies of the input signal by an equal amount.
	 */
	class FrequencyShifter : public SoundEffect {
	public:
		/** Constructor
		 */
		FrequencyShifter();

		/** Sets the carrier frequency. For carrier frequencies below the audible range,
		 * the single-sideband modulator may produce phaser, spatial effects.
		 * @param value Range 0.0 to 24000.0, default 0.0.
		 */
		void setFrequency(float value);

		/** Return carrier frequency.
		 */
		float getFrequency() const;

		/** Sets left direction, selects which internal signals are
		 * added together to produce the output.
		 * @param value Range 0 to 2. 0=down, 1=up, 2=off, default 0.
		 */
		void setLeftDirection(uint8_t value);

		/** Return left direction.
		 */
		uint8_t getLeftDirection() const;

		/** Sets right direction, selects which internal signals are
		* added together to produce the output.
		* @param value Range 0 to 2. 0=down, 1=up, 2=off, default 0.
		*/
		void setRightDirection(uint8_t value);

		/** Return left direction.
		 */
		uint8_t getRightDirection() const;

	private:
		float m_frequency;
		uint8_t m_leftDirection;
		uint8_t m_rightDirection;
	};

	/** The vocal morpher consists of a pair of 4-band formant filters,
	 * used to impose vocal tract effects upon the input signal.
	 * Phoneme values:
	 * 0=“A”, 1=“E”, 2="I", 3="O", 4="U", 5=“AA”, 6=“AE”, 7=“AH”, 8=“AO”, 9=“EH”,
	 * 10=“ER”, 11=“IH”, 12=“IY”, 13=“UH”, 14=“UW”, 15=“B”, 16=“D”, 17=“F”, 18=“G”,
	 * 19=“J”, 20=“K”, 21=“L”, 22=“M”, 23=“N”, 24=“P”, 25=“R”, 26=“S”, 27=“T”, 28=“V”, 29=“Z”                     
	 */
	class VocalMorpher : public SoundEffect {
	public:
		/** Constructor
		 */
		VocalMorpher();

		/** Sets phoneme A.
		 * @param value Range 0 to 29, default 0.
		 */
		void setPhonemeA(uint16_t value);

		/** Return phoneme A.
		 */
		uint16_t getPhonemeA() const;

		/** Sets phoneme B.
		 * @param value Range 0 to 29, default 10.
		 */
		void setPhonemeB(uint16_t value);

		/** Return phoneme B.
		 */
		uint16_t getPhonemeB() const;

		/** Sets phoneme coarse A, used to adjust the pitch of phoneme
		 * filters A and B in 1-semitone increments.
		 * @param value Range -24 to 24, default 0.
		 */
		void setPhonemeCoarseA(int16_t value);

		/** Return phoneme coarse A.
		 */
		int16_t getPhonemeCoarseA() const;

		/** Sets phoneme coarse B, used to adjust the pitch of phoneme
		 * filters A and B in 1-semitone increments.
		 * @param value Range -24 to 24, default 0.
		 */
		void setPhonemeCoarseB(int16_t value);

		/** Return phoneme coarse B.
		 */
		int16_t getPhonemeCoarseB() const;

		/** Sets waveform, the shape of the low-frequency oscillator
		 * used to morph between the two phoneme filters.
		 * @param value Range 0 to 2, default 0. 0=sin, 1=tri, 2=saw.
		 */
		void setWaveform(uint8_t value);

		/** Return waveform.
		 */
		uint8_t getWaveform() const;

		/** Sets rate, the frequency of the low-frequency oscillator used
		 * to morph between the two phoneme filters.
		 * @param value Range 0.0 to 10.0, dfault 1.41.
		 */
		void setRate(float value);

		/** Return rate.
		 */
		float getRate() const;

	private:
		uint16_t m_phonemeA;
		uint16_t m_phonemeB;
		int16_t m_phonemeCoarseA;
		int16_t m_phonemeCoarseB;
		uint8_t m_waveform;
		float m_rate;
	};

	/** The pitch shifter applies time-invariant pitch shifting to the input signal,
	 * over a one octave range and controllable at a semi-tone and cent resolution.
	 */
	class PitchShifter : public SoundEffect {
	public:
		/** Constructor
		 */
		PitchShifter();

		/** Sets coarse tune, the number of semitones by which the pitch is shifted.
		 * There are 12 semitones per octave.
		 * @param value Range -12 to 12, default 12.
		 */
		void setCoarseTune(int16_t value);

		/** Return coarse tune.
		 */
		int16_t getCoarseTune() const;

		/** Sets fine tune, the number of cents between Semitones a pitch is shifted.
		 * A Cent is 1/100th of a Semitone.
		 * @param value Range -50 to 50, default 0.
		 */
		void setFineTune(int16_t value);

		/** Return fine tune.
		 */
		int16_t getFineTune() const;

	private:
		int16_t m_coarseTune;
		int16_t m_fineTune;
	};

	/** The ring modulator multiplies an input signal by a carrier signal in the
	 * time domain, resulting in tremolo or inharmonic effects.
	 */
	class RingModulator : public SoundEffect {
	public:
		/** Constructor
		 */
		RingModulator();

		/** Sets the carrier signal frequency.
		 * @param value Range 0.0 to 8000.0, default 440.0.
		 */
		void setFrequency(float value);

		/** Return carrier frequency.
		 */
		float getFrequency() const;

		/** Sets highpass cutoff at which the input signal is high-pass
		 * filtered before being ring modulated.
		 * @param value Range 0.0 to 24000.0, default 800.0.
		 */
		void setHighpassCutoff(float value);

		/** Return highpass cutoff frequency.
		 */
		float getHighpassCutoff() const;

		/** Sets waveform used as the carrier signal.
		 * @param value Range 0 to 2, default 0. 0=sin, 1=saw, 2=square.
		 */
		void setWaveform(uint8_t value);
		uint8_t getWaveform() const;

	private:
		float m_frequency;
		float m_highpassCutoff;
		uint8_t m_waveform;
	};

	/** The Auto-wah effect emulates the sound of a wah-wah pedal used
	 * with an electric guitar, or a mute on a brass instrument.
	 */
	class Autowah : public SoundEffect {
	public:
		/** Constructor
		 */
		Autowah();

		/** Sets attack time, the time the filtering effect takes to sweep
		 * from minimum to maximum center frequency when it is triggered by input signal.
		 * @param value Range 0.0001 to 1.0, default 0.06.
		 */
		void setAttackTime(float value);

		/** Return attack time.
		 */
		float getAttackTime() const;

		/** Sets relase time, the time the filtering effect takes to sweep
		 * from maximum back to base center frequency, when the input signal ends.
		 * @param value Range 0.0001 to 1.0, default 0.06.
		 */
		void setReleaseTime(float value);

		/** Return release time.
		 */
		float getReleaseTime() const;

		/** Sets resonance, the peak, some times known as emphasis or Q,
		 * of the auto-wah band-pass filter.
		 * @param value Range 2.0 to 1000.0, default 1000.0.
		 */
		void setResonance(float value);

		/** Return resonance.
		 */
		float getResonance() const;

		/** Sets peak gain, the input signal level at which the band-pass
		 * filter will be fully opened.
		 * @param value Range 0.00003 to 31621.0, default 11.22.
		 */
		void setPeakGain(float value);

		/** Return peak gain.
		 */
		float getPeakGain() const;

	private:
		float m_attackTime;
		float m_releaseTime;
		float m_resonance;
		float m_peakGain;
	};

	/** The Automatic Gain Control effect performs the same task as a studio
	 * compressor – evening out the audio dynamic range of an input sound.
	 */
	class Compressor : public SoundEffect {
	public:
		/** Constructor
		 */
		Compressor();

		/** Sets compressor on or off.
		 * @param value True or false, default is true.
		 */
		void setCompressor(bool value);

		/** Return true if compressor is on, false otherwise.
		 */
		bool isCompressor() const;

	private:
		bool m_active;
	};

	/** The Equalizer providing tonal control over four different
	 * adjustable frequency ranges. Low, Mid1, Mid2 and High.
	 */
	class Equalizer : public SoundEffect {
	public:
		/** Constructor
		 */
		Equalizer();

		/** Sets low gain, the amount of cut or boost on the low frequency range.
		 * @param value Range 0.126 to 7.943, default 1.0.
		 */
		void setLowGain(float value);

		/** Return low gain.
		 */
		float getLowGain() const;

		/** Sets low cutoff, the low frequency below which signal will be cut off.
		 * @param value Range 50.0 to 800.0, default 200.0.
		 */
		void setLowCutoff(float value);

		/** Return low cutoff.
		 */
		float getLowCutoff() const;

		/** Sets mid1 gain, cut / boost signal on the “mid1” range.
		 * @param value Range 0.126 to 7.943, default 1.0.
		 */
		void setMid1Gain(float value);

		/** Return mid1 gain.
		 */
		float getMid1Gain() const;

		/** Sets mid1 center, the center frequency for the “mid1” range.
		 * @param value Range 200.0 to 3000.0, default 500.0.
		 */
		void setMid1Center(float value);

		/** Return mid1 center frequency.
		 */
		float getMid1Center() const;

		/** Sets mid1 width, the width of the “mid1” range.
		 * @param value Range 0.01 to 1.0, default 1.0.
		 */
		void setMid1Width(float value);

		/** Return mid1 width.
		 */
		float getMid1Width() const;

		/** Sets mid2 gain, cut / boost signal on the “mid2” range.
		 * @param value Range 0.126 to 7.943, default 1.0.
		 */
		void setMid2Gain(float value);

		/** Return mid1 gain.
		 */
		float getMid2Gain() const;

		/** Sets mid2 center, the center frequency for the “mid2” range.
		 * @param value Range 1000.0 to 8000.0, default 3000.0.
		 */
		void setMid2Center(float value);

		/** Return mid2 center frequency.
		 */
		float getMid2Center() const;

		/** Sets mid2 width, the width of the “mid2” range.
		 * @param value Range 0.01 to 1.0, default 1.0.
		 */
		void setMid2Width(float value);

		/** Return mid2 width.
		 */
		float getMid2Width() const;

		/** Sets high gain, cut / boost signal on the “high” range.
		 * @param value Range 0.126 to 7.943, default 1.0.
		 */
		void setHighGain(float value);

		/** Return high gain.
		 */
		float getHighGain() const;

		/** Sets high cutoff, the high frequency above which signal will be cut off.
		 * @param value Range 4000.0 to 16000.0, default 6000.0.
		 */
		void setHighCutoff(float value);

		/** Return high cutoff frequency.
		 */
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

	/** The EAX Reverb is similar to Reverb but supports more options.
	 * There exists already over 100 presets for it. See SoundEffectPreset.
	 */
	class EaxReverb : public SoundEffect {
	public:
		/** Constructor
		 */
		EaxReverb();

		/** Sets density, controls the coloration of the late reverb.
		 * Lowering the value adds more coloration.
		 * @param value Range 0.0 to 1.0, default 1.0.
		 */
		void setDensity(float value);

		/** Return density value.
		 */
		float getDensity() const;

		/** Sets diffusion, controls the echo density in the reverberation decay.
		 * @param value Range 0.0 to 1.0, default 1.0.
		 */
		void setDiffusion(float value);

		/** Return diffusion value.
		 */
		float getDiffusion() const;

		/** Sets gain, is the master volume control for the reflected sound.
		 * @param value Range 0.0 to 1.0, default 0.32.
		 */
		void setGain(float value);

		/** Return gain value.
		 */
		float getGain() const;

		/** Sets gain hf, tweaks reflected sound by attenuating it at high frequencies.
		 * @param value Range 0.0 to 1.0, default 0.89.
		 */
		void setGainHf(float value);

		/** Return hf gain.
		 */
		float getGainHf() const;

		/** Sets gain lf, tweaks reflected sound by attenuating it at low frequencies.
		 * @param value Range 0.0 to 1.0, default 0.0.
		 */
		void setGainLf(float value);

		/** Return lf gain.
		 */
		float getGainLf() const;

		/** The reverberation decay time.
		 * @param value Range 0.0 to 20.0, default 1.49 seconds.
		 */
		void setDecayTime(float value);

		/** Return decay time.
		 */
		float getDecayTime() const;

		/** Sets decay hf ratio, the spectral quality of the Decay Time parameter. It
		 * is the ratio of high-frequency decay time relative to the time set by Decay Time.
		 * @param value Range 0.1 to 2.0, default 0.83.
		 */
		void setDecayHfRatio(float value);

		/** Return decay hf ratio.
		 */
		float getDecayHfRatio() const;

		/** Sets decay lf ratio, the spectral quality of the Decay Time parameter. It
		 * is the ratio of low-frequency decay time relative to the time set by Decay Time.
		 * @param value Range 0.1 to 2.0, default 1.0.
		 */
		void setDecayLfRatio(float value);

		/** Return decay lf ratio.
		 */
		float getDecayLfRatio() const;
		
		/** Sets reflections gain, the overall amount of initial reflections
		 * relative to the Gain property.
		 * @param value Range 0.0 to 3.16, default 0.05.
		 */
		void setReflectionsGain(float value);

		/** Return reflections gain.
		 */
		float getReflectionsGain() const;

		/** Sets the reflections delay, the amount of delay between the arrival time of
		 * the direct path from the source to the first reflection from the source.
		 * @param value Range 0.0 to 0.3, default 0.007.
		 */
		void setReflectionsDelay(float value);

		/** Return reflections delay.
		 */
		float getReflectionsDelay() const;

		/** Sets reflections pan, the 3D vector that controls the spatial distribution
		 * of the cluster of early reflections. The direction of this vector controls
		 * the global direction of the reflections, while its magnitude controls how
		 * focused the reflections are towards this direction.
		 * @param coordinate A 3D vector with values between 0.0 and 1.0.
		 */
		void setReflectionsPan(const AudioSpaceCoordinate& coordinate);

		/** Return reflections pan.
		 */
		AudioSpaceCoordinate getReflectionsPan() const;

		/** Sets late reverb gain, the overall amount of later reverberation relative to gain.
		 * @param value Range 0.0 to 10.0, default 1.26.
		 */
		void setLateReverbGain(float value);

		/** Return late reverb gain.
		 */
		float getLateReverbGain() const;

		/** Sets late reverb delay, the begin time of the late reverberation relative
		 * to the time of the initial reflection.
		 * @param value Range 0.0 to 0.1, default 0.011 seconds.
		 */
		void setLateReverbDelay(float value);

		/** Return late reverb delay.
		 */
		float getLateReverbDelay() const;

		/** Sets late reverb pan, the 3D vector that controls the spatial distribution
		 * of the late reverb. The direction of this vector controls the global direction
		 * of the reverb, while its magnitude controls how focused the reverb are towards this direction.
		 * @param coordinate A 3D vector with values between 0.0 and 1.0.
		 */
		void setLateReverbPan(const AudioSpaceCoordinate& coordinate);

		/** Return late reverb pan.
		 */
		AudioSpaceCoordinate getLateReverbPan() const;

		/** Sets echo time, the rate at which the cyclic echo repeats itself along
		 * the reverberation decay.
		 * @param value Range 0.075 to 0.25, default 0.25.
		 */
		void setEchoTime(float value);

		/** Return the echo time.
		 */
		float getEchoTime() const;

		/** Sets echo depth, the cyclic echo in the reverberation decay, which will be
		 * noticeable with transient or percussive sounds.
		 * @param value Range 0.0 to 1.0, defult 0.0.
		 */
		void setEchoDepth(float value);

		/** Return echo depth.
		 */
		float getEchoDepth() const;

		/** Sets modulation time, the speed of the vibrato (rate of periodic changes in pitch).
		 * @param value Range 0.004 to 4.0, default 0.25 seconds.
		 */
		void setModulationTime(float value);

		/** Return modulation time.
		 */
		float getModulationTime() const;

		/** Sets modulation depth, the amount of pitch change.
		 * @param value Range 0.0 to 1.0, default 0.0.
		 */
		void setModulationDepth(float value);

		/** Return modulation depth.
		 */
		float getModulationDepth() const;

		/** Sets air absorption gain hf, the distance-dependent attenuation at
		 * high frequencies caused by the propagation medium.
		 * @param value Range 0.892 to 1.0, default 0.994.
		 */
		void setAirAbsorptionGainHf(float value);

		/** Return air absorption gain hf.
		 */
		float getAirAbsorptionGainHf() const;

		/** Sets hf reference, determine respectively the frequencies at which the
		 * high-frequency effects created by EAX Reverb properties are measured,
		 * for example Decay HF Ratio.
		 * @param value Range 1000.0 to 20000.0, default 5000.0 Hz.
		 */ 
		void setHfReference(float value);

		/** Return hf reference.
		 */
		float getHfReference() const;

		/** Sets lf reference, determine respectively the frequencies at which the
		 * low-frequency effects created by EAX Reverb properties are measured,
		 * for example Decay LF Ratio.
		 * Note: Should be less than 1/10 of HF Reference.
		 * @param value Range 20.0 to 1000.0 , default 250.0 Hz.
		*/
		void setLfReference(float value);

		/** Return lf reference.
		 */
		float getLfReference() const;

		/** Sets room rolloff factor, similar to rolloff factor it attenuate the reflected sound
		 * (containing both reflections and reverberation) according to source-listener distance.
		 * @param value Range 0.0 to 10.0, default 0.0.
		 */
		void setRoomRolloffFactor(float value);

		/** Return room rolloff factor.
		 */
		float getRoomRolloffFactor() const;

		/** If decay hf limit is enabled, the high-frequency decay time automatically stays
		 * below a limit value that’s derived from the setting of Air Absorption HF.
		 * @param value True or false. Default is true.
		 */
		void setDecayHfLimit(bool value);

		/** Return if decay hf limit is enabled.
		 */
		bool isDecayHfLimit() const;

		/** Load presets into the EAX reverb.
		 * @see SoundEffectPreset
		 */
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
