// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_CELLCACHE_H
#define FIFE_CELLCACHE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "cell.h"
#include "layer.h"
#include "model/metamodel/modelcoords.h"
#include "model/metamodel/object.h"
#include "util/base/fifeclass.h"
#include "util/structures/rect.h"

namespace FIFE
{

    /** A Zone is an abstract depiction of a CellCache or of a part of it.
     */
    class FIFE_API Zone
    {
        public:
            /** Constructor
             * @param id A integer value used as identifier. Simple counter values are used.
             */
            explicit Zone(uint32_t id);

            /** Destructor
             */
            ~Zone();

            /** Adds a cell to this zone.
             * @param cell A pointer to cell which should be added.
             */
            void addCell(Cell* cell);

            /** Removes a cell from this zone.
             * @param cell A pointer to cell which should be removed.
             */
            void removeCell(Cell* cell);

            /** Merge two zones to one.
             * @param zone A pointer to the old zone.
             */
            void mergeZone(Zone* zone);

            /** Returns all cells of this zone.
             * @return A const reference to a set that contains all cells of this zone.
             */
            std::set<Cell*> const & getCells() const;

            /** Remove all cells from zone but does not alter the cells.
             */
            void resetCells();

            /** Returns the zone identifier.
             * @return A unsigned integer with the identifier.
             */
            uint32_t getId() const;

            /** Returns the number of cells.
             * @return A unsigned integer with the number of cells.
             */
            uint32_t getCellCount() const;

            /** Returns transistion cells of this zone.
             * @param layer A pointer to the layer which should be the target of the transition. If NULL all
             * transistions be returned.
             * @return A vector which contains the transition cells.
             */
            std::vector<Cell*> getTransitionCells(Layer const * layer = nullptr);

        private:
            //! identifier
            uint32_t m_id;
            //! cells in the zone
            std::set<Cell*> m_cells;
    };

    /** A CellCache is an abstract depiction of one or a few layers
     *	and contains additional information, such as different cost and speed and so on.
     */
    class FIFE_API CellCache : public FifeClass
    {
        public:
            /** Constructor
             * @param layer A pointer to the associated layer.
             */
            explicit CellCache(Layer* layer);

            /** Destructor
             */
            ~CellCache();

            /** Resets the CellCache
             */
            void reset();

            /** Checks the layer size and if the size is different with current size then
                    the cache size is adjusted.
             */
            void resize();

            /** Checks the given size and if the size is different with current size then
                    the cache is adjusted.
             * @param rec A rect that contains the new size in layer coordinates.
             */
            void resize(Rect const & rec);

            /** Creates cells for this CellCache based on the size of the assigned layer.
             */
            void createCells();

            /** Updates all cells.
             */
            void forceUpdate();

            /** Adds cell to this CellCache.
             * @param cell A pointer to cell which should be added.
             */
            void addCell(Cell* cell);

            /** Creates cell on this CellCache.
             * @param mc A const reference to ModelCoordinate where the cell should be created.
             * @return A pointer to the new cell.
             */
            Cell* createCell(ModelCoordinate const & mc);

            /** Returns cell on this coordinate.
             * @param mc A const reference to ModelCoordinate where the cell should be.
             * @return A pointer to the cell or NULL if there is no.
             */
            Cell* getCell(ModelCoordinate const & mc);

            /** Returns all cells of this CellCache.
             * @return A const reference to a two dimensional vector which contain all cells.
             */
            std::vector<std::vector<Cell*>> const & getCells();

            /** Removes cell from CellCache.
             * Removes cell from cost table, special cost and speed,
             * futhermore from areas and narrows.
             * @param cell A pointer to the cell.
             */
            void removeCell(Cell* cell);

            /** Adds a interact layer to the CellCache on runtime and sets all needed layer properties.
             * @param interact A pointer to the interact layer.
             */
            void addInteractOnRuntime(Layer* interact);

            /** Removes a interact layer from the CellCache on runtime and sets all needed layer properties.
             * @param interact A pointer to the interact layer.
             */
            void removeInteractOnRuntime(Layer* interact);

            /** Returns change listener.
             * @return A pointer to the change listener.
             */
            LayerChangeListener* getCellCacheChangeListener();

            /** Returns layer.
             * @return A pointer to the layer this CellCache is based on.
             */
            Layer* getLayer();

            /** Returns CellCache size.
             * @return A const reference to rect that contain the min and max coordinates.
             */
            Rect const & getSize();

            /** Sets CellCache size.
             * @param rec A const reference to a rect that contain new min, max coordinates.
                    rec.x = min.x, rec.w = max.x, rec.y = min.y, rec.h = max.y
             */
            void setSize(Rect const & rec);

            /** Returns width of the CellCache.
             * @return A unsigned integer that is the width.
             */
            uint32_t getWidth() const;

            /** Returns height of the CellCache.
             * @return A unsigned integer that is the height.
             */
            uint32_t getHeight() const;

            /** Checks whether the location is in CellCache range.
             * @param location A const reference to location which should be checked.
             * @return A boolean, true if the location is in range, otherwise false.
             */
            bool isInCellCache(Location const & location) const;

            /** Convertes coordinate to unique identifier.
             * @param coord A const reference to ModelCoordinate which should be converted.
             * @return A integer, the cell identifier.
             */
            int32_t convertCoordToInt(ModelCoordinate const & coord) const;

            /** Convertes unique identifier to coordinate.
             * @param cell A const reference to the integer id which should be converted.
             * @return A ModelCoordinate, contain the cell coordinate.
             */
            ModelCoordinate convertIntToCoord(int32_t cell) const;

            /** Returns the number of cells on this CellCache.
             * @return A integer value, the number of cells.
             */
            int32_t getMaxIndex() const;

            /** Sets maximal z range for neighbors.
             * @param z The maximal z range as int.
             */
            void setMaxNeighborZ(int32_t z);

            /** Gets maximal z range for neighbors. By default disabled with the value -1.
             * @return The maximal z range as int.
             */
            int32_t getMaxNeighborZ() const;

            /** Returns all cells in the line.
             * @param pt1 A const reference to the ModelCoordinate where the line begin.
             * @param pt2 A const reference to the ModelCoordinate where the line end.
             * @param blocker A boolean, true stops when a blocker is found, false ignored blocker.
             * @return A vector that contain the cells.
             */
            std::vector<Cell*> getCellsInLine(
                ModelCoordinate const & pt1, ModelCoordinate const & pt2, bool blocker = false);

            /** Returns all cells in the rect.
             * @param rec A const reference to the Rect which specifies the size.
             * @return A vector that contain the cells.
             */
            std::vector<Cell*> getCellsInRect(Rect const & rec);

            /** Returns all blocking cells in the rect.
             * @param rec A const reference to the Rect which specifies the size.
             * @return A vector that contain the cells.
             */
            std::vector<Cell*> getBlockingCellsInRect(Rect const & rec);

            /** Returns all cells in the circle.
             * @param center A const reference to the ModelCoordinate where the center of the circle is.
             * @param radius A unsigned integer, radius of the circle.
             * @return A vector that contain the cells.
             */
            std::vector<Cell*> getCellsInCircle(ModelCoordinate const & center, uint16_t radius);

            /** Returns all cells in the circle segment.
             * @param center A const reference to the ModelCoordinate where the center of the circle is.
             * @param radius A unsigned integer, radius of the circle.
             * @param sangle A interger, start angle of the segment.
             * @param eangle A interger, end angle of the segment.
             * @return A vector that contain the cells.
             */
            std::vector<Cell*> getCellsInCircleSegment(
                ModelCoordinate const & center, uint16_t radius, int32_t sangle, int32_t eangle);

            /** Adds a cost with the given id and value.
             * @param costId A const reference to a string that refs to the cost id.
             * @param cost A double that contains the cost value. Used as multiplier for default cost.
             */
            void registerCost(std::string const & costId, double cost);

            /** Removes a cost with the given id.
             * @param costId A const reference to a string that refs to the cost id.
             */
            void unregisterCost(std::string const & costId);

            /** Returns the cost value for the given id.
             * @param costId A const reference to a string that refs to the cost id.
             * @return cost value as a double, if cost id can not be found 1.0 is returned.
             */
            double getCost(std::string const & costId);

            /** Returns if the cost for the given id exists.
             * @return True if cost id could be found otherwise false.
             */
            bool existsCost(std::string const & costId);

            /** Returns all registered cost ids.
             * @return A list that contains the cost ids.
             */
            std::list<std::string> getCosts();

            /** Removes all costs.
             */
            void unregisterAllCosts();

            /** Assigns a cell to a cost identifier.
             * @param costId A const reference to the cost identifier.
             * @param cell A pointer to the cell.
             */
            void addCellToCost(std::string const & costId, Cell* cell);

            /** Assigns cells to a cost identifier.
             * @param costId A const reference to the cost identifier.
             * @param cells A const reference to a vector which contains the cells.
             */
            void addCellsToCost(std::string const & costId, std::vector<Cell*> const & cells);

            /** Removes a cell from costs.
             * @param cell A pointer to the cell.
             */
            void removeCellFromCost(Cell* cell);

            /** Removes a cell from a cost identifier.
             * @param costId A const reference to the cost identifier.
             * @param cell A pointer to the cell.
             */
            void removeCellFromCost(std::string const & costId, Cell* cell);

            /** Removes cells from a cost identifier.
             * @param costId A const reference to the cost identifier.
             * @param cells A const reference to a vector which contains the cells.
             */
            void removeCellsFromCost(std::string const & costId, std::vector<Cell*> const & cells);

            /** Returns cells for a cost identifier.
             * @param costId A const reference to the cost identifier.
             * @return A vector which contains the cells.
             */
            std::vector<Cell*> getCostCells(std::string const & costId);

            /** Returns cost identifiers for cell.
             * @param cell A pointer to the cell.
             * @return A vector which contains the cost ids.
             */
            std::vector<std::string> getCellCosts(Cell* cell);

            /** Gets if cell is assigned to cost identifier.
             * @param costId A const reference to the cost identifier.
             * @param cell A pointer to the cell.
             * @return A boolean, true if the cell is assigned to the cost identifier, otherwise false.
             */
            bool existsCostForCell(std::string const & costId, Cell* cell);

            /** Returns cost for movement between these two adjacent coordinates.
             * @param adjacent A const reference to the start ModelCoordinate.
             * @param next A const reference to the end ModelCoordinate.
             * @return A double which represents the cost.
             */
            double getAdjacentCost(ModelCoordinate const & adjacent, ModelCoordinate const & next);

            /** Returns cost for movement between these two adjacent coordinates.
             * @param adjacent A const reference to the start ModelCoordinate.
             * @param next A const reference to the end ModelCoordinate.
             * @param costId A const reference to the string that contain a cost identifier.
             * @return A double which represents the cost.
             */
            double getAdjacentCost(
                ModelCoordinate const & adjacent, ModelCoordinate const & next, std::string const & costId);

            /** Returns speed value from cell.
             * @param cell A const reference to the cell ModelCoordinate.
             * @param multiplier A reference to a double which receives the speed value.
             * @return A boolean, true if on next cell use special speed, otherwise false.
             */
            bool getCellSpeedMultiplier(ModelCoordinate const & cell, double& multiplier);

            /** Sets default cost for this CellCache.
             * @param multi A double, the cost.
             */
            void setDefaultCostMultiplier(double multi);

            /** Gets default cost for this CellCache.
             * @return A double, the cost.
             */
            double getDefaultCostMultiplier() const;

            /** Sets default speed for this CellCache.
             * @param multi A double, the speed.
             */
            void setDefaultSpeedMultiplier(double multi);

            /** Gets default speed for this CellCache.
             * @return A double, the speed.
             */
            double getDefaultSpeedMultiplier() const;

            /** Gets if cell uses default cost multiplier.
             * @param cell A pointer to the cell.
             * @return A boolean, true if the cell uses default cost multiplier, otherwise false.
             */
            bool isDefaultCost(Cell* cell);

            /** Sets cost multiplier for the cell.
             * @param cell A pointer to the cell.
             * @param multi A double, the cost multiplier.
             */
            void setCostMultiplier(Cell* cell, double multi);

            /** Returns cost multiplier for the cell.
             * @param cell A pointer to the cell.
             * @return A double, the cost multiplier. 1.0 is default.
             */
            double getCostMultiplier(Cell* cell);

            /** Resets the cost multiplier for the cell.
             * @param cell A pointer to the cell.
             */
            void resetCostMultiplier(Cell* cell);

            /** Gets if cell uses default speed multiplier.
             * @param cell A pointer to the cell.
             * @return A boolean, true if the cell uses default speed multiplier, otherwise false.
             */
            bool isDefaultSpeed(Cell* cell);

            /** Sets speed multiplier for the cell.
             * @param cell A pointer to the cell.
             * @param multi A double, the speed multiplier.
             */
            void setSpeedMultiplier(Cell* cell, double multi);

            /** Returns speed multiplier for the cell.
             * @param cell A pointer to the cell.
             * @return A double, the speed multiplier. 1.0 is default.
             */
            double getSpeedMultiplier(Cell* cell);

            /** Resets the speed multiplier for the cell.
             * @param cell A pointer to the cell.
             */
            void resetSpeedMultiplier(Cell* cell);

            /** Adds a cell as transition.
             * @param cell A pointer to transition cell.
             */
            void addTransition(Cell* cell);

            /** Removes a cell as transition.
             * @param cell A pointer to transition cell.
             */
            void removeTransition(Cell* cell);

            /** Returns transistion cells of this CellCache.
             * @param layer A pointer to the layer which should be the target of the transition. If NULL all
             * transistions be returned.
             * @return A vector which contains the transition cells.
             */
            std::vector<Cell*> getTransitionCells(Layer* layer = nullptr);

            /** Returns zones of this CellCache.
             * @return A vector which contains the zones.
             */
            std::vector<Zone*> const & getZones();

            /** Gets zone by identifier.
             * @param id A unsigned integer which is used as zone identifier,
             * @return A pointer to the zone.
             */
            Zone* getZone(uint32_t id);

            /** Creates zone.
             * @return A pointer to the new zone.
             */
            Zone* createZone();

            /** Removes zone.
             * @param zone A pointer to the zone which should be removed.
             */
            void removeZone(Zone* zone);

            /** Splits zone on the cell.
             * @param cell A pointer to the cell where the zone should be splited.
             */
            void splitZone(Cell* cell);

            /** Merges two zones to one.
             * @param zone1 A pointer to the first zone.
             * @param zone2 A pointer to the second zone.
             */
            void mergeZones(Zone* zone1, Zone* zone2);

            /** Adds cell to narrow cells.
             * Narrow cells are observed. On blocking change, the underlying zones are merged or splitted.
             * @param cell A pointer to the cell.
             */
            void addNarrowCell(Cell* cell);

            /** Returns narrow cells.
             * @return A const reference to a set which contains the cells.
             */
            std::set<Cell*> const & getNarrowCells();

            /** Removes cell from narrow cells.
             * @param cell A pointer to the cell.
             */
            void removeNarrowCell(Cell* cell);

            /** Resets narrow cells.
             */
            void resetNarrowCells();

            /** Gets if narrow cells should be searched automatic.
             * Note that narrow cells will only saved if this is disabled.
             * @return A boolean, if true the automatic search is enabled, otherwise false.
             */
            bool isSearchNarrowCells() const;

            /** Sets if narrow cells should be searched automatic.
             * @param search A boolean, if true the automatic search is enabled, otherwise false.
             */
            void setSearchNarrowCells(bool search);

            /** Adds a cell to a specific area group. With an area you can group cells without the need
             *	of checking the underlying instances or similar.
             * @param id A const reference to string that contains the area id.
             * @param cell A pointer to the cell which should be added.
             */
            void addCellToArea(std::string const & id, Cell* cell);

            /** Adds few cell to a specific area group. With an area you can group cells without the need
             *	of checking the underlying instances or similar.
             * @param id A const reference to string that contains the area id.
             * @param cells A const reference to vector which contains the cells.
             */
            void addCellsToArea(std::string const & id, std::vector<Cell*> const & cells);

            /** Removes the cell from all areas.
             * @param cell A pointer to the cell which should be removed.
             */
            void removeCellFromArea(Cell* cell);

            /** Removes the cell from a area.
             * @param id A const reference to string that contains the area id.
             * @param cell A pointer to the cell which should be removed.
             */
            void removeCellFromArea(std::string const & id, Cell* cell);

            /** Removes few cells from a area.
             * @param id A const reference to string that contains the area id.
             * @param cells A const reference to vector which contains the cells.
             */
            void removeCellsFromArea(std::string const & id, std::vector<Cell*> const & cells);

            /** Removes a area.
             * @param id A const reference to string that contains the area id.
             */
            void removeArea(std::string const & id);

            /** Checks whether the area exists.
             * @param id A const reference to string that contains the area id.
             * @return A boolean, true if the area id exists, otherwise false.
             */
            bool existsArea(std::string const & id);

            /** Returns all area ids.
             * @return A vector that contains the area ids.
             */
            std::vector<std::string> getAreas();

            /** Returns all areas of a cell.
             * @param cell A pointer to the cell.
             * @return A vector that contains the area ids.
             */
            std::vector<std::string> getCellAreas(Cell* cell);

            /** Returns all cells of an area.
             * @param id A const reference to string that contains the area id.
             * @return A vector that contains the cells from the area.
             */
            std::vector<Cell*> getAreaCells(std::string const & id);

            /** Returns true if cell is part of the area, otherwise false.
             * @param id A const reference to string that contains the area id.
             * @param cell A pointer to the cell which is used for the check.
             * @return A boolean, true if the cell is part of the area, otherwise false.
             */
            bool isCellInArea(std::string const & id, Cell* cell);

            /** Sets the cache size to static so that automatic resize is disabled.
             * @param staticSize A boolean, true if the cache size is static, otherwise false.
             */
            void setStaticSize(bool staticSize);

            /** Returns if the cache size is static.
             * @return A boolean, true if the cache size is static, otherwise false.
             */
            bool isStaticSize() const;

            void setBlockingUpdate(bool update);
            void setSizeUpdate(bool update);
            void update();

        private:
            using StringCellMultimap = std::multimap<std::string, Cell*>;
            using StringCellIterator = StringCellMultimap::iterator;
            using StringCellPair     = std::pair<StringCellIterator, StringCellIterator>;
            /** Returns the current size.
             * @return A rect that contains the min, max coordinates.
             */
            Rect calculateCurrentSize();

            //! walkable layer
            Layer* m_layer;

            //! default cost
            double m_defaultCostMulti;

            //! default speed
            double m_defaultSpeedMulti;

            //! change listener
            LayerChangeListener* m_cellListener;

            // cells on this cache
            std::vector<std::vector<Cell*>> m_cells;

            //! Rect holds the min and max size
            //! x = min.x, w = max.x, y = min.y, h = max.y
            Rect m_size;

            //! cache width
            uint32_t m_width;

            //! cache height
            uint32_t m_height;

            //! max z value for neighbors
            int32_t m_neighborZ;

            //! indicates blocking update
            bool m_blockingUpdate;

            //! indicates size update
            bool m_sizeUpdate;

            //! is automatic seach enabled
            bool m_searchNarrow;

            //! is automatic size update enabled/disabled
            bool m_staticSize;

            //! cells with transitions
            std::vector<Cell*> m_transitions;

            //! zones
            std::vector<Zone*> m_zones;

            //! special cells which are monitored (zone split and merge)
            std::set<Cell*> m_narrowCells;

            //! areas with assigned cells
            StringCellMultimap m_cellAreas;

            //! listener for zones
            CellChangeListener* m_cellZoneListener;

            //! holds cost table
            std::map<std::string, double> m_costsTable;

            //! holds cells for each cost
            StringCellMultimap m_costsToCells;

            //! holds default cost multiplier, only if it is not default(1.0)
            std::map<Cell*, double> m_costMultipliers;

            //! holds default speed multiplier, only if it is not default(1.0)
            std::map<Cell*, double> m_speedMultipliers;
    };

} // namespace FIFE

#endif
