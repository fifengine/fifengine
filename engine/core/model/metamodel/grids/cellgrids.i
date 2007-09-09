%module model
%{
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/grids/hexgrid.h"
#include "model/metamodel/grids/squaregrid.h"
%}

%include "util/point.i"

namespace std {
	%template(PointVector) vector<FIFE::Point>;
}

namespace FIFE {
	class CellGrid {
	public:
		CellGrid();
		virtual ~CellGrid();
		void getAccessibleCoordinates(const Point& curpos, std::vector<Point>& coordinates);
		virtual const std::string& getName() const = 0;
		virtual bool isAccessible(const Point& curpos, const Point& target) = 0;
		virtual float getAdjacentCost(const Point& curpos, const Point& target) = 0;
		virtual DoublePoint getOffset(const Point& curpos, const Point& target, double distance) = 0;
		virtual unsigned int getCellSideCount() const = 0;
		DoublePoint toElevationCoordinates(const Point& layer_coords);
		virtual DoublePoint toElevationCoordinates(const DoublePoint& layer_coords) = 0;
		virtual Point toLayerCoordinates(const DoublePoint& elevation_coord) = 0;
		virtual DoublePoint toExactLayerCoordinates(const DoublePoint& elevation_coord) = 0;
		virtual void getVertices(std::vector<DoublePoint>& vtx, const Point& cell) = 0;
		void setXShift(const double& xshift);
		const double getXShift() const;
		void setYShift(const double yshift);
		const double getYShift() const;
		void setScale(const double scale);
		const double getScale() const;
		void setRotation(const double rotation);
		const double getRotation() const;
	};

	class HexGrid: public CellGrid {
	public:
		HexGrid();
		virtual ~HexGrid();

		bool isAccessible(const Point& curpos, const Point& target);
		const std::string& getName() const;
		float getAdjacentCost(const Point& curpos, const Point& target);
		unsigned int getCellSideCount() const { return 6; }
		DoublePoint getOffset(const Point& curpos, const Point& target, double distance);
		DoublePoint toElevationCoordinates(const DoublePoint& layer_coords);
		Point toLayerCoordinates(const DoublePoint& elevation_coord);
		DoublePoint toExactLayerCoordinates(const DoublePoint& elevation_coord);
		void getVertices(std::vector<DoublePoint>& vtx, const Point& cell);
	};

	class SquareGrid: public CellGrid {
	public:
		SquareGrid(bool diagonals_accessible=false);
		virtual ~SquareGrid();

		const std::string& getName() const;
		bool isAccessible(const Point& curpos, const Point& target);
		float getAdjacentCost(const Point& curpos, const Point& target);
		unsigned int getCellSideCount() const { return 4; }
		DoublePoint getOffset(const Point& curpos, const Point& target, double distance);
		DoublePoint toElevationCoordinates(const DoublePoint& layer_coords);
		Point toLayerCoordinates(const DoublePoint& elevation_coord);
		DoublePoint toExactLayerCoordinates(const DoublePoint& elevation_coord);
		void getVertices(std::vector<DoublePoint>& vtx, const Point& cell);
	};
}
