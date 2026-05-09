// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "util/time/timeevent.h"
%}

namespace FIFE {
	%feature("director") TimeEvent;
	class TimeEvent {
	public:
		TimeEvent(int32_t period = -1);
		virtual ~TimeEvent();
		virtual void updateEvent(uint32_t time) = 0;
		void setPeriod(int32_t period);
		int32_t getPeriod();
		uint32_t getLastUpdateTime();
		uint64_t getLastUpdateTime64();
		void setLastUpdateTime(uint32_t);
		void setLastUpdateTime64(uint64_t);
	};
}
