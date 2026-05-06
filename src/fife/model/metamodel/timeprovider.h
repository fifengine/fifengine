// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_TIME_PROVIDER_H
#define FIFE_TIME_PROVIDER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
// 3rd party library includes

// FIFE includes
#include "util/base/exception.h"
#include "util/base/fife_stdint.h"

namespace FIFE
{

    /** Timeprovider is an utility providing time management functionality
     * You can have hierarchy of time providers, whose multiplier can be changed independently.
     * Changing providers upper in the hierarchy, changes also all subproviders with the same ratio
     */
    class /*FIFE_API*/ TimeProvider
    {
        public:
            /** Constructor
             * In case there there is no provider master, you can use NULL
             */
            explicit TimeProvider(TimeProvider* master);
            ~TimeProvider() = default;

            /** With multiplier, you can adjust the time speed. 0.5 means time runs half as slow,
             * while 2.0 means it runs twice as fast
             */
            void setMultiplier(float multiplier);

            /** @see setMultiplier. Returns mutliplier for this single provider
             */
            float getMultiplier() const;

            /** @see setMultiplier. Returns mutliplier for whole chain of timeproviders
             * E.g. if master has multiplier 2.0 and this has 0.5, end result = 1.0
             */
            float getTotalMultiplier() const;

            /** Returns current game ticks, already scaled.
             */
            uint32_t getGameTime() const;

        private:
            TimeProvider* m_master;
            float m_multiplier;
            double m_time_static, m_time_scaled;

            /** Returns current game ticks, already scaled, more precise.
             */
            double getPreciseGameTime() const;
    };

    /** Utility function to calculate time scaling. Mostly done to avoid littering other code
     * with related casting
     * @return multiplier * ticks
     */
    uint32_t scaleTime(float multiplier, uint32_t ticks);

} // namespace FIFE
#endif
