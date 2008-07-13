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

#ifndef FIFE_PATHFINDER_HEURISTIC_H
#define FIFE_PATHFINDER_HEURISTIC_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/modelcoords.h"
#include "util/base/singleton.h"

namespace FIFE {
	class Heuristic {
	public:
		virtual ~Heuristic(void) { }

		virtual float calculate(const ModelCoordinate& current, const ModelCoordinate& dest) = 0;

		static Heuristic* getHeuristic(const std::string& cellgridType);
	};

	class SquareGridHeuristic : public Heuristic, public StaticSingleton<SquareGridHeuristic>  {	
	public:
		virtual float calculate(const ModelCoordinate& current, const ModelCoordinate& dest);
	
	private:
		SINGLEFRIEND(SquareGridHeuristic);
	};

	class HexGridHeuristic : public Heuristic, public StaticSingleton<HexGridHeuristic> {
	public:
		virtual float calculate(const ModelCoordinate& current, const ModelCoordinate& dest);
	
	private:
		SINGLEFRIEND(HexGridHeuristic);
	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
