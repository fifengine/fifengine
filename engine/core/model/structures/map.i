%module fife
%{
#include "model/structures/map.h"
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

	%feature("director") MapChangeListener;
	class MapChangeListener {
	public:
		virtual ~MapChangeListener() {};
		virtual void onMapChanged(Map* map, std::vector<Layer*>& changedLayers) = 0;
		virtual void onLayerCreate(Map* map, Layer* layer) = 0;
		virtual void onLayerDelete(Map* map, Layer* layer) = 0;
	};

	class Map : public ResourceClass {
		public:

			Map(const std::string& identifier, TimeProvider* tp_master=NULL);
			~Map();

			const std::string& getId();

			Layer* createLayer(const std::string& identifier, CellGrid* grid);
			void deleteLayer(Layer* index);

			std::list<Layer*> getLayers() const;
			Layer* getLayer(const std::string& id);

			size_t getNumLayers() const;
			void deleteLayers();
			
			void setTimeMultiplier(float multip);
			double getTimeMultiplier() const;
			
			void addChangeListener(MapChangeListener* listener);
			void removeChangeListener(MapChangeListener* listener);
			bool isChanged();
			std::vector<Layer*>& getChangedLayers();
	};
}
