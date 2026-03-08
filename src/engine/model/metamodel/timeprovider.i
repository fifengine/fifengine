// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "model/metamodel/timeprovider.h"
%}

namespace FIFE {
	class TimeProvider {
	public:
		TimeProvider(TimeProvider* master);
		~TimeProvider();
		
		void setMultiplier(float multiplier);
		float getMultiplier() const;
		float getTotalMultiplier() const;
		uint32_t getGameTime() const;
	};
}