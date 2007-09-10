/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes
#include <assert.h>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/fife_math.h"
#include "util/logger.h"

#include "hexgrid.h"

namespace FIFE {
	static Logger _log(LM_HEXGRID);

	static const double SIDE_DIST = 0.5;
	static const double EDGE_DIST = 0.57735026918962584;
	static const double SIDE_LENGTH = 0.57735026918962595;
	static const double SIDE_HALF = SIDE_LENGTH / 2;

	HexGrid::HexGrid(): CellGrid() {
	}

	HexGrid::~HexGrid() {
	}

	bool HexGrid::isAccessible(const Point& curpos, const Point& target) {
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

		if ((curpos.x - 1 == target.x) && (curpos.y - 1 == target.y))
			return true;
		if ((curpos.x + 1 == target.x) && (curpos.y + 1 == target.y))
			return true;

		return false;
	}

	float HexGrid::getAdjacentCost(const Point& curpos, const Point& target) {
		assert(isAccessible(curpos, target));
		if (curpos == target) {
			return 0;
		}
		return 1;
	}
	
	const std::string& HexGrid::getName() const {
		static std::string hexGrid("Hex Grid");
		return hexGrid;
	}

	double HexGrid::getXOffsetMultiplier(double y) {
		// each uneven row has shifted coordinate of 0.5 horizontally
		// shift has to be gradual on vertical axis
		int i_layer_y = static_cast<int>(y);
		double y_offset = y - static_cast<double>(i_layer_y);
		double x_offset_multiplier = ABS(y_offset);
		if ((i_layer_y % 2) == 0) {
			x_offset_multiplier = 1.0 - x_offset_multiplier;
			x_offset_multiplier = ABS(x_offset_multiplier);
		}
		return x_offset_multiplier;
	}
	
	DoublePoint HexGrid::toElevationCoordinates(const DoublePoint& layer_coords) {
		DoublePoint tranformed_coords(layer_coords);
		tranformed_coords.x -= getXOffsetMultiplier(layer_coords.y);
		DoublePoint result = m_matrix * tranformed_coords;
		FL_DBG(_log, LMsg("layercoords ") << layer_coords << " converted to elev: " << result);
		return result;
	}

	DoublePoint HexGrid::toExactLayerCoordinates(const DoublePoint& elevation_coord) {
		DoublePoint layer_coords = m_inverse_matrix * elevation_coord;
		layer_coords.x += getXOffsetMultiplier(layer_coords.y);
		FL_DBG(_log, LMsg("elevcoords ") << elevation_coord << " converted to layer: " << layer_coords);
		return layer_coords;
	}

	Point HexGrid::toLayerCoordinates(const DoublePoint& elevation_coord) {
		FL_DBG(_log, LMsg("Converting elev coords ") << elevation_coord << " to int layer coords...");
		DoublePoint elc = m_inverse_matrix * elevation_coord;
		DoublePoint lc = DoublePoint(floor(elc.x), floor(elc.y));
		double dx = elc.x - lc.x;
		double dy = elc.y - lc.y;
		int x = static_cast<int>(lc.x);
		int y = static_cast<int>(lc.y);
		Point result;
		// in even rows
		if ((y % 2) == 0) {
			// in lower rect area
			if (dy < SIDE_HALF) {
				result = Point(x+1, y+1);
			} 
			// in upper rect area
			else if ((1 - dy) < SIDE_HALF) {
				// right
				if (dx > 0.5) {
					result = Point(x+1, y);
				} 
				// left
				else {
					result = Point(x, y);
				}
			} 
			// in middle triangle area
			else {
				// in left triangles
				if (dx < 0.5) {
					if (ptInTriangle(DoublePoint(0, SIDE_HALF), DoublePoint(0, 1-SIDE_HALF), 
					                DoublePoint(0.5, SIDE_HALF), DoublePoint(dx, dy))) {
						result = Point(x, y);
					} else {
						result = Point(x, y+1);
					}
				} else {
					if (ptInTriangle(DoublePoint(1, SIDE_HALF), DoublePoint(1, 1-SIDE_HALF), 
					                DoublePoint(0.5, SIDE_HALF), DoublePoint(dx, dy))) {
						result = Point(x+1, y);
					} else {
						result = Point(x, y+1);
					}
				}
			}		
		} 
		// in uneven rows
		else {
			// in upper rect area
			if (dy < SIDE_HALF) {
				result = Point(x, y);
			} 
			// in lower rect area
			else if ((1 - dy) < SIDE_HALF) {
				// right
				if (dx > 0.5) {
					result = Point(x+1, y+1);
				} 
				// left
				else {
					result = Point(x, y+1);
				}
			} 
			// in middle triangle area
			else {
				// in left triangles
				if (dx < 0.5) {
					if (ptInTriangle(DoublePoint(0, SIDE_HALF), DoublePoint(0, 1-SIDE_HALF), 
					                DoublePoint(0.5, 1-SIDE_HALF), DoublePoint(dx, dy))) {
						result = Point(x, y+1);
					} else {
						result = Point(x, y);
					}
				} else {
					if (ptInTriangle(DoublePoint(1, SIDE_HALF), DoublePoint(1, 1-SIDE_HALF), 
					                DoublePoint(0.5, 1-SIDE_HALF), DoublePoint(dx, dy))) {
						result = Point(x+1, y+1);
					} else {
						result = Point(x, y);
					}
				}
			}
		}
		FL_DBG(_log, LMsg("  result = ") << result);
		return result;
	}

	void HexGrid::getVertices(std::vector<DoublePoint>& vtx, const Point& cell) {
		vtx.clear();
		double x = static_cast<double>(cell.x);
		double y = static_cast<double>(cell.y);
		vtx.push_back(DoublePoint((x-0.5) + getXOffsetMultiplier(y-SIDE_HALF), y-SIDE_HALF));
		vtx.push_back(DoublePoint(x + getXOffsetMultiplier(y-EDGE_DIST),     y-EDGE_DIST));
		vtx.push_back(DoublePoint((x+0.5) + getXOffsetMultiplier(y-SIDE_HALF), y-SIDE_HALF));
		vtx.push_back(DoublePoint((x+0.5) + getXOffsetMultiplier(y+SIDE_HALF), y+SIDE_HALF));
		vtx.push_back(DoublePoint(x + getXOffsetMultiplier(y+EDGE_DIST),     y+EDGE_DIST));
		vtx.push_back(DoublePoint((x-0.5) + getXOffsetMultiplier(y+SIDE_HALF), y+SIDE_HALF));
	}
}
