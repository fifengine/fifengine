// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

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
			Layer* getLayer();
			void createTransition(Layer* layer, const ModelCoordinate& mc);
			void deleteTransition();

			void addChangeListener(CellChangeListener* listener);
			void removeChangeListener(CellChangeListener* listener);
			void addDeleteListener(CellDeleteListener* listener);
			void removeDeleteListener(CellDeleteListener* listener);
	};
}

%template(InstanceSet) std::set<FIFE::Instance*>;
%template(CellSet) std::set<FIFE::Cell*>;
%template(CellVector) std::vector<FIFE::Cell*>;