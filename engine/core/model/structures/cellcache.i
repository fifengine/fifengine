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
#include "model/structures/cellcache.h"
%}

namespace FIFE {

	class Cell;
	class Layer;

	class CellCache : public FifeClass {
		public:
			CellCache(Layer* layer);
			~CellCache();
			
			void createCells();
			void forceUpdate();
			void addCell(Cell* cell);
			Cell* createCell(const ModelCoordinate& mc);
			Cell* getCell(const ModelCoordinate& mc);
			void addInteractOnRuntime(Layer* interact);
			void removeInteractOnRuntime(Layer* interact);
			const Rect& getSize();
			void setSize(const Rect& rec);
			uint32_t getWidth();
			uint32_t getHeight();
			int32_t getMaxIndex() const;
			void setMaxNeighborZ(int32_t z);
			int32_t getMaxNeighborZ();

			bool isInCellCache(const Location& location) const;

			std::vector<Cell*> getCellsInLine(const ModelCoordinate& pt1, const ModelCoordinate& pt2, bool blocker = false);
			std::vector<Cell*> getCellsInRect(const Rect& rec);
			std::vector<Cell*> getCellsInCircle(const ModelCoordinate& center, uint16_t radius);
			std::vector<Cell*> getCellsInCircleSegment(const ModelCoordinate& center, uint16_t radius, int32_t sangle, int32_t eangle);

			void registerCost(const std::string& costId, double cost);
			void unregisterCost(const std::string& costId);
			double getCost(const std::string& costId);
			bool existsCost(const std::string& costId);
			std::list<std::string> getCosts();
			void unregisterAllCosts();
			void addCellToCost(const std::string& costId, Cell* cell);
			void addCellsToCost(const std::string& costId, const std::vector<Cell*>& cells);
			void removeCellFromCost(Cell* cell);
			void removeCellFromCost(const std::string& costId, Cell* cell);
			void removeCellsFromCost(const std::string& costId, const std::vector<Cell*>& cells);
			std::vector<Cell*> getCostCells(const std::string& costId);
			std::vector<std::string> getCellCosts(Cell* cell);
			bool existsCostForCell(const std::string& costId, Cell* cell);

			void setDefaultCostMultiplier(double multi);
			double getDefaultCostMultiplier();
			void setDefaultSpeedMultiplier(double multi);
			double getDefaultSpeedMultiplier();

			void addCellToArea(const std::string& id, Cell* cell);
			void addCellsToArea(const std::string& id, const std::vector<Cell*>& cells);
			void removeCellFromArea(const std::string& id, Cell* cell);
			void removeCellsFromArea(const std::string& id, const std::vector<Cell*>& cells);
			void removeArea(const std::string& id);
			bool existsArea(const std::string& id);
			std::vector<std::string> getAreas();
			std::vector<std::string> getCellAreas(Cell* cell);
			std::vector<Cell*> getAreaCells(const std::string& id);
			bool isCellInArea(const std::string& id, Cell* cell);
			void setStaticSize(bool staticSize);
			bool isStaticSize();
	};
}
