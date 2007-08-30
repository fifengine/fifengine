%module model
%{
#include "model/structures/elevation.h"
%}

%include "std_list.i"

%include "util/point.i"
%include "util/attributedclass.i"

namespace FIFE {
  class Layer;
}

namespace std {
  %template(LayerList) list<FIFE::Layer*>;
}

namespace FIFE {

	class Map;
	class Geometry;

	class Elevation : public AttributedClass {
		public:
			Elevation(Map* map=NULL);
			~Elevation();

			Map* getMap();

			Layer* addLayer(CellGrid* grid);
			void removeLayer(Layer* index);

			template<typename T>
			std::list<Layer*> getLayers(const std::string& field, const T& value) const;
			%template(getLayersByBool) getLayers<bool>;
			%template(getLayersByInt) getLayers<long>;
			%template(getLayersByPoint) getLayers<Point>;
			%template(getLayersByRect) getLayers<Rect>;
			%template(getLayersByString) getLayers<std::string>;

			size_t getNumLayers() const;
			void clearLayers();

			void update();
	};
}
