// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "util/time/timemanager.h"
%}

namespace FIFE {
	class TimeEvent;
	class TimeManager {
	public:
		TimeManager();
		virtual ~TimeManager();
		void update();
		uint64_t now64() const;
		uint64_t getTimeDelta64() const;
		uint64_t getTicks64() const;
		void sleep64(uint64_t ms) const;
		double getAverageFrameTime() const;
		void printStatistics() const;
		void registerEvent(TimeEvent* event);
		void unregisterEvent(TimeEvent* event);
        };
}
