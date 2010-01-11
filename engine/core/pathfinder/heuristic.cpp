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

// Standard C++ library includes
#include "util/math/fife_math.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "heuristic.h"

namespace FIFE {

	Heuristic* Heuristic::getHeuristic(const std::string& cellgridType) {
		if(cellgridType == "square") {
			return SquareGridHeuristic::instance();
		}

		if(cellgridType == "hexagonal") {
			return HexGridHeuristic::instance();
		}

		return 0;
	}

	SquareGridHeuristic::SquareGridHeuristic(void) {
	}

	SquareGridHeuristic::~SquareGridHeuristic(void) {
	}

	float SquareGridHeuristic::calculate(const ModelCoordinate& current, const ModelCoordinate& dest) {
		return (float)(ABS(dest.x - current.x) + ABS(dest.y - current.y));
	}

	HexGridHeuristic::HexGridHeuristic(void) {
	}

	HexGridHeuristic::~HexGridHeuristic(void) {
	}

	float HexGridHeuristic::calculate(const ModelCoordinate& current, const ModelCoordinate& dest) {
		float cost = (float)(((dest.x - current.x) * (dest.x - current.x)) +
					 ((dest.y - current.y) * (dest.y - current.y)) +
					 ((dest.x - current.x) * (dest.y - current.y)));
		return cost;
	}
}
