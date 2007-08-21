%module model
%{
#include "model/metamodel/geometries/geometry.h"
#include "model/metamodel/geometries/hexgeometry.h"
#include "model/metamodel/geometries/squaregeometry.h"
%}


namespace FIFE { namespace model {
	class Geometry {
	public:
		Geometry();
		virtual ~Geometry();
	};

	class HexGeometry: public Geometry {
	public:
		HexGeometry();
		virtual ~HexGeometry();
	};

	class SquareGeometry: public Geometry {
	public:
		SquareGeometry();
		virtual ~SquareGeometry();
	};
}}
