// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "soundfilter.h"

// Standard C++ library includes
#include <algorithm>
#include <array>

// Platform specific includes

// 3rd party library includes

// FIFE includes
#include "util/base/exception.h"
#include "util/log/logger.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_AUDIO);

    SoundFilter::SoundFilter(SoundFilterType type) :
        m_filter(0), m_type(SF_FILTER_NULL), m_enabled(false), m_gain(1.0), m_hGain(1.0), m_lGain(1.0)
    {

        alGenFilters(1, &m_filter);
        CHECK_OPENAL_LOG(_log, LogManager::LEVEL_ERROR, "error creating filter");
        setFilterType(type);
    }

    SoundFilter::~SoundFilter()
    {
        alDeleteFilters(1, &m_filter);
    }

    ALuint SoundFilter::getFilterId() const
    {
        return m_filter;
    }

    void SoundFilter::setFilterType(SoundFilterType type)
    {
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

    SoundFilterType SoundFilter::getFilterType() const
    {
        return m_type;
    }

    void SoundFilter::setEnabled(bool enabled)
    {
        m_enabled = enabled;
    }

    bool SoundFilter::isEnabled() const
    {
        return m_enabled;
    }

    void SoundFilter::setGain(float gain)
    {
        gain   = std::min(gain, 1.0F);
        gain   = std::max(gain, 0.0F);
        m_gain = gain;

        ALenum gainParam                                                              = 0;
        static constexpr std::array<std::pair<SoundFilterType, ALenum>, 3> gainParams = {
            std::pair{SF_FILTER_LOWPASS, AL_LOWPASS_GAIN},
            std::pair{SF_FILTER_HIGHPASS, AL_HIGHPASS_GAIN},
            std::pair{SF_FILTER_BANDPASS, AL_BANDPASS_GAIN},
        };
        for (auto const& [filterType, param] : gainParams) {
            if (m_type == filterType) {
                gainParam = param;
                break;
            }
        }

        if (gainParam != 0) {
            alFilterf(m_filter, gainParam, m_gain);
        }
    }

    float SoundFilter::getGain() const
    {
        return m_gain;
    }

    void SoundFilter::setGainHf(float gain)
    {
        gain    = std::min(gain, 1.0F);
        gain    = std::max(gain, 0.0F);
        m_hGain = gain;

        if (m_type == SF_FILTER_LOWPASS) {
            alFilterf(m_filter, AL_LOWPASS_GAINHF, m_hGain);
        } else if (m_type == SF_FILTER_BANDPASS) {
            alFilterf(m_filter, AL_BANDPASS_GAINHF, m_hGain);
        }
    }

    float SoundFilter::getGainHf() const
    {
        return m_hGain;
    }

    void SoundFilter::setGainLf(float gain)
    {
        gain    = std::min(gain, 1.0F);
        gain    = std::max(gain, 0.0F);
        m_lGain = gain;

        ALenum gainLfParam                                                              = 0;
        static constexpr std::array<std::pair<SoundFilterType, ALenum>, 2> gainLfParams = {
            std::pair{SF_FILTER_HIGHPASS, AL_HIGHPASS_GAINLF},
            std::pair{SF_FILTER_BANDPASS, AL_BANDPASS_GAINLF},
        };
        for (auto const& [filterType, param] : gainLfParams) {
            if (m_type == filterType) {
                gainLfParam = param;
                break;
            }
        }

        if (gainLfParam != 0) {
            alFilterf(m_filter, gainLfParam, m_lGain);
        }
    }

    float SoundFilter::getGainLf() const
    {
        return m_lGain;
    }

} // namespace FIFE
