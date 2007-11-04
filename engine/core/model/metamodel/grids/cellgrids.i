%module fife
%{
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/grids/hexgrid.h"
#include "model/metamodel/grids/squaregrid.h"
%}

%include "model/metamodel/modelcoords.i"

namespace std {
	%template(ModelCoordinateVector) vector<FIFE::ModelCoordinate>;
}

namespace FIFE {
	class CellGrid {
	public:
		CellGrid();
		virtual ~CellGrid();
		void getAccessibleCoordinates(const ModelCoordinate& curpos, std::vector<ModelCoordinate>& coordinates);
		virtual const std::string& getType() const = 0;
		virtual const std::string& getName() const = 0;
		virtual bool isAccessible(const ModelCoordinate& curpos, const ModelCoordinate& target) = 0;
		virtual float getAdjacentCost(const ModelCoordinate& curpos, const ModelCoordinate& target) = 0;
		virtual unsigned int getCellSideCount() const = 0;
		ExactModelCoordinate toElevationCoordinates(const ModelCoordinate& layer_coords);
		virtual ExactModelCoordinate toElevationCoordinates(const ExactModelCoordinate& layer_coords) = 0;
		virtual ModelCoordinate toLayerCoordinates(const ExactModelCoordinate& elevation_coord) = 0;
		virtual ExactModelCoordinate toExactLayerCoordinates(const ExactModelCoordinate& elevation_coord) = 0;
		virtual void getVertices(std::vector<ExactModelCoordinate>& vtx, const ModelCoordinate& cell) = 0;
		void setXShift(const double& xshift);
		const double getXShift() const;
		void setYShift(const double yshift);
		const double getYShift() const;
		void setXScale(const double scale);
		void setYScale(const double scale);
		const double getXScale() const;
		const double getYScale() const;
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
		float getAdjacentCost(const ModelCoordinate& curpos, const ModelCoordinate& target);
		unsigned int getCellSideCount() const { return 6; }
		ExactModelCoordinate toElevationCoordinates(const ExactModelCoordinate& layer_coords);
		ModelCoordinate toLayerCoordinates(const ExactModelCoordinate& elevation_coord);
		ExactModelCoordinate toExactLayerCoordinates(const ExactModelCoordinate& elevation_coord);
		void getVertices(std::vector<ExactModelCoordinate>& vtx, const ModelCoordinate& cell);
	};

	class SquareGrid: public CellGrid {
	public:
		SquareGrid(bool diagonals_accessible=false);
		virtual ~SquareGrid();

		const std::string& getType() const;
		const std::string& getName() const;
		bool isAccessible(const ModelCoordinate& curpos, const ModelCoordinate& target);
		float getAdjacentCost(const ModelCoordinate& curpos, const ModelCoordinate& target);
		unsigned int getCellSideCount() const { return 4; }
		ExactModelCoordinate toElevationCoordinates(const ExactModelCoordinate& layer_coords);
		ModelCoordinate toLayerCoordinates(const ExactModelCoordinate& elevation_coord);
		ExactModelCoordinate toExactLayerCoordinates(const ExactModelCoordinate& elevation_coord);
		void getVertices(std::vector<ExactModelCoordinate>& vtx, const ModelCoordinate& cell);
	};
}
