%module fife
%{
#include "model/structures/layer.h"
%}

%include "std_vector.i"

%include "model/metamodel/modelcoords.i"
%include "model/metamodel/grids/cellgrids.i"
%include "util/structures/utilstructures.i"
%include "util/base/utilbase.i"

namespace FIFE {

	class Map;
	class Selection;
	class Instance;
	class Object;
	class CellGrid;
	
	enum PathingStrategy {
		CELL_EDGES_ONLY,
		CELL_EDGES_AND_DIAGONALS,
		FREEFORM
	};	

	%feature("director") LayerChangeListener;
	class LayerChangeListener {
	public:
		virtual ~LayerChangeListener() {};
		virtual void onLayerChanged(Layer* layer, std::vector<Instance*>& changedInstances) = 0;
		virtual void onInstanceCreate(Layer* layer, Instance* instance) = 0;
		virtual void onInstanceDelete(Layer* layer, Instance* instance) = 0;
	};
	

	class Layer : public AttributedClass {
		public:
			Layer(const std::string& identifier, Map* map, CellGrid* geometry);
			~Layer();

			CellGrid* getCellGrid() const { return m_grid; }

			Map* getMap();
			bool hasInstances() const;
			Instance* createInstance(Object* object, const ModelCoordinate& p, const std::string& id="");
			Instance* createInstance(Object* object, const ExactModelCoordinate& p, const std::string& id="");
			void deleteInstance(Instance* object);

			const std::vector<Instance*>& getInstances();
			std::vector<Instance*> getInstances(const std::string& field, const std::string& value);

			void setInstancesVisible(bool vis);
			bool cellContainsBlockingInstance(const ModelCoordinate& cellCoordinate);
			void toggleInstancesVisible();
			bool areInstancesVisible() const;

			void setPathingStrategy(PathingStrategy strategy);
			PathingStrategy getPathingStrategy();
			
			void addChangeListener(LayerChangeListener* listener);
			void removeChangeListener(LayerChangeListener* listener);
			bool isChanged();
			std::vector<Instance*>& getChangedInstances();
	};
}
