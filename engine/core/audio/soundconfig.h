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

#ifndef FIFE_AUDIO_CONFIG_H
#define FIFE_AUDIO_CONFIG_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fife_stdint.h"

namespace FIFE {

	/** Sound filter type
	 */
	enum SoundFilterType {
		SF_FILTER_NULL,
		SF_FILTER_LOWPASS,
		SF_FILTER_HIGHPASS,
		SF_FILTER_BANDPASS
	};

	/** Sound effect type
	 */
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

	/** Presets for EAX Reverb
	 */
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

	/* The max. length of a decoded audio data
	 * for which streaming is not used. (bytes)
	 * Has to be <= than BUFFERLEN * 3.
	 */
	const uint32_t MAX_KEEP_IN_MEM = 3145728;

	// The number of buffers used for streaming.
	const int16_t BUFFER_NUM = 3;

	// The length of one buffer. (bytes)
	const uint32_t BUFFER_LEN = 1048576;

	// The max. number of OpenAL sources.
	const uint16_t MAX_SOURCES = 64;

	// The max. number of OpenAL effect slots.
	const uint16_t MAX_EFFECT_SLOTS = 32;
}

#endif
