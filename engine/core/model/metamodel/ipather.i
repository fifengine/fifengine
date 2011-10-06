/***************************************************************************
 *   Copyright (C) 2005-2011 by the FIFE team                              *
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

namespace FIFE {
	class Map;
	
	class IPather {
	public:
		virtual ~IPather();
		virtual int32_t getNextLocation(const Instance* instance, const Location& target, 
		                            double distance_to_travel, Location& nextLocation,
		                            Location& facingLocation, int32_t session_id=-1, 
									int32_t priority = MEDIUM_PRIORITY) = 0;
		virtual std::string getName() const = 0;
	private:
		IPather();
	};
}
