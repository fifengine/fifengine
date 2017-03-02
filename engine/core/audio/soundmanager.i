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

	enum SoundEffectPreset {
		SE_PRESET_GENERIC,
		SE_PRESET_PADDEDCELL,
		SE_PRESET_ROOM,
		SE_PRESET_BATHROOM,
		SE_PRESET_LIVINGROOM,
		SE_PRESET_STONEROOM,
		SE_PRESET_AUDITORIUM,
		SE_PRESET_CONCERTHALL,
		SE_PRESET_CAVE,
		SE_PRESET_ARENA,
		SE_PRESET_HANGAR,
		SE_PRESET_CARPETEDHALLWAY,
		SE_PRESET_HALLWAY,
		SE_PRESET_STONECORRIDOR,
		SE_PRESET_ALLEY,
		SE_PRESET_FOREST,
		SE_PRESET_CITY,
		SE_PRESET_MOUNTAINS,
		SE_PRESET_QUARRY,
		SE_PRESET_PLAIN,
		SE_PRESET_PARKINGLOT,
		SE_PRESET_SEWERPIPE,
		SE_PRESET_UNDERWATER,
		SE_PRESET_DRUGGED,
		SE_PRESET_DIZZY,
		SE_PRESET_PSYCHOTIC,
		SE_PRESET_CASTLE_SMALLROOM,
		SE_PRESET_CASTLE_SHORTPASSAGE,
		SE_PRESET_CASTLE_MEDIUMROOM,
		SE_PRESET_CASTLE_LARGEROOM,
		SE_PRESET_CASTLE_LONGPASSAGE,
		SE_PRESET_CASTLE_HALL,
		SE_PRESET_CASTLE_CUPBOARD,
		SE_PRESET_CASTLE_COURTYARD,
		SE_PRESET_CASTLE_ALCOVE,
		SE_PRESET_FACTORY_SMALLROOM,
		SE_PRESET_FACTORY_SHORTPASSAGE,
		SE_PRESET_FACTORY_MEDIUMROOM,
		SE_PRESET_FACTORY_LARGEROOM,
		SE_PRESET_FACTORY_LONGPASSAGE,
		SE_PRESET_FACTORY_HALL,
		SE_PRESET_FACTORY_CUPBOARD,
		SE_PRESET_FACTORY_COURTYARD,
		SE_PRESET_FACTORY_ALCOVE,
		SE_PRESET_ICEPALACE_SMALLROOM,
		SE_PRESET_ICEPALACE_SHORTPASSAGE,
		SE_PRESET_ICEPALACE_MEDIUMROOM,
		SE_PRESET_ICEPALACE_LARGEROOM,
		SE_PRESET_ICEPALACE_LONGPASSAGE,
		SE_PRESET_ICEPALACE_HALL,
		SE_PRESET_ICEPALACE_CUPBOARD,
		SE_PRESET_ICEPALACE_COURTYARD,
		SE_PRESET_ICEPALACE_ALCOVE,
		SE_PRESET_SPACESTATION_SMALLROOM,
		SE_PRESET_SPACESTATION_SHORTPASSAGE,
		SE_PRESET_SPACESTATION_MEDIUMROOM,
		SE_PRESET_SPACESTATION_LARGEROOM,
		SE_PRESET_SPACESTATION_LONGPASSAGE,
		SE_PRESET_SPACESTATION_HALL,
		SE_PRESET_SPACESTATION_CUPBOARD,
		SE_PRESET_SPACESTATION_ALCOVE,
		SE_PRESET_WOODEN_SMALLROOM,
		SE_PRESET_WOODEN_SHORTPASSAGE,
		SE_PRESET_WOODEN_MEDIUMROOM,
		SE_PRESET_WOODEN_LARGEROOM,
		SE_PRESET_WOODEN_LONGPASSAGE,
		SE_PRESET_WOODEN_HALL,
		SE_PRESET_WOODEN_CUPBOARD,
		SE_PRESET_WOODEN_COURTYARD,
		SE_PRESET_WOODEN_ALCOVE,
		SE_PRESET_SPORT_EMPTYSTADIUM,
		SE_PRESET_SPORT_SQUASHCOURT,
		SE_PRESET_SPORT_SMALLSWIMMINGPOOL,
		SE_PRESET_SPORT_LARGESWIMMINGPOOL,
		SE_PRESET_SPORT_GYMNASIUM,
		SE_PRESET_SPORT_FULLSTADIUM,
		SE_PRESET_SPORT_STADIUMTANNOY,
		SE_PRESET_PREFAB_WORKSHOP,
		SE_PRESET_PREFAB_SCHOOLROOM,
		SE_PRESET_PREFAB_PRACTISEROOM,
		SE_PRESET_PREFAB_OUTHOUSE,
		SE_PRESET_PREFAB_CARAVAN,
		SE_PRESET_DOME_TOMB,
		SE_PRESET_PIPE_SMALL,
		SE_PRESET_DOME_SAINTPAULS,
		SE_PRESET_PIPE_LONGTHIN,
		SE_PRESET_PIPE_LARGE,
		SE_PRESET_PIPE_RESONANT,
		SE_PRESET_OUTDOORS_BACKYARD,
		SE_PRESET_OUTDOORS_ROLLINGPLAINS,
		SE_PRESET_OUTDOORS_DEEPCANYON,
		SE_PRESET_OUTDOORS_CREEK,
		SE_PRESET_OUTDOORS_VALLEY,
		SE_PRESET_MOOD_HEAVEN,
		SE_PRESET_MOOD_HELL,
		SE_PRESET_MOOD_MEMORY,
		SE_PRESET_DRIVING_COMMENTATOR,
		SE_PRESET_DRIVING_PITGARAGE,
		SE_PRESET_DRIVING_INCAR_RACER,
		SE_PRESET_DRIVING_INCAR_SPORTS,
		SE_PRESET_DRIVING_INCAR_LUXURY,
		SE_PRESET_DRIVING_FULLGRANDSTAND,
		SE_PRESET_DRIVING_EMPTYGRANDSTAND,
		SE_PRESET_DRIVING_TUNNEL,
		SE_PRESET_CITY_STREETS,
		SE_PRESET_CITY_SUBWAY,
		SE_PRESET_CITY_MUSEUM,
		SE_PRESET_CITY_LIBRARY,
		SE_PRESET_CITY_UNDERPASS,
		SE_PRESET_CITY_ABANDONED,
		SE_PRESET_DUSTYROOM,
		SE_PRESET_CHAPEL,
		SE_PRESET_SMALLWATERROOM
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

			void play();
			void pause();
			void stop();
			void rewind();
			
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
			SoundEffect* createSoundEffectPreset(SoundEffectPreset type);
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

			void addToGroup(SoundEmitter* emitter);
			void removeFromGroup(SoundEmitter* emitter);
			void removeGroup(const std::string& group);
			void removeAllGroups();
			void play(const std::string& group);
			void pause(const std::string& group);
			void stop(const std::string& group);
			void rewind(const std::string& group);
			void setGain(const std::string& group, float gain);
			void setMaxGain(const std::string& group, float gain);
			void setMinGain(const std::string& group, float gain);
	};
}
