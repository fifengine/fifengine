/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include "model/metamodel/abstractpather.h"
%}

%include "model/structures/instance.i"

namespace FIFE {
	class Map;
	
	class AbstractPather {
	public:
		virtual ~AbstractPather();
		virtual int getNextLocation(const Instance* instance, const Location& target, 
		                            double distance_to_travel, Location& nextLocation,
		                            Location& facingLocation, int session_id=-1, 
									int priority = MEDIUM_PRIORITY) = 0;
		virtual std::string getName() const = 0;
	private:
		AbstractPather();
	};
}
