// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "model/metamodel/ipather.h"
%}

%include "model/structures/instance.i"
%include "pathfinder/route.h"

namespace FIFE {
	enum PriorityType {
		HIGH_PRIORITY,
		MEDIUM_PRIORITY,
		LOW_PRIORITY
	};
	
	%feature("director") IPather;
	class IPather {
	public:
		virtual ~IPather();
		virtual Route* createRoute(const Location& start, const Location& end, bool immediate = false, const std::string& cost_id = "") = 0;
		virtual bool solveRoute(Route* route, int32_t priority = MEDIUM_PRIORITY, bool immediate = false) = 0;
		virtual bool followRoute(const Location& current, Route* route, double speed, Location& nextLocation) = 0;
		virtual void update() = 0;
		virtual bool cancelSession(const int32_t sessionId) = 0;
		virtual void setMaxTicks(int32_t ticks) = 0;
		virtual int32_t getMaxTicks() = 0;
		virtual std::string getName() const = 0;
	};
}