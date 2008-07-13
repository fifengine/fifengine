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
#include <cassert>
#include <iostream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/math/fife_math.h"
#include "util/log/logger.h"

#include "squaregrid.h"

namespace FIFE {
	static Logger _log(LM_SQUAREGRID);

	SquareGrid::SquareGrid(bool allow_diagonals): 
		CellGrid(allow_diagonals) {
	}

	CellGrid* SquareGrid::clone() {
		return new SquareGrid(this);
	}

	SquareGrid::~SquareGrid() {
	}

	bool SquareGrid::isAccessible(const ModelCoordinate& curpos, const ModelCoordinate& target) {
		if (curpos == target)
			return true;
		if ((curpos.x == target.x) && (curpos.y - 1 == target.y))
			return true;
		if ((curpos.x == target.x) && (curpos.y + 1 == target.y))
			return true;
		if ((curpos.x + 1 == target.x) && (curpos.y == target.y))
			return true;
		if ((curpos.x - 1 == target.x) && (curpos.y == target.y))
			return true;

		if (m_allow_diagonals) {
			return isAccessibleDiagonal(curpos, target);
		}

		return false;
	}

	bool SquareGrid::isAccessibleDiagonal(const ModelCoordinate& curpos, const ModelCoordinate& target) {
		if ((curpos.x - 1 == target.x) && (curpos.y - 1 == target.y))
			return true;
		if ((curpos.x - 1 == target.x) && (curpos.y + 1 == target.y))
			return true;
		if ((curpos.x + 1 == target.x) && (curpos.y - 1 == target.y))
			return true;
		if ((curpos.x + 1 == target.x) && (curpos.y + 1 == target.y))
			return true;
		return false;
	}

	float SquareGrid::getAdjacentCost(const ModelCoordinate& curpos, const ModelCoordinate& target) {
		assert(isAccessible(curpos, target));
		if (curpos == target) {
			return 0;
		}
		if (isAccessibleDiagonal(curpos, target)) {
			return sqrt(m_xscale*m_xscale + m_yscale*m_yscale);
		}
		if (curpos.x == target.x) {
			return m_xscale;
		}
		return m_yscale;
	}

	const std::string& SquareGrid::getType() const {
		static std::string type("square");
		return type;
	}

	const std::string& SquareGrid::getName() const {
		static std::string squareGrid("Square Grid");
		return squareGrid;
	}

	ExactModelCoordinate SquareGrid::toMapCoordinates(const ExactModelCoordinate& layer_coords) {
		return m_matrix * layer_coords;
	}

	ExactModelCoordinate SquareGrid::toExactLayerCoordinates(const ExactModelCoordinate& map_coord) {
		return m_inverse_matrix * map_coord;
	}

	ModelCoordinate SquareGrid::toLayerCoordinates(const ExactModelCoordinate& map_coord) {
		ExactModelCoordinate dblpt = toExactLayerCoordinates(map_coord);
		ModelCoordinate result;
		result.x = static_cast<int>(floor(dblpt.x));
		result.y = static_cast<int>(floor(dblpt.y));
		
		if ((dblpt.x - static_cast<double>(result.x)) > 0.5) {
			result.x++;
		}
		if ((dblpt.y - static_cast<double>(result.y)) > 0.5) {
			result.y++;
		}
		
		return result;
	}
	
	void SquareGrid::getVertices(std::vector<ExactModelCoordinate>& vtx, const ModelCoordinate& cell) {
		vtx.clear();
		double x = static_cast<double>(cell.x);
		double y = static_cast<double>(cell.y);
		vtx.push_back(ExactModelCoordinate(x-0.5, y-0.5));
		vtx.push_back(ExactModelCoordinate(x+0.5, y-0.5));
		vtx.push_back(ExactModelCoordinate(x+0.5, y+0.5));
		vtx.push_back(ExactModelCoordinate(x-0.5, y+0.5));
	}
}
