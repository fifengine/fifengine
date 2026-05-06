// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module soundfilter
%{
#include "audio/effects/soundfilter.h"
%}

namespace FIFE {

	enum SoundFilterType {
		SF_FILTER_NULL,
		SF_FILTER_LOWPASS,
		SF_FILTER_HIGHPASS,
		SF_FILTER_BANDPASS
	};

	class SoundFilter {
	public:
		SoundFilterType getFilterType() const;
		bool isEnabled() const;
		void setGain(float gain);
		float getGain() const;
		void setGainHf(float gain);
		float getGainHf() const;
		void setGainLf(float gain);
		float getGainLf() const;
	
	private:
		SoundFilter(SoundFilterType type);
	};
}
