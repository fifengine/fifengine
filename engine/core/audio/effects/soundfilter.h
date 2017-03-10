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

#ifndef FIFE_SOUNDFILTER_H
#define FIFE_SOUNDFILTER_H

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "audio/fife_openal.h"
#include "audio/soundconfig.h"

namespace FIFE {

	/** The class defines filters. Lowpass, Highpass and Bandpass filters are possible.
	 * Note: On Lowpass filter setGainLf have no effect, same with Highpass and setGainHf.
	 */
	class SoundFilter {
	public:
		/** Constructor
		 * @param type The filter type.
		 */
		SoundFilter(SoundFilterType type);

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
}
#endif
