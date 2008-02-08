%module fife
%{
#include "model/structures/map.h"
%}

%include "std_list.i"

%include "util/attributedclass.i"

namespace FIFE {
  class Layer;
}

namespace std {
  %template(LayerList) list<FIFE::Layer*>;
}

namespace FIFE {

  class Layer;

  class Dataset;

	class Map : public AttributedClass {
		public:

			Map(const std::string& identifier);
			~Map();

			void addDataset(Dataset* dataset);
			std::list<Dataset*> getDatasets();
			std::list<Dataset*> getDatasetsRec();

			Layer* createLayer(const std::string& identifier, CellGrid* grid);
			void deleteLayer(Layer* index);

			std::list<Layer*> getLayers() const;
			std::list<Layer*> getLayers(const std::string& field, const std::string& value) const;

			size_t getNumLayers() const;
			void deleteLayers();

			void update();
	};
}
