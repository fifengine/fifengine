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
		uint32_t getTime() const;
		uint32_t getTimeDelta() const;
		double getAverageFrameTime() const;
		void printStatistics() const;
		void registerEvent(TimeEvent* event);
		void unregisterEvent(TimeEvent* event);
        };
}