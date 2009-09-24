/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

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
	

	class Layer : public ResourceClass {
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

			const std::vector<Instance*>& getInstances() const;
			std::vector<Instance*> getInstances(const std::string& identifier);
			std::vector<Instance*> getInstancesAt(Location& loc, bool use_exactcoordinates=false);
			Instance* getInstance(const std::string& id);

			void setInstancesVisible(bool vis);
			void setLayerTransparency(uint8_t transparency);
			uint8_t getLayerTransparency();
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
