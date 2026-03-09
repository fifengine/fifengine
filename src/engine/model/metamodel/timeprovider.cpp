// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/time/timemanager.h"

#include "timeprovider.h"

namespace FIFE
{
    TimeProvider::TimeProvider(TimeProvider* master) : m_master(master), m_multiplier(1.0)
    {
        m_time_static = m_time_scaled = (master != nullptr) ? master->getGameTime() : TimeManager::instance()->getTime();
    }

    TimeProvider::~TimeProvider() = default;

    void TimeProvider::setMultiplier(float multiplier)
    {
        if (multiplier < 0.0) {
            throw NotSupported("Negative time multiplier are not supported");
        }
        m_time_static = getPreciseGameTime();
        m_time_scaled =
            (m_master != nullptr) ? m_master->getPreciseGameTime() : static_cast<float>(TimeManager::instance()->getTime());
        m_multiplier = multiplier;
    }

    float TimeProvider::getMultiplier() const
    {
        return m_multiplier;
    }

    float TimeProvider::getTotalMultiplier() const
    {
        if (m_master != nullptr) {
            return m_master->getTotalMultiplier() * m_multiplier;
        } else {
            return m_multiplier;
        }
    }

    uint32_t TimeProvider::getGameTime() const
    {
        return static_cast<uint32_t>(getPreciseGameTime());
    }

    double TimeProvider::getPreciseGameTime() const
    {
        return m_time_static + (m_multiplier * (((m_master != nullptr) ? m_master->getPreciseGameTime() :
                                                           static_cast<float>(TimeManager::instance()->getTime())) -
                                               m_time_scaled));
    }

    uint32_t scaleTime(float multiplier, uint32_t ticks)
    {
        return static_cast<uint32_t>(static_cast<float>(ticks) * multiplier);
    }
} // namespace FIFE