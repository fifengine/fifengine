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

	class Location: public FifeClass {
	public:
		Location();
		Location(const Location& loc);
		Location(Layer* layer);
		~Location();
		void reset();
		Location& operator=(const Location& rhs) const;
		bool operator==(const Location& loc) const;
		
		Map* getMap() const;
		void setLayer(Layer* layer);
		Layer* getLayer() const;
		
		void setExactLayerCoordinates(const ExactModelCoordinate& coordinates) throw(NotSet);
		void setLayerCoordinates(const ModelCoordinate& coordinates) throw(NotSet);
		void setMapCoordinates(const ExactModelCoordinate& coordinates);
		
		ExactModelCoordinate& getExactLayerCoordinatesRef();
		ExactModelCoordinate getExactLayerCoordinates() const throw(NotSet);
		ExactModelCoordinate getExactLayerCoordinates(const Layer* layer) const throw(NotSet);
		
		ModelCoordinate getLayerCoordinates() const throw(NotSet);
		ModelCoordinate getLayerCoordinates(const Layer* layer) const throw(NotSet);
		
		ExactModelCoordinate getMapCoordinates() const;
		
		bool isValid() const;
		double getCellOffsetDistance() const;
	};
	std::ostream& operator<<(std::ostream& os, const Location& l);
}
