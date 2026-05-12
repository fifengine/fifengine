// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_SOUNDFILTER_H
#define FIFE_SOUNDFILTER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
// 3rd party library includes

// FIFE includes
#include "audio/fife_openal.h"
#include "audio/soundconfig.h"

namespace FIFE
{

    /** The class defines filters. Lowpass, Highpass and Bandpass filters are possible.
     * Note: On Lowpass filter setGainLf have no effect, same with Highpass and setGainHf.
     */
    class FIFE_API SoundFilter
    {
        public:
            /** Constructor
             * @param type The filter type.
             */
            explicit SoundFilter(SoundFilterType type);

            /** Destructor
             */
            ~SoundFilter();

            /** Return the OpenAL filter handle.
             */
            ALuint getFilterId() const;

            /** Sets the filter type.
             * @see SoundFilterType
             */
            void setFilterType(SoundFilterType type);

            /** Return the filter type
             * @see SoundFilterType
             */
            SoundFilterType getFilterType() const;

            /** Enables or disables the filter.
             * @param enabled A bool to indicate if the filter should be enabled or disabled.
             */
            void setEnabled(bool enabled);

            /** Return true if the filter is enabled, false otherwise.
             */
            bool isEnabled() const;

            /** Sets filter gain.
             * @param gain The gain as float, range 0.0 - 1.0.
             */
            void setGain(float gain);

            /** Return filter gain. Default is 1.0.
             */
            float getGain() const;

            /** Sets filter high frequency gain.
             * @param gain The gain as float, range 0.0 - 1.0.
             */
            void setGainHf(float gain);

            /** Return filter high frequency gain. Default is 1.0.
             */
            float getGainHf() const;

            /** Sets filter low frequency gain.
             * @param gain The gain as float, range 0.0 - 1.0.
             */
            void setGainLf(float gain);

            /** Return filter low frequency gain. Default is 1.0.
             */
            float getGainLf() const;

        private:
            //! Filter object id
            ALuint m_filter;
            //! Filter type
            SoundFilterType m_type;
            //! Filter enabled
            bool m_enabled;
            //! Gain
            float m_gain;
            //! High frequency gain
            float m_hGain;
            //! Low frequency gain
            float m_lGain;
    };
} // namespace FIFE
#endif
