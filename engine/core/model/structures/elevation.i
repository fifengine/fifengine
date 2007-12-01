%module fife
%{
#include "model/structures/elevation.h"
%}

%include "std_list.i"

%include "model/metamodel/modelcoords.i"
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
			Elevation(const std::string& identifier, Map* map=NULL);
			~Elevation();

			Map* getMap();

			Layer* createLayer(const std::string& identifier, CellGrid* grid);
			void deleteLayer(Layer* index);

			std::list<Layer*> getLayers() const;
			std::list<Layer*> getLayers(const std::string& field, const std::string& value) const;

			size_t getNumLayers() const;
			void deleteLayers();

			void update();
	};
}
