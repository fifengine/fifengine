// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "util/time/timemanager.h"
%}

namespace FIFE {
	%feature("deprecated", "Use now64() instead.") TimeManager::getTime;
	%feature("deprecated", "Use getTimeDelta64() instead.") TimeManager::getTimeDelta;

	class TimeEvent;
	class TimeManager {
	public:
		TimeManager();
		virtual ~TimeManager();
		void update();
		uint32_t getTime() const;
		uint64_t now64() const;
		uint32_t getTimeDelta() const;
		uint64_t getTimeDelta64() const;
		uint64_t getTicks64() const;
		void sleep64(uint64_t ms) const;
		double getAverageFrameTime() const;
		void printStatistics() const;
		void registerEvent(TimeEvent* event);
		void unregisterEvent(TimeEvent* event);
        };
}
