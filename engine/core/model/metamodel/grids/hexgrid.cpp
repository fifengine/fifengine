/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/math/fife_math.h"
#include "util/log/logger.h"

#include "hexgrid.h"

namespace FIFE {
	static Logger _log(LM_HEXGRID);

	static const double HEX_WIDTH = 1;
	static const double HEX_TO_EDGE = HEX_WIDTH / 2;
	static const double HEX_TO_CORNER = 0.5 / Mathd::Cos(Mathd::pi() / 6);
	static const double HEX_EDGE_HALF = HEX_TO_CORNER * Mathd::Sin(Mathd::pi() / 6);
	static const double VERTICAL_MULTIP = Mathd::Sqrt(HEX_WIDTH*HEX_WIDTH - HEX_TO_EDGE*HEX_TO_EDGE);
	static const double VERTICAL_MULTIP_INV = 1 / VERTICAL_MULTIP;
	static const double HEX_EDGE_GRADIENT = 1 / Mathd::Sqrt(3);

	HexGrid::HexGrid():
		CellGrid() {
		FL_DBG(_log, "Constructing new HexGrid");
		FL_DBG(_log, LMsg("HEX_WIDTH ") << HEX_WIDTH);
		FL_DBG(_log, LMsg("HEX_TO_EDGE ") << HEX_TO_EDGE);
		FL_DBG(_log, LMsg("HEX_TO_CORNER ") << HEX_TO_CORNER);
		FL_DBG(_log, LMsg("HEX_EDGE_HALF ") << HEX_EDGE_HALF);
		FL_DBG(_log, LMsg("VERTICAL_MULTIP ") << VERTICAL_MULTIP);
	}

	CellGrid* HexGrid::clone() {
		HexGrid* nGrid = new HexGrid();
		nGrid->setRotation(m_rotation);
		nGrid->setXScale(m_xscale);
		nGrid->setYScale(m_yscale);
		nGrid->setXShift(m_xshift);
		nGrid->setYShift(m_yshift);
		nGrid->setZShift(m_zshift);
		nGrid->setAllowDiagonals(m_allow_diagonals);

		return nGrid;
	}

	HexGrid::~HexGrid() {
	}

	bool HexGrid::isAccessible(const ModelCoordinate& curpos, const ModelCoordinate& target) {
		int32_t x = target.x-curpos.x;
		int32_t y = target.y-curpos.y;

		if (ABS(x) <= 1 && ABS(y) <= 1) {
			if (y == 0) {
				return true;
			} else if (curpos.y & 1) {
				if (x >= 0) return true;
			} else if (x <= 0) return true;
        }
		return false;
	}

	double HexGrid::getAdjacentCost(const ModelCoordinate& curpos, const ModelCoordinate& target) {
		if (curpos == target) {
			return 0.0;
		}
		return 1.0;
	}

	double HexGrid::getHeuristicCost(const ModelCoordinate& curpos, const ModelCoordinate& target) {
		return static_cast<double>(ABS(target.x - curpos.x) + ABS(target.y - curpos.y));
	}

	const std::string& HexGrid::getType() const {
		static std::string type("hexagonal");
		return type;
	}

	const std::string& HexGrid::getName() const {
		static std::string hexGrid("Hex Grid");
		return hexGrid;
	}

	double HexGrid::getXZigzagOffset(double y) {
		// each uneven row has shifted coordinate of 0.5 horizontally
		// shift has to be gradual on vertical axis
		double ay = ABS(y);
		int32_t i_layer_y = static_cast<int32_t>(ay);
		double offset = ay - static_cast<double>(i_layer_y);
		if ((i_layer_y % 2) == 1) {
			offset = 1 - offset;
		}
		return HEX_TO_EDGE * offset;
	}

	ExactModelCoordinate HexGrid::toMapCoordinates(const ExactModelCoordinate& layer_coords) {
		ExactModelCoordinate tranformed_coords(layer_coords);
		tranformed_coords.x += getXZigzagOffset(layer_coords.y);
		tranformed_coords.y *= VERTICAL_MULTIP;
		ExactModelCoordinate result = m_matrix * tranformed_coords;
		FL_DBG(_log, LMsg("layercoords ") << layer_coords << " converted to map: " << result);
		return result;
	}

	ExactModelCoordinate HexGrid::toExactLayerCoordinates(const ExactModelCoordinate& map_coord) {
		ExactModelCoordinate layer_coords = m_inverse_matrix * map_coord;
		layer_coords.y /= VERTICAL_MULTIP;
		layer_coords.x -= getXZigzagOffset(layer_coords.y);
		FL_DBG(_log, LMsg("mapcoords ") << map_coord << " converted to layer: " << layer_coords);
		return layer_coords;
	}

	ModelCoordinate HexGrid::toLayerCoordinates(const ExactModelCoordinate& map_coord) {
		FL_DBG(_log, LMsg("==============\nConverting map coords ") << map_coord << " to int32_t layer coords...");
		ExactModelCoordinate elc = m_inverse_matrix * map_coord;
		elc.y *= VERTICAL_MULTIP_INV;

		// approximate conversion using squares instead of hexes
		if( static_cast<int32_t>(round(elc.y)) & 1 )
			elc.x -= 0.5;
		ExactModelCoordinate lc = ExactModelCoordinate(round(elc.x), round(elc.y), round(elc.z));

		int32_t x = static_cast<int32_t>(lc.x);
		int32_t y = static_cast<int32_t>(lc.y);
		int32_t z = static_cast<int32_t>(lc.z);

		// distance of given point from our approximation
		// If y uneven dx=-dx and dy=-dy
		double dx,dy;
		if (y & 1) {
			dx = elc.x - lc.x;
			dy = elc.y - lc.y;
		} else {
			dx = lc.x - elc.x;
			dy = lc.y - elc.y;
		}

		// adjustment for cases where our approximation lies beyond the hex edge
		if (ABS(dy) > ((HEX_TO_CORNER - HEX_EDGE_GRADIENT * ABS(dx)) * VERTICAL_MULTIP_INV)) {
			int8_t ddx, ddy;
			if (dx>0) ddx = -1;
			else ddx = 0;

			if (dy>0) ddy = -1;
			else ddy = 1;

			if (y & 1) {
				ddx = -ddx;
				ddy = -ddy;
			}

			x += ddx;
			y += ddy;
		}

		return ModelCoordinate(x,y,z);
	}

	void HexGrid::getVertices(std::vector<ExactModelCoordinate>& vtx, const ModelCoordinate& cell) {
		FL_DBG(_log, LMsg("===============\ngetting vertices for ") << cell);
		vtx.clear();
		double x = static_cast<double>(cell.x);
		double y = static_cast<double>(cell.y);
		double horiz_shift = 0;
		if (cell.y % 2 != 0) {
			horiz_shift = HEX_TO_EDGE;
			FL_DBG(_log, "on uneven row");
		}
		double tx, ty;

		#define ADD_PT(_x, _y) vtx.push_back(ExactModelCoordinate(_x, _y));
		// FL_DBG(_log, LMsg("Added point ") << _x << ", " << _y)
		ty = y - VERTICAL_MULTIP_INV * HEX_EDGE_HALF;
		tx = x - HEX_TO_EDGE - getXZigzagOffset(ty) + horiz_shift;
		ADD_PT(tx, ty);

		ty = y - VERTICAL_MULTIP_INV * HEX_TO_CORNER;
		tx = x - getXZigzagOffset(ty) + horiz_shift;
		ADD_PT(tx, ty);

		ty = y - VERTICAL_MULTIP_INV * HEX_EDGE_HALF;
		tx = x + HEX_TO_EDGE - getXZigzagOffset(ty) + horiz_shift;
		ADD_PT(tx, ty);

		ty = y + VERTICAL_MULTIP_INV * HEX_EDGE_HALF;
		tx = x + HEX_TO_EDGE - getXZigzagOffset(ty) + horiz_shift;
		ADD_PT(tx, ty);

		ty = y + VERTICAL_MULTIP_INV * HEX_TO_CORNER;
		tx = x - getXZigzagOffset(ty) + horiz_shift;
		ADD_PT(tx, ty);

		ty = y + VERTICAL_MULTIP_INV * HEX_EDGE_HALF;
		tx = x - HEX_TO_EDGE - getXZigzagOffset(ty) + horiz_shift;
		ADD_PT(tx, ty);
	}

	std::vector<ModelCoordinate> HexGrid::toMultiCoordinates(const ModelCoordinate& position, const std::vector<ModelCoordinate>& orig, bool reverse) {
		std::vector<ModelCoordinate> coords;
		std::vector<ModelCoordinate>::const_iterator it = orig.begin();
		if (reverse) {
			for (; it != orig.end(); ++it) {
				ModelCoordinate mc = position;
				if (mc.y % 2 != 0) {
					mc.x -= (*it).x;
					mc.y -= (*it).y;
					if (mc.y % 2 == 0) {
						mc.x -= 1;
					}
				} else {
					mc.x -= (*it).x;
					mc.y -= (*it).y;
				}
				coords.push_back(mc);
			}
		} else {
			for (; it != orig.end(); ++it) {
				ModelCoordinate mc = position;
				if (mc.y % 2 != 0) {
					mc.x += (*it).x;
					mc.y += (*it).y;
					if (mc.y % 2 == 0) {
						mc.x += 1;
					}
				} else {
					mc.x += (*it).x;
					mc.y += (*it).y;
				}
				coords.push_back(mc);
			}
		}
		return coords;
	}

	std::vector<ModelCoordinate> HexGrid::getCoordinatesInLine(const ModelCoordinate& start, const ModelCoordinate& end) {
		std::vector<ModelCoordinate> coords;
		int32_t doubleDeltaX = 2*(end.x - start.x) + ABS(end.y % 2) - ABS(start.y % 2);
		int32_t deltaX = (end.x - start.x) + ABS(end.y % 2) - ABS(start.y % 2);
		int32_t deltaY = end.y - start.y;

		//int8_t signX = (deltaX >= 0) ? 1 : -1;
		//int8_t signY = (deltaY >= 0) ? 1 : -1;
		int8_t signX = (start.x < end.x) ? 1 : -1;
		int8_t signY = (start.y < end.y) ? 1 : -1;
		ModelCoordinate current(start);
		coords.push_back(current);
		int32_t err = 0;
		if (ABS(deltaY) < ABS(doubleDeltaX)) {
			int32_t errX = 3 * ABS(doubleDeltaX);
			int32_t errY = 3 * ABS(deltaY);
			while (current.x != end.x || current.y != end.y) {
				err += errY;
				if (err > ABS(doubleDeltaX)) {
					if (signX == -1) {
						if (signY == -1) {
							// down left
							if (current.y % 2 == 0 && current.x != end.x) {
								current.x -= 1;
							}
							current.y -= 1;
						} else {
							// up left
							if (current.y % 2 == 0 && current.x != end.x) {
								current.x -= 1;
							}
							current.y += 1;
						}
					} else {
						if (signY == -1) {
							// down right
							if (current.y % 2 != 0 && current.x != end.x) {
								current.x += 1;
							}
							current.y -= 1;
						} else {
							// up right
							if (current.y % 2 != 0 && current.x != end.x) {
								current.x += 1;
							}
							current.y += 1;
						}
					}
					err -= errX;
				} else {
					if (signX == -1) {
						// left
						current.x -= 1;
					} else {
						// right
						current.x += 1;
					}
					err += errY;
				}
				coords.push_back(current);
			}
		} else {
			int32_t errX = ABS(doubleDeltaX);
			int32_t errY = ABS(deltaY);
			while (current.x != end.x || current.y != end.y) {
				err += errX;
				if (err > 0) {
					if (signX == -1) {
						if (signY == -1) {
							// down left
							if (current.y % 2 == 0 && current.x != end.x) {
								current.x -= 1;
							}
							current.y -= 1;
						} else {
							// up left
							if (current.y % 2 == 0 && current.x != end.x) {
								current.x -= 1;
							}
							current.y += 1;
						}
					} else if (signX == 1) {
						if (signY == -1) {
							// down right
							if (current.y % 2 != 0 && current.x != end.x) {
								current.x += 1;
							}
							current.y -= 1;
						} else {
							// up right
							if (current.y % 2 != 0 && current.x != end.x) {
								current.x += 1;
							}
							current.y += 1;
						}
					}
					err -= errY;
				} else {
					signX = -signX;
					if (signX == -1) {
						if (signY == -1) {
							// down left
							if (current.y % 2 == 0 && current.x != end.x) {
								current.x -= 1;
							}
							current.y -= 1;
						} else {
							// up left
							if (current.y % 2 == 0 && current.x != end.x) {
								current.x -= 1;
							}
							current.y += 1;
						}
					} else if (signX == 1) {
						if (signY == -1) {
							// down right
							if (current.y % 2 != 0 && current.x != end.x) {
								current.x += 1;
							}
							current.y -= 1;
						} else {
							// up right
							if (current.y % 2 != 0 && current.x != end.x) {
								current.x += 1;
							}
							current.y += 1;
						}
					}
					signX = -signX;
					err += errY;
				}
				coords.push_back(current);
			}
		}
		return coords;
	}
}
