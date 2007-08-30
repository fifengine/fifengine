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
		virtual const std::string& getName() = 0;
		virtual bool isAccessible(const Point& curpos, const Point& target) = 0;
		virtual float getAdjacentCost(const Point& curpos, const Point& target) = 0;
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
		virtual const std::string& getName();
		virtual bool isAccessible(const Point& curpos, const Point& target);
		virtual float getAdjacentCost(const Point& curpos, const Point& target);
	};

	class SquareGrid: public CellGrid {
	public:
		SquareGrid(bool diagonals_accessible=false);
		virtual ~SquareGrid();
		virtual const std::string& getName();
		virtual bool isAccessible(const Point& curpos, const Point& target);
		virtual float getAdjacentCost(const Point& curpos, const Point& target);
	};
}
