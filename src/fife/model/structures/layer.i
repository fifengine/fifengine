// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "model/structures/layer.h"
%}

%include "model/metamodel/modelcoords.i"
%include "model/metamodel/grids/cellgrids.i"
%include "util/structures/utilstructures.i"
%include "util/base/utilbase.i"

namespace FIFE {

	class Map;
	class Instance;
	class Object;
	class CellGrid;
	class CellCache;
	
	enum PathingStrategy {
		CELL_EDGES_ONLY,
		CELL_EDGES_AND_DIAGONALS
	};	

	enum SortingStrategy {
		SORTING_CAMERA,
		SORTING_LOCATION,
		SORTING_CAMERA_AND_LOCATION
	};

	%feature("director") LayerChangeListener;
	class LayerChangeListener {
	public:
		virtual ~LayerChangeListener() {};
		virtual void onLayerChanged(Layer* layer, std::vector<Instance*>& changedInstances) = 0;
		virtual void onInstanceCreate(Layer* layer, Instance* instance) = 0;
		virtual void onInstanceDelete(Layer* layer, Instance* instance) = 0;
	};
	

	class Layer : public FifeClass {
		public:
			Layer(const std::string& identifier, Map* map, CellGrid* geometry);
			~Layer();

			const std::string& getId() const;
			void setId(const std::string& id);

			CellGrid* getCellGrid() const;
			void setCellGrid(CellGrid* grid);

			Map* getMap();
			bool hasInstances() const;
			Instance* createInstance(Object* object, const ModelCoordinate& p, const std::string& id="");
			Instance* createInstance(Object* object, const ExactModelCoordinate& p, const std::string& id="");
			bool addInstance(Instance* instance, const ExactModelCoordinate& p);
			void deleteInstance(Instance* object);
			void removeInstance(Instance* object);

			const std::vector<Instance*>& getInstances() const;
			std::vector<Instance*> getInstances(const std::string& identifier);
			std::vector<Instance*> getInstancesAt(Location& loc, bool use_exactcoordinates=false);
			std::list<Instance*> getInstancesIn(Rect& rec);
			std::vector<Instance*> getInstancesInLine(const ModelCoordinate& pt1, const ModelCoordinate& pt2);
			std::vector<Instance*> getInstancesInCircle(const ModelCoordinate& center, uint16_t radius);
			std::vector<Instance*> getInstancesInCircleSegment(const ModelCoordinate& center, uint16_t radius, int32_t sangle, int32_t eangle);
			Instance* getInstance(const std::string& id);

			void setInstancesVisible(bool vis);
			void setLayerTransparency(uint8_t transparency);
			uint8_t getLayerTransparency();
			void getMinMaxCoordinates(ModelCoordinate& min, ModelCoordinate& max, const Layer* layer = 0) const;
			bool cellContainsBlockingInstance(const ModelCoordinate& cellCoordinate);
			std::vector<Instance*> getBlockingInstances(const ModelCoordinate& cellCoordinate);
			void toggleInstancesVisible();
			bool areInstancesVisible() const;

			void setPathingStrategy(PathingStrategy strategy);
			PathingStrategy getPathingStrategy();
			
			void setSortingStrategy(SortingStrategy strategy);
			SortingStrategy getSortingStrategy() const;

			void setWalkable(bool walkable);
			bool isWalkable();
			
			void setInteract(bool interact, const std::string& id);
			bool isInteract();
			const std::string& getWalkableId();
			
			void addInteractLayer(Layer* layer);
			const std::vector<Layer*>& getInteractLayers();
			void removeInteractLayer(Layer* layer);

			void createCellCache();
			CellCache* getCellCache();
			void destroyCellCache();
			
			void addChangeListener(LayerChangeListener* listener);
			void removeChangeListener(LayerChangeListener* listener);
			bool isChanged();
			std::vector<Instance*>& getChangedInstances();

			void setStatic(bool stati);
			bool isStatic();
	};
}