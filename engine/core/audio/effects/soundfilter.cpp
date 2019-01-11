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

#include "soundfilter.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_AUDIO);

	SoundFilter::SoundFilter(SoundFilterType type) :
		m_filter(0),
		m_type(SF_FILTER_NULL),
		m_enabled(false),
		m_gain(1.0),
		m_hGain(1.0),
		m_lGain(1.0) {

		alGenFilters(1, &m_filter);
		CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error creating filter");
		setFilterType(type);
	}

	SoundFilter::~SoundFilter() {
		alDeleteFilters(1, &m_filter);
	}

	ALuint SoundFilter::getFilterId() const {
		return m_filter;
	}

	void SoundFilter::setFilterType(SoundFilterType type) {
		if (m_type == type || isEnabled()) {
			return;
		}
		m_type = type;
		if (m_type == SF_FILTER_NULL) {
			alFilteri(m_filter, AL_FILTER_TYPE, AL_FILTER_NULL);
		} else if (m_type == SF_FILTER_LOWPASS) {
			alFilteri(m_filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
		} else if (m_type == SF_FILTER_HIGHPASS) {
			alFilteri(m_filter, AL_FILTER_TYPE, AL_FILTER_HIGHPASS);
		} else if (m_type == SF_FILTER_BANDPASS) {
			alFilteri(m_filter, AL_FILTER_TYPE, AL_FILTER_BANDPASS);
		}
		CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error setting filter");
	}

	SoundFilterType SoundFilter::getFilterType() const {
		return m_type;
	}

	void SoundFilter::setEnabled(bool enabled) {
		m_enabled = enabled;
	}

	bool SoundFilter::isEnabled() const {
		return m_enabled;
	}

	void SoundFilter::setGain(float gain) {
		gain = std::min(gain, 1.0f);
		gain = std::max(gain, 0.0f);
		m_gain = gain;

		if (m_type == SF_FILTER_LOWPASS) {
			alFilterf(m_filter, AL_LOWPASS_GAIN, m_gain);
		} else if (m_type == SF_FILTER_HIGHPASS) {
			alFilterf(m_filter, AL_HIGHPASS_GAIN, m_gain);
		} else if (m_type == SF_FILTER_BANDPASS) {
			alFilterf(m_filter, AL_BANDPASS_GAIN, m_gain);
		}
	}

	float SoundFilter::getGain() const {
		return m_gain;
	}

	void SoundFilter::setGainHf(float gain) {
		gain = std::min(gain, 1.0f);
		gain = std::max(gain, 0.0f);
		m_hGain = gain;

		if (m_type == SF_FILTER_LOWPASS) {
			alFilterf(m_filter, AL_LOWPASS_GAINHF, m_hGain);
		} else if (m_type == SF_FILTER_BANDPASS) {
			alFilterf(m_filter, AL_BANDPASS_GAINHF, m_hGain);
		}
	}

	float SoundFilter::getGainHf() const {
		return m_hGain;
	}

	void SoundFilter::setGainLf(float gain) {
		gain = std::min(gain, 1.0f);
		gain = std::max(gain, 0.0f);
		m_lGain = gain;

		if (m_type == SF_FILTER_HIGHPASS) {
			alFilterf(m_filter, AL_HIGHPASS_GAINLF, m_lGain);
		}
		else if (m_type == SF_FILTER_BANDPASS) {
			alFilterf(m_filter, AL_BANDPASS_GAINLF, m_lGain);
		}
	}

	float SoundFilter::getGainLf() const {
		return m_lGain;
	}

} //FIFE
