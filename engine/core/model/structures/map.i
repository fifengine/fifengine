%module fife
%{
#include "model/structures/map.h"
#include "model/structures/map_loader.h"
%}

%include "std_list.i"

%include "util/base/utilbase.i"

namespace FIFE {
  class Layer;
}

namespace std {
  %template(LayerList) list<FIFE::Layer*>;
}

namespace FIFE {

	class Layer;
	class Map;	
	class Dataset;

	%feature("director") MapChangeListener;
	class MapChangeListener {
	public:
		virtual ~MapChangeListener() {};
		virtual void onMapChanged(Map* map, std::vector<Layer*>& changedLayers) = 0;
		virtual void onLayerCreate(Map* map, Layer* layer) = 0;
		virtual void onLayerDelete(Map* map, Layer* layer) = 0;
	};

	class Map : public AttributedClass {
		public:

			Map(const std::string& identifier, TimeProvider* tp_master=NULL);
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
			
			void setTimeMultiplier(float multip);
			double getTimeMultiplier() const;
			
			void addChangeListener(MapChangeListener* listener);
			void removeChangeListener(MapChangeListener* listener);
			bool isChanged();
			std::vector<Layer*>& getChangedLayers();
	};

	%warnfilter(473) MapLoader; // filter out "returning a pointer or reference in a director method is not recommended"
	%feature("director") MapLoader;
	class MapLoader : public ResourceLoader {
	public:
		Map* load(const ResourceLocation& location);
		Map* load(const std::string& filename);
	};
}
