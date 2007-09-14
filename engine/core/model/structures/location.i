%module model
%{
#include "model/structures/location.h"
%}

namespace FIFE {

	class Elevation;
	class Layer;
	class NotSet;
	class ModelCoordinate;
	class ExactModelCoordinate;

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
		void setExactLayerCoordinates(const ExactModelCoordinate& coordinates) throw(NotSet);
		void setLayerCoordinates(const ModelCoordinate& coordinates) throw(NotSet);
		void setElevationCoordinates(const ExactModelCoordinate& coordinates);
		ExactModelCoordinate getExactLayerCoordinates() const throw(NotSet);
		ExactModelCoordinate getExactLayerCoordinates(const Layer* layer) const throw(NotSet);
		ModelCoordinate getLayerCoordinates() const throw(NotSet);
		ModelCoordinate getLayerCoordinates(const Layer* layer) const throw(NotSet);
		ExactModelCoordinate getElevationCoordinates() const;
		bool isValid() const;
		double getCellOffsetDistance() const;
	};
}
