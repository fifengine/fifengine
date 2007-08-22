%module model
%{
#include "model/metamodel/geometries/geometry.h"
#include "model/metamodel/geometries/hexgeometry.h"
#include "model/metamodel/geometries/squaregeometry.h"
#include "util/point.h"
%}

%include "util/point.h"

namespace std {
	%template(PointVector) vector<FIFE::Point>;
}

namespace FIFE { namespace model {
	class Geometry {
	public:
		Geometry();
		virtual ~Geometry();

		void getAccessibleCoordinates(const Point& curpos, std::vector<Point>& coordinates);
		virtual const std::string getName() = 0;
		virtual const bool isAccessible(const Point& curpos, const Point& target) = 0;
		virtual const float getAdjacentCost(const Point& curpos, const Point& target) = 0;
	};

	class HexGeometry: public Geometry {
	public:
		HexGeometry();
		virtual ~HexGeometry();
		virtual const std::string getName();
		virtual const bool isAccessible(const Point& curpos, const Point& target);
		virtual const float getAdjacentCost(const Point& curpos, const Point& target);
	};

	class SquareGeometry: public Geometry {
	public:
		SquareGeometry(bool diagonals_accessible=false);
		virtual ~SquareGeometry();
		virtual const std::string getName();
		virtual const bool isAccessible(const Point& curpos, const Point& target);
		virtual const float getAdjacentCost(const Point& curpos, const Point& target);
	};
}}
