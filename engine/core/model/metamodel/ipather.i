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
