%module fife
%{
#include "model/structures/location.h"
%}

namespace FIFE {

	class Map;
	class Layer;
	class NotSet;
	class ModelCoordinate;
	class ExactModelCoordinate;

	// for some reason swig ignores operator= and gives warning. Ignore this for now
	%ignore Location::operator=;

	class Location {
	public:
		Location();
		Location(const Location& loc);
		Location(Layer* layer);
		~Location();
		void reset();
		Location& operator=(const Location& rhs) const;
		inline bool operator==(const Location& loc) const;
		
		Map* getMap() const;
		void setLayer(Layer* layer);
		Layer* getLayer() const;
		
		void setExactLayerCoordinates(const ExactModelCoordinate& coordinates);
		void setLayerCoordinates(const ModelCoordinate& coordinates);
		void setMapCoordinates(const ExactModelCoordinate& coordinates);
		
		ExactModelCoordinate& getExactLayerCoordinatesRef();
		ExactModelCoordinate getExactLayerCoordinates() const;
		ExactModelCoordinate getExactLayerCoordinates(const Layer* layer) const;
		
		ModelCoordinate getLayerCoordinates() const;
		ModelCoordinate getLayerCoordinates(const Layer* layer) const;
		
		ExactModelCoordinate getMapCoordinates() const;
		
		bool isValid() const;
		double getCellOffsetDistance() const;
		
		double getMapDistanceTo(const Location& location) const;
		double getLayerDistanceTo(const Location& location) const;
	};
	std::ostream& operator<<(std::ostream& os, const Location& l);
}
