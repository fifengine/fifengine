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

	SquareGrid::SquareGrid():
		CellGrid() {
	}

	CellGrid* SquareGrid::clone() {
		SquareGrid* nGrid = new SquareGrid();
		nGrid->setRotation(m_rotation);
		nGrid->setXScale(m_xscale);
		nGrid->setYScale(m_yscale);
		nGrid->setXShift(m_xshift);
		nGrid->setYShift(m_yshift);
		nGrid->setZShift(m_zshift);
		nGrid->setAllowDiagonals(m_allow_diagonals);

		return nGrid;
	}

	SquareGrid::~SquareGrid() {
	}

	bool SquareGrid::isAccessible(const ModelCoordinate& curpos, const ModelCoordinate& target) {
		uint8_t x = ABS(target.x-curpos.x);
		uint8_t y = ABS(target.y-curpos.y); 
		if ((x<=1) && (y<=1)) {
			if (m_allow_diagonals) {
				return true;
			} else if (x^y) {
				return true;
			}
		}

		return false;
	}

	double SquareGrid::getAdjacentCost(const ModelCoordinate& curpos, const ModelCoordinate& target) {
		if (curpos == target) {
			return 0.0;
		} else if (ABS(target.x-curpos.x)^ABS(target.y-curpos.y)) {
			return 1.0;
		}
		return 1.4;
	}
	
	double SquareGrid::getHeuristicCost(const ModelCoordinate& curpos, const ModelCoordinate& target) {
		return static_cast<double>(ABS(target.x - curpos.x) + ABS(target.y - curpos.y));
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
		return toLayerCoordinatesFromExactLayerCoordinates(dblpt);
	}

	ModelCoordinate SquareGrid::toLayerCoordinatesFromExactLayerCoordinates(const ExactModelCoordinate& exact_layer_coords) {
		ModelCoordinate result(round(exact_layer_coords.x), round(exact_layer_coords.y), round(exact_layer_coords.z));
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

	std::vector<ModelCoordinate> SquareGrid::toMultiCoordinates(const ModelCoordinate& position, const std::vector<ModelCoordinate>& orig, bool reverse) {
		std::vector<ModelCoordinate> coords;
		std::vector<ModelCoordinate>::const_iterator it = orig.begin();
		if (reverse) {
			for (; it != orig.end(); ++it) {
				ModelCoordinate mc = position;
				mc.x -= (*it).x;
				mc.y -= (*it).y;
				coords.push_back(mc);
			}
		} else {
			for (; it != orig.end(); ++it) {
				ModelCoordinate mc = position;
				mc.x += (*it).x;
				mc.y += (*it).y;
				coords.push_back(mc);
			}
		}
		return coords;
	}

	std::vector<ModelCoordinate> SquareGrid::getCoordinatesInLine(const ModelCoordinate& start, const ModelCoordinate& end) {
		std::vector<ModelCoordinate> coords;
		int32_t dx = ABS(end.x - start.x);
		int32_t dy = ABS(end.y - start.y);
		int8_t sx = -1;
		int8_t sy = -1;

		if (start.x < end.x) {
			sx = 1;
		}
		if (start.y < end.y) {
			sy = 1;
		}

		int32_t err = dx - dy;
		int32_t err2 = err*2;
		ModelCoordinate current(start);
		bool finished = false;
		while (!finished) {
			coords.push_back(current);
			if (current.x == end.x && current.y == end.y) {
				finished = true;
				break;
			}
			
			if (err2 > -dy) {
				err -= dy;
				current.x += sx;
			} else if (err2 < dx) {
				err += dx;
				current.y += sy;
			}
			err2 = err*2;
		}
		return coords;
	}
}
