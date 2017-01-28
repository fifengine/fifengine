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
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/grids/hexgrid.h"
#include "model/metamodel/grids/squaregrid.h"
%}

%include "model/metamodel/modelcoords.i"

namespace std {
	%template(ModelCoordinateVector) vector<FIFE::ModelCoordinate>;
	%template(ExactModelCoordinateVector) vector<FIFE::ExactModelCoordinate>;
}

namespace FIFE {
	%apply std::vector<ExactModelCoordinate> &OUTPUT { std::vector<ExactModelCoordinate>& vtx };
	class CellGrid {
	public:
		CellGrid();
		virtual ~CellGrid();
		void getAccessibleCoordinates(const ModelCoordinate& curpos, std::vector<ModelCoordinate>& coordinates);
		virtual const std::string& getType() const = 0;
		virtual const std::string& getName() const = 0;
		virtual bool isAccessible(const ModelCoordinate& curpos, const ModelCoordinate& target) = 0;
		virtual double getAdjacentCost(const ModelCoordinate& curpos, const ModelCoordinate& target) = 0;
		virtual double getHeuristicCost(const ModelCoordinate& curpos, const ModelCoordinate& target) = 0;
		virtual uint32_t getCellSideCount() const = 0;
		ExactModelCoordinate toMapCoordinates(const ModelCoordinate& layer_coords);
		virtual ExactModelCoordinate toMapCoordinates(const ExactModelCoordinate& layer_coords) = 0;
		virtual ModelCoordinate toLayerCoordinates(const ExactModelCoordinate& map_coord) = 0;
		virtual ExactModelCoordinate toExactLayerCoordinates(const ExactModelCoordinate& map_coord) = 0;
		virtual ModelCoordinate toLayerCoordinatesFromExactLayerCoordinates(const ExactModelCoordinate& exact_layer_coords) = 0;
		virtual void getVertices(std::vector<ExactModelCoordinate>& vtx, const ModelCoordinate& cell) = 0;
		virtual std::vector<ModelCoordinate> getCoordinatesInLine(const ModelCoordinate& start, const ModelCoordinate& end) = 0;
		void setXShift(const double& xshift);
		const double getXShift() const;
		void setYShift(const double yshift);
		const double getYShift() const;
		void setZShift(const double zshift); 
		const double getZShift() const;
		void setXScale(const double scale);
		void setYScale(const double scale);
		void setZScale(const double scale);
		const double getXScale() const;
		const double getYScale() const;
		const double getZScale() const;
		void setRotation(const double rotation);
		const double getRotation() const;
	};

	class HexGrid: public CellGrid {
	public:
		HexGrid();
		virtual ~HexGrid();

		bool isAccessible(const ModelCoordinate& curpos, const ModelCoordinate& target);
		const std::string& getType() const;
		const std::string& getName() const;
		double getAdjacentCost(const ModelCoordinate& curpos, const ModelCoordinate& target);
		uint32_t getCellSideCount() const { return 6; }
		ExactModelCoordinate toMapCoordinates(const ExactModelCoordinate& layer_coords);
		ModelCoordinate toLayerCoordinates(const ExactModelCoordinate& map_coord);
		ExactModelCoordinate toExactLayerCoordinates(const ExactModelCoordinate& map_coord);
		ModelCoordinate toLayerCoordinatesFromExactLayerCoordinates(const ExactModelCoordinate& exact_layer_coords);
		void getVertices(std::vector<ExactModelCoordinate>& vtx, const ModelCoordinate& cell);
		std::vector<ModelCoordinate> getCoordinatesInLine(const ModelCoordinate& start, const ModelCoordinate& end);
	};

	class SquareGrid: public CellGrid {
	public:
		SquareGrid();
		virtual ~SquareGrid();

		const std::string& getType() const;
		const std::string& getName() const;
		bool isAccessible(const ModelCoordinate& curpos, const ModelCoordinate& target);
		double getAdjacentCost(const ModelCoordinate& curpos, const ModelCoordinate& target);
		uint32_t getCellSideCount() const { return 4; }
		ExactModelCoordinate toMapCoordinates(const ExactModelCoordinate& layer_coords);
		ModelCoordinate toLayerCoordinates(const ExactModelCoordinate& map_coord);
		ExactModelCoordinate toExactLayerCoordinates(const ExactModelCoordinate& map_coord);
		ModelCoordinate toLayerCoordinatesFromExactLayerCoordinates(const ExactModelCoordinate& exact_layer_coords);
		void getVertices(std::vector<ExactModelCoordinate>& vtx, const ModelCoordinate& cell);
		std::vector<ModelCoordinate> getCoordinatesInLine(const ModelCoordinate& start, const ModelCoordinate& end);
	};
}
