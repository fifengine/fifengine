/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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
#include "model/structures/cell.h"
%}

namespace FIFE {

	class Instance;
	class Layer;
	class Cell;
	
	enum CellType {
		CTYPE_NO_BLOCKER = 0,
		CTYPE_STATIC_BLOCKER = 1,
		CTYPE_DYNAMIC_BLOCKER = 2,
		CTYPE_CELL_NO_BLOCKER = 3,
		CTYPE_CELL_BLOCKER = 4
	};
	typedef uint8_t CellTypeInfo;
	
	enum CellVisualType {
		CELLV_CONCEALED = 0,
		CELLV_REVEALED,
		CELLV_MASKED
	};
	typedef uint8_t CellVisualEffect;

	%feature("director") CellChangeListener;
	class CellChangeListener {
	public:
		virtual ~CellChangeListener() {};
		virtual void onInstanceEnteredCell(Cell* cell, Instance* instance) = 0;
		virtual void onInstanceExitedCell(Cell* cell, Instance* instance) = 0;
		virtual void onBlockingChangedCell(Cell* cell, CellTypeInfo type, bool blocks) = 0;
	};

	%feature("director") CellDeleteListener;
	class CellDeleteListener {
	public:
		virtual ~CellDeleteListener() {};
		virtual void onCellDeleted(Cell* cell) = 0;
	};

	class Cell : public FifeClass {
		public:
			Cell(int32_t coordint, ModelCoordinate coordinate, Layer* layer);
			~Cell();

			void addInstances(const std::list<Instance*>& instances);
			void addInstance(Instance* instance);
			void changeInstance(Instance* instance);
			void removeInstance(Instance* instance);

			bool isNeighbor(Cell* cell);
			std::vector<Cell*> getNeighbors();
			void updateCellInfo();
			int32_t getCellId();
			const ModelCoordinate getLayerCoordinates() const;
			bool defaultCost();
			void setCostMultiplier(double multi);
			double getCostMultiplier();
			void resetCostMultiplier();
			bool defaultSpeed();
			void setSpeedMultiplier(double multi);
			double getSpeedMultiplier();
			void resetSpeedMultiplier();
			
			const std::set<Instance*>& getInstances();
			void setCellType(CellTypeInfo type);
			CellTypeInfo getCellType();
			void setFoWType(CellVisualEffect type);
			CellVisualEffect getFoWType();
			Layer* getLayer();
			void createTransition(Layer* layer, const ModelCoordinate& mc);
			void deleteTransition();

			void addChangeListener(CellChangeListener* listener);
			void removeChangeListener(CellChangeListener* listener);
			void addDeleteListener(CellDeleteListener* listener);
			void removeDeleteListener(CellDeleteListener* listener);
	};
}

namespace std {
	%template(InstanceSet) set<FIFE::Instance*>;
	%template(CellSet) set<FIFE::Cell*>;
	%template(CellVector) vector<FIFE::Cell*>;
}
