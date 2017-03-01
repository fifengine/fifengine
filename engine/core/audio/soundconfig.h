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
		SE_EFFECT_EAXREVERB,
		SE_EFFECT_FADE,
		SE_EFFECT_FADE_IN,
		SE_EFFECT_FADE_OUT
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
