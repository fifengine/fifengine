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

#ifndef FIFE_MODEL_GRIDS_HEXGRID_H
#define FIFE_MODEL_GRIDS_HEXGRID_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fife_stdint.h"

#include "cellgrid.h"

namespace FIFE {
	class HexGrid: public CellGrid {
	public:
		HexGrid();
		virtual ~HexGrid();

		bool isAccessible(const ModelCoordinate& curpos, const ModelCoordinate& target);
		const std::string& getType() const;
		const std::string& getName() const;
		double getAdjacentCost(const ModelCoordinate& curpos, const ModelCoordinate& target);
		double getHeuristicCost(const ModelCoordinate& curpos, const ModelCoordinate& target);
		uint32_t getCellSideCount() const { return 6; }
		ExactModelCoordinate toMapCoordinates(const ExactModelCoordinate& layer_coords);
		ModelCoordinate toLayerCoordinates(const ExactModelCoordinate& map_coord);
		ExactModelCoordinate toExactLayerCoordinates(const ExactModelCoordinate& map_coord);
		ModelCoordinate toLayerCoordinatesFromExactLayerCoordinates(const ExactModelCoordinate& exact_layer_coords);
		void getVertices(std::vector<ExactModelCoordinate>& vtx, const ModelCoordinate& cell);
		std::vector<ModelCoordinate> toMultiCoordinates(const ModelCoordinate& position, const std::vector<ModelCoordinate>& orig, bool reverse);
		std::vector<ModelCoordinate> getCoordinatesInLine(const ModelCoordinate& start, const ModelCoordinate& end);
		CellGrid* clone();

	private:
		double getXZigzagOffset(double y);
		ModelCoordinate toLayerCoordinatesHelper(const ExactModelCoordinate& coords);
	};
}

#endif
