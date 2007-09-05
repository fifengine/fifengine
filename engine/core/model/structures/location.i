%module model
%{
#include "model/structures/location.h"
%}

namespace FIFE {

	class Elevation;
	class Layer;
	class NotSet;
	class Point;
	class DoublePoint;

	class Location {
	public:
		Location();
		Location(const Location& loc);
		~Location();
		void reset();
		Location& operator=(const Location& rhs) const;
		bool operator==(const Location& loc) const;
		Elevation* getElevation() const;
		void setLayer(Layer* layer);
		Layer* getLayer() const;
		void setExactLayerCoordinates(const DoublePoint& coordinates) throw(NotSet);
		void setLayerCoordinates(const Point& coordinates) throw(NotSet);
		void setElevationCoordinates(const DoublePoint& coordinates);
		DoublePoint getExactLayerCoordinates() const throw(NotSet);
		DoublePoint getExactLayerCoordinates(const Layer* layer) const throw(NotSet);
		Point getLayerCoordinates() const throw(NotSet);
		Point getLayerCoordinates(const Layer* layer) const throw(NotSet);
		DoublePoint getElevationCoordinates() const;
		bool isValid() const;
		double getCellOffsetDistance() const;
	};
}
