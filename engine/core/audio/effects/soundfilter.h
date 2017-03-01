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

	class SoundFilter {
	public:
		/** Constructor
		 */
		SoundFilter(SoundFilterType type);

		/** Destructor
		 */
		~SoundFilter();

		ALuint getFilterId() const;

		void setFilterType(SoundFilterType type);
		SoundFilterType getFilterType() const;

		void setEnabled(bool enabled);
		bool isEnabled() const;

		void setGain(float gain);
		float getGain() const;

		void setGainHF(float gain);
		float getGainHF() const;

		void setGainLF(float gain);
		float getGainLF() const;

	private:
		//! Filter object id
		ALuint m_filter;
		//! Filter type
		SoundFilterType m_type;
		//! Filter enabled
		bool m_enabled;
		//! Gain
		float m_gain;
		//! High frequence gain
		float m_hGain;
		//! Low frequence gain
		float m_lGain;
	};
}
#endif
