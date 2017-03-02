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

%module soundmanager
%{
#include "audio/soundmanager.h"
#include "audio/fife_openal.h"
%}

%include "model/metamodel/modelcoords.i"
%include "util/resource/resource.i"

namespace FIFE {

	enum SoundDistanceModelType {
		SD_DISTANCE_NONE,
		SD_DISTANCE_INVERSE,
		SD_DISTANCE_INVERSE_CLAMPED,
		SD_DISTANCE_LINEAR,
		SD_DISTANCE_LINEAR_CLAMPED,
		SD_DISTANCE_EXPONENT,
		SD_DISTANCE_EXPONENT_CLAMPED
	};

	enum SoundFilterType {
		SF_FILTER_NULL,
		SF_FILTER_LOWPASS,
		SF_FILTER_HIGHPASS,
		SF_FILTER_BANDPASS
	};
	
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

	class SoundClip;
	class SoundEffect;
	class SoundFilter;

	class SoundManager {
		public:
			SoundManager();
			~SoundManager();
	
			void init();
			
			SoundEmitter* createEmitter();
			SoundEmitter* createEmitter(const std::string& name);
			SoundEmitter* getEmitter(uint32_t emitterId);
			void releaseEmitter(uint32_t emitterId);
			
			ALCcontext* getContext();
			
			void setVolume(float vol);
			float getVolume() const;
			void mute();
			void unmute();
			
			void setDistanceModel(SoundDistanceModelType model);
			SoundDistanceModelType getDistanceModel() const;

			void setListenerPosition(const AudioSpaceCoordinate& position);
			AudioSpaceCoordinate getListenerPosition() const;
			void setListenerOrientation(const AudioSpaceCoordinate& orientation);
			AudioSpaceCoordinate getListenerOrientation() const;
			void setListenerVelocity(const AudioSpaceCoordinate& velocity);
			AudioSpaceCoordinate getListenerVelocity() const;
			void setDopplerFactor(float factor);
			float getDopplerFactor() const;

			SoundEffect* createSoundEffect(SoundEffectType  type);
			void deleteSoundEffect(SoundEffect* effect);
			void enableSoundEffect(SoundEffect* effect);
			void disableSoundEffect(SoundEffect* effect);
			void addEmitterToSoundEffect(SoundEffect* effect, SoundEmitter* emitter);
			void removeEmitterFromSoundEffect(SoundEffect* effect, SoundEmitter* emitter);

			SoundFilter* createSoundFilter(SoundFilterType type);
			void deleteSoundFilter(SoundFilter* filter);
			void enableSoundFilter(SoundFilter* filter);
			void disableSoundFilter(SoundFilter* filter);
			void addEmitterToSoundFilter(SoundFilter* filter, SoundEmitter* emitter);
			void removeEmitterFromSoundFilter(SoundFilter* filter, SoundEmitter* emitter);
	};
}
