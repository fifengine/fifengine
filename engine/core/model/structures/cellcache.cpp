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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/grids/cellgrid.h"
#include "util/log/logger.h"
#include "util/structures/purge.h"

#include "cellcache.h"
#include "cell.h"
#include "layer.h"
#include "instance.h"
#include "map.h"
#include "instancetree.h"

namespace FIFE {

	static Logger _log(LM_STRUCTURES);

	class CellCacheChangeListener : public LayerChangeListener {
	public:
		CellCacheChangeListener(Layer* layer)	{
			m_layer = layer;
		}
		virtual ~CellCacheChangeListener() {
		}

		virtual void onLayerChanged(Layer* layer, std::vector<Instance*>& instances) {
			for(std::vector<Instance*>::iterator i = instances.begin();	i != instances.end(); ++i) {
				if ((*i)->isMultiCell()) {
					bool rotchange = ((*i)->getChangeInfo() & ICHANGE_ROTATION) == ICHANGE_ROTATION;
					bool locchange = ((*i)->getChangeInfo() & ICHANGE_LOC) == ICHANGE_LOC;
					bool celchange = ((*i)->getChangeInfo() & ICHANGE_CELL) == ICHANGE_CELL;
					bool blochange = ((*i)->getChangeInfo() & ICHANGE_BLOCK) == ICHANGE_BLOCK;

					if (!rotchange && !locchange && !celchange && !blochange) {
						continue;
					}

					int32_t oldrotation = (*i)->getOldRotation();
					int32_t newrotation = (*i)->getRotation();
					if (!rotchange) {
						oldrotation = newrotation;
					}

					if (rotchange || locchange || celchange) {
						// update visual positions
						(*i)->updateMultiInstances();
					}
					if (rotchange || celchange) {
						// update cache positions
						ModelCoordinate oldmc;
						ModelCoordinate newmc;
						if (m_layer == layer) {
							oldmc = (*i)->getOldLocationRef().getLayerCoordinates();
							newmc = (*i)->getLocationRef().getLayerCoordinates();
						} else {
							oldmc = m_layer->getCellGrid()->toLayerCoordinates(
								layer->getCellGrid()->toMapCoordinates((*i)->getOldLocationRef().getExactLayerCoordinatesRef()));
							newmc = m_layer->getCellGrid()->toLayerCoordinates(
								layer->getCellGrid()->toMapCoordinates((*i)->getLocationRef().getExactLayerCoordinatesRef()));
						}

						if (!celchange) {
							oldmc = newmc;
						}

						CellGrid* cg = m_layer->getCellGrid();
						const std::vector<Instance*>& multiinstances = (*i)->getMultiInstances();
						std::vector<Instance*>::const_iterator it = multiinstances.begin();
						for (; it != multiinstances.end(); ++it) {
							// remove
							std::vector<ModelCoordinate> coordinates =
								cg->toMultiCoordinates(oldmc, (*it)->getObject()->getMultiPartCoordinates(oldrotation));
							std::vector<ModelCoordinate>::iterator mcit = coordinates.begin();
							for (; mcit != coordinates.end(); ++mcit) {
								Cell* cell = m_layer->getCellCache()->getCell(*mcit);
								if (cell) {
									cell->removeInstance(*it);
								}
							}
							// add
							coordinates = cg->toMultiCoordinates(newmc, (*it)->getObject()->getMultiPartCoordinates(newrotation));
							mcit = coordinates.begin();
							for (; mcit != coordinates.end(); ++mcit) {
								Cell* cell = m_layer->getCellCache()->getCell(*mcit);
								if (cell) {
									cell->addInstance(*it);
								}
							}
						}

						if (celchange) {
							// leader instance
							Cell* oldcell = m_layer->getCellCache()->getCell(oldmc);
							Cell* newcell = m_layer->getCellCache()->getCell(newmc);
							if (oldcell == newcell) {
								continue;
							}
							if (oldcell) {
								oldcell->removeInstance(*i);
							}
							if (newcell) {
								newcell->addInstance(*i);
							}
						}
					}
					continue;
				}
				if ((*i)->getObject()->isMultiPart()) {
					continue;
				}
				if (((*i)->getChangeInfo() & ICHANGE_BLOCK) == ICHANGE_BLOCK) {
					ModelCoordinate mc;
					if (m_layer == layer) {
						mc = (*i)->getLocationRef().getLayerCoordinates();
					} else {
						mc = m_layer->getCellGrid()->toLayerCoordinates(
							layer->getCellGrid()->toMapCoordinates((*i)->getLocationRef().getExactLayerCoordinatesRef()));
					}
					Cell* cell = m_layer->getCellCache()->getCell(mc);
					if (cell) {
						cell->changeInstance(*i);
					}
				}
				if (((*i)->getChangeInfo() & ICHANGE_CELL) == ICHANGE_CELL) {
					ModelCoordinate oldmc;
					ModelCoordinate newmc;
					if (m_layer == layer) {
						oldmc = (*i)->getOldLocationRef().getLayerCoordinates();
						newmc = (*i)->getLocationRef().getLayerCoordinates();
					} else {
						oldmc = m_layer->getCellGrid()->toLayerCoordinates(
							layer->getCellGrid()->toMapCoordinates((*i)->getOldLocationRef().getExactLayerCoordinatesRef()));
						newmc = m_layer->getCellGrid()->toLayerCoordinates(
							layer->getCellGrid()->toMapCoordinates((*i)->getLocationRef().getExactLayerCoordinatesRef()));
					}

					Cell* oldcell = m_layer->getCellCache()->getCell(oldmc);
					Cell* newcell = m_layer->getCellCache()->getCell(newmc);
					if (oldcell == newcell) {
						continue;
					}
					if (oldcell) {
						oldcell->removeInstance(*i);
					}
					if (newcell) {
						newcell->addInstance(*i);
					}
				}
			}
		}

		virtual void onInstanceCreate(Layer* layer, Instance* instance)	{
			ModelCoordinate mc;
			if (m_layer == layer) {
				mc = instance->getLocationRef().getLayerCoordinates();
			} else {
				mc = m_layer->getCellGrid()->toLayerCoordinates(
					layer->getCellGrid()->toMapCoordinates(instance->getLocationRef().getExactLayerCoordinatesRef()));
			}

			CellCache* cache = m_layer->getCellCache();
			Location loc(m_layer);
			loc.setLayerCoordinates(mc);
			if (!cache->isInCellCache(loc)) {
				cache->resize();
			}
			if (instance->isMultiCell()) {
				instance->updateMultiInstances();
				CellGrid* cg = m_layer->getCellGrid();
				const std::vector<Instance*>& multiinstances = instance->getMultiInstances();
				std::vector<Instance*>::const_iterator it = multiinstances.begin();
				for (; it != multiinstances.end(); ++it) {
					std::vector<ModelCoordinate> coordinates =
						cg->toMultiCoordinates(mc, (*it)->getObject()->getMultiPartCoordinates(instance->getRotation()));
					std::vector<ModelCoordinate>::iterator mcit = coordinates.begin();
					for (; mcit != coordinates.end(); ++mcit) {
						loc.setLayerCoordinates(*mcit);
						if (!cache->isInCellCache(loc)) {
							cache->resize();
						}
						Cell* cell = cache->getCell(*mcit);
						if (cell) {
							cell->addInstance(*it);
						}
					}
				}
			}
			Cell* cell = cache->getCell(mc);
			if (cell) {
				cell->addInstance(instance);
			}
		}

		virtual void onInstanceDelete(Layer* layer, Instance* instance)	{
			ModelCoordinate mc;
			if (m_layer == layer) {
				mc = instance->getLocationRef().getLayerCoordinates();
			} else {
				mc = m_layer->getCellGrid()->toLayerCoordinates(
					layer->getCellGrid()->toMapCoordinates(instance->getLocationRef().getExactLayerCoordinatesRef()));
			}

			CellCache* cache = m_layer->getCellCache();
			if (instance->isMultiCell()) {
				instance->updateMultiInstances();
				CellGrid* cg = m_layer->getCellGrid();
				const std::vector<Instance*>& multiinstances = instance->getMultiInstances();
				std::vector<Instance*>::const_iterator it = multiinstances.begin();
				for (; it != multiinstances.end(); ++it) {
					std::vector<ModelCoordinate> coordinates =
						cg->toMultiCoordinates(mc, (*it)->getObject()->getMultiPartCoordinates(instance->getRotation()));
					std::vector<ModelCoordinate>::iterator mcit = coordinates.begin();
					for (; mcit != coordinates.end(); ++mcit) {
						Cell* cell = cache->getCell(*mcit);
						if (cell) {
							cell->removeInstance(*it);
						}
					}
				}
			}
			Cell* cell = cache->getCell(mc);
			if (cell) {
				cell->removeInstance(instance);
			}
			// updates size on the next cache update (happens on same pump)
			cache->setSizeUpdate(true);
		}

	private:
		Layer* m_layer;
	};

	Zone::Zone(uint32_t id):
		m_id(id) {
	}

	Zone::~Zone() {
		for (std::set<Cell*>::iterator i = m_cells.begin(); i != m_cells.end(); ++i) {
			(*i)->resetZone();
		}
	}

	void Zone::addCell(Cell* cell) {
		if (!cell->getZone()) {
			cell->setZone(this);
			m_cells.insert(cell);
		}
	}

	void Zone::removeCell(Cell* cell) {
		std::set<Cell*>::iterator i = m_cells.find(cell);
		if (i != m_cells.end()) {
			(*i)->resetZone();
			m_cells.erase(i);
		}
	}
	
	void Zone::mergeZone(Zone* zone) {
		const std::set<Cell*>& cells = zone->getCells();
		m_cells.insert(cells.begin(), cells.end());
		for (std::set<Cell*>::const_iterator it = cells.begin(); it != cells.end(); ++it) {
			(*it)->setZone(this);
		}
		zone->resetCells();
	}

	const std::set<Cell*>& Zone::getCells() const {
		return m_cells;
	}

	void Zone::resetCells() {
		m_cells.clear();
	}

	uint32_t Zone::getId() const {
		return m_id;
	}

	uint32_t Zone::getCellCount() const {
		return static_cast<uint32_t>(m_cells.size());
	}

	std::vector<Cell*> Zone::getTransitionCells(Layer* layer) {
		std::vector<Cell*> transitions;
		std::set<Cell*>::iterator it = m_cells.begin();
		for (; it != m_cells.end(); ++it) {
			TransitionInfo* trans = (*it)->getTransition();
			if (!trans) {
				continue;
			}
			if (layer) {
				if (layer == (*it)->getLayer()) {
					transitions.push_back(*it);
				}
			} else {
				transitions.push_back(*it);
			}
		}
		return transitions;
	}

	class ZoneCellChangeListener : public CellChangeListener {
	public:
		ZoneCellChangeListener(CellCache* cache) {
			m_cache = cache;
		}
		virtual ~ZoneCellChangeListener() {
		}

		virtual void onInstanceEnteredCell(Cell* cell, Instance* instance) {

		}

		virtual void onInstanceExitedCell(Cell* cell, Instance* instance) {

		}

		virtual void onBlockingChangedCell(Cell* cell, CellTypeInfo type, bool blocks) {
			if (blocks) {
				cell->setZoneProtected(true);
				m_cache->splitZone(cell);
			} else {
				Zone* z1 = cell->getZone();
				Zone* z2 = NULL;
				const std::vector<Cell*>& neighbors = cell->getNeighbors();
				std::vector<Cell*>::const_iterator it = neighbors.begin();
				for (; it != neighbors.end(); ++it) {
					Zone* z = (*it)->getZone();
					if (z && z != z1) {
						z2 = z;
					}
				}
				if (z1 && z2) {
					cell->setZoneProtected(false);
					m_cache->mergeZones(z1, z2);
				}
			}
		}

	private:
		CellCache* m_cache;
	};

	CellCache::CellCache(Layer* layer):
		m_layer(layer),
		m_defaultCostMulti(1.0),
		m_defaultSpeedMulti(1.0),
		m_neighborZ(-1),
		m_blockingUpdate(false),
		m_fowUpdate(false),
		m_sizeUpdate(false),
		m_updated(false),
		m_searchNarrow(true),
		m_staticSize(false) {
		// create cell change listener
		m_cellZoneListener = new ZoneCellChangeListener(this);
		// set base size
		ModelCoordinate min, max;
		m_layer->getMinMaxCoordinates(min, max);
		m_size.w = max.x;
		m_size.h = max.y;
		m_size.x = min.x;
		m_size.y = min.y;

		// create and add layer listener
		m_cellListener = new CellCacheChangeListener(m_layer);
		m_layer->addChangeListener(m_cellListener);
		const std::vector<Layer*>& interacts = m_layer->getInteractLayers();
		if (!interacts.empty()) {
			std::vector<Layer*>::const_iterator layit = interacts.begin();
			for(; layit != interacts.end(); ++layit) {
				// set size
				(*layit)->getMinMaxCoordinates(min, max, m_layer);
				m_size.w = std::max(max.x, m_size.w);
				m_size.h = std::max(max.y, m_size.h);
				m_size.x = std::min(min.x, m_size.x);
				m_size.y = std::min(min.y, m_size.y);
				// add listener
				(*layit)->addChangeListener(m_cellListener);
			}
		}

		m_width = ABS(m_size.w - m_size.x) + 1;
		m_height = ABS(m_size.h - m_size.y) + 1;

		m_cells.resize(m_width);
		for (uint32_t i = 0; i < m_width; ++i) {
			m_cells[i].resize(m_height, NULL);
		}
	}

	CellCache::~CellCache() {
		// reset cache
		reset();
		// remove listener from layers
		m_layer->removeChangeListener(m_cellListener);
		const std::vector<Layer*>& interacts = m_layer->getInteractLayers();
		if (!interacts.empty()) {
			std::vector<Layer*>::const_iterator layit = interacts.begin();
			for(; layit != interacts.end(); ++layit) {
				(*layit)->removeChangeListener(m_cellListener);
			}
		}
		// delete listener
		delete m_cellListener;
		delete m_cellZoneListener;
	}

	void CellCache::reset() {
		// delete zones
		if (!m_zones.empty()) {
			std::vector<Zone*>::iterator it = m_zones.begin();
			for (; it != m_zones.end(); ++it) {
				delete *it;
			}
			m_zones.clear();
		}
		// clear all containers
		m_costsToCells.clear();
		m_costsTable.clear();
		m_costMultipliers.clear();
		m_speedMultipliers.clear();
		m_narrowCells.clear();
		m_cellAreas.clear();
		// delete cells
		if (!m_cells.empty()) {
			std::vector<std::vector<Cell*> >::iterator it = m_cells.begin();
			for (; it != m_cells.end(); ++it) {
				std::vector<Cell*>::iterator cit = (*it).begin();
				for (; cit != (*it).end(); ++cit) {
					delete *cit;
				}
			}
			m_cells.clear();
		}
		// reset default cost and speed
		m_defaultCostMulti = 1.0;
		m_defaultSpeedMulti = 1.0;
		// reset size
		m_size.x = 0;
		m_size.y = 0;
		m_size.w = 0;
		m_size.h = 0;
		m_width = 0;
		m_height = 0;
	}

	void CellCache::resize() {
		if (m_staticSize) {
			return;
		}
		// get new size and check if it has changed
		Rect newsize = calculateCurrentSize();
		resize(newsize);
	}

	void CellCache::resize(const Rect& rec) {
		// check if size has changed
		Rect newsize = rec;
		if (newsize.x != m_size.x || newsize.y != m_size.y || newsize.w != m_size.w || newsize.h != m_size.h) {
			uint32_t w = ABS(newsize.w - newsize.x) + 1;
			uint32_t h = ABS(newsize.h - newsize.y) + 1;

			std::vector<std::vector<Cell*> > cells;
			cells.resize(w);
			for (uint32_t i = 0; i < w; ++i) {
				cells[i].resize(h, NULL);
			}
			const std::vector<Layer*>& interacts = m_layer->getInteractLayers();
			for(uint32_t y = 0; y < h; ++y) {
				for(uint32_t x = 0; x < w; ++x) {
					// transfer cells
					ModelCoordinate mc(newsize.x+x, newsize.y+y);
					Cell* cell = NULL;
					int32_t old_x = mc.x - m_size.x;
					int32_t old_y = mc.y - m_size.y;
					// out of range in the old size, so we create a new cell
					if (old_x < 0 || old_x >= static_cast<int32_t>(m_width) || old_y < 0 || old_y >= static_cast<int32_t>(m_height)) {
						int32_t coordId = x + y * w;
						cell = new Cell(coordId, mc, m_layer);
						cells[x][y] = cell;

						std::list<Instance*> cell_instances;
						m_layer->getInstanceTree()->findInstances(mc, 0, 0, cell_instances);
						if (!interacts.empty()) {
							// fill interact Instances into Cell
							std::vector<Layer*>::const_iterator it = interacts.begin();
							std::list<Instance*> interact_instances;
							for(; it != interacts.end(); ++it) {
								// convert coordinates
								ExactModelCoordinate emc(FIFE::intPt2doublePt(mc));
								ModelCoordinate inter_mc = (*it)->getCellGrid()->toLayerCoordinates(m_layer->getCellGrid()->toMapCoordinates(emc));
								// check interact layer for instances
								(*it)->getInstanceTree()->findInstances(inter_mc, 0, 0, interact_instances);
								if (!interact_instances.empty()) {
									cell_instances.insert(cell_instances.end(), interact_instances.begin(), interact_instances.end());
									interact_instances.clear();
								}
							}
						}
						if (!cell_instances.empty()) {
							// add instances to cell
							cell->addInstances(cell_instances);
						}
					// transfer ownership
					} else {
						cell = m_cells[static_cast<uint32_t>(old_x)][static_cast<uint32_t>(old_y)];
						m_cells[static_cast<uint32_t>(old_x)][static_cast<uint32_t>(old_y)] = NULL;
						cells[x][y] = cell;
						int32_t coordId = x + y * w;
						cell->setCellId(coordId);
						cell->resetNeighbors();
					}
				}
			}
			// delete old unused cells
			std::vector<std::vector<Cell*> >::iterator it = m_cells.begin();
			for (; it != m_cells.end(); ++it) {
				std::vector<Cell*>::iterator cit = (*it).begin();
				for (; cit != (*it).end(); ++cit) {
					if (*cit) {
						delete *cit;
						*cit = NULL;
					}
				}
			}
			// use new values
			m_cells = cells;
			m_size = newsize;
			m_width = w;
			m_height = h;

			bool zCheck = m_neighborZ != -1;
			// fill neighbors into cells
			it = m_cells.begin();
			for (; it != m_cells.end(); ++it) {
				std::vector<Cell*>::iterator cit = (*it).begin();
				for (; cit != (*it).end(); ++cit) {
					int32_t cellZ = (*cit)->getLayerCoordinates().z;
					std::vector<ModelCoordinate> coordinates;
					m_layer->getCellGrid()->getAccessibleCoordinates((*cit)->getLayerCoordinates(), coordinates);
					for (std::vector<ModelCoordinate>::iterator mi = coordinates.begin(); mi != coordinates.end(); ++mi) {
						Cell* c = getCell(*mi);
						if (*cit == c || !c) {
							continue;
						}
						if (zCheck) {
							if (ABS(c->getLayerCoordinates().z - cellZ) > m_neighborZ) {
								continue;
							}
						}
						(*cit)->addNeighbor(c);
					}
				}
			}
		}
	}

	void CellCache::createCells() {
		const std::vector<Layer*>& interacts = m_layer->getInteractLayers();
		for(uint32_t y = 0; y < m_height; ++y) {
			for(uint32_t x = 0; x < m_width; ++x) {
				ModelCoordinate mc(m_size.x+x, m_size.y+y);
				Cell* cell = getCell(mc);
				if (!cell) {
					cell = new Cell(convertCoordToInt(mc), mc, m_layer);
					m_cells[x][y] = cell;
				}
				// fill Instances into Cell
				std::list<Instance*> cell_instances;
				m_layer->getInstanceTree()->findInstances(mc, 0, 0, cell_instances);
				if (!interacts.empty()) {
					// fill interact Instances into Cell
					std::vector<Layer*>::const_iterator it = interacts.begin();
					std::list<Instance*> interact_instances;
					for(; it != interacts.end(); ++it) {
						// convert coordinates
						ExactModelCoordinate emc(FIFE::intPt2doublePt(mc));
						ModelCoordinate inter_mc = (*it)->getCellGrid()->toLayerCoordinates(m_layer->getCellGrid()->toMapCoordinates(emc));
						// check interact layer for instances
						(*it)->getInstanceTree()->findInstances(inter_mc, 0, 0, interact_instances);
						if (!interact_instances.empty()) {
							cell_instances.insert(cell_instances.end(), interact_instances.begin(), interact_instances.end());
							interact_instances.clear();
						}
					}
				}
				if (!cell_instances.empty()) {
					// add instances to cell
					cell->addInstances(cell_instances);
				}
			}
		}
		// fill neighbors into cells
		std::vector<std::vector<Cell*> >::iterator it = m_cells.begin();
		for (; it != m_cells.end(); ++it) {
			std::vector<Cell*>::iterator cit = (*it).begin();
			for (; cit != (*it).end(); ++cit) {
				uint8_t accessible = 0;
				bool selfblocker = (*cit)->getCellType() == CTYPE_STATIC_BLOCKER || (*cit)->getCellType() == CTYPE_CELL_BLOCKER;
				std::vector<ModelCoordinate> coordinates;
				m_layer->getCellGrid()->getAccessibleCoordinates((*cit)->getLayerCoordinates(), coordinates);
				for (std::vector<ModelCoordinate>::iterator mi = coordinates.begin(); mi != coordinates.end(); ++mi) {
					Cell* c = getCell(*mi);
					if (*cit == c || !c) {
						continue;
					}
					if (!selfblocker && c->getCellType() != CTYPE_STATIC_BLOCKER &&
						c->getCellType() != CTYPE_CELL_BLOCKER) {
						++accessible;
					}
					(*cit)->addNeighbor(c);
				}
				// add cell to narrow cells and add listener for zone change
				if (m_searchNarrow && !selfblocker && accessible < 3) {
					addNarrowCell(*cit);
				}
			}
		}
		// create Zones
		it = m_cells.begin();
		for (; it != m_cells.end(); ++it) {
			std::vector<Cell*>::iterator cit = (*it).begin();
			for (; cit != (*it).end(); ++cit) {
				Cell* cell = *cit;
				if (cell->getZone() || cell->isInserted()) {
					continue;
				}
				if (cell->getCellType() == CTYPE_STATIC_BLOCKER || cell->getCellType() == CTYPE_CELL_BLOCKER) {
					continue;
				}
				Zone* zone = createZone();
				cell->setInserted(true);
				std::stack<Cell*> cellstack;
				cellstack.push(cell);
				while(!cellstack.empty()) {
					Cell* c = cellstack.top();
					cellstack.pop();
					zone->addCell(c);

					const std::vector<Cell*>& neighbors = c->getNeighbors();
					for (std::vector<Cell*>::const_iterator nit = neighbors.begin(); nit != neighbors.end(); ++nit) {
						Cell* nc = *nit;
						if (!nc->isInserted() &&
							nc->getCellType() != CTYPE_STATIC_BLOCKER && nc->getCellType() != CTYPE_CELL_BLOCKER) {
							nc->setInserted(true);
							cellstack.push(nc);
						}
					}
				}
			}
		}
	}

	void CellCache::forceUpdate() {
		std::vector<std::vector<Cell*> >::iterator it = m_cells.begin();
		for (; it != m_cells.end(); ++it) {
			std::vector<Cell*>::iterator cit = (*it).begin();
			for (; cit != (*it).end(); ++cit) {
				(*cit)->updateCellInfo();
			}
		}
	}

	void CellCache::addCell(Cell* cell) {
		ModelCoordinate mc = cell->getLayerCoordinates();
		m_cells[(mc.x-m_size.x)][(mc.y-m_size.y)] = cell;
	}

	Cell* CellCache::createCell(const ModelCoordinate& mc) {
		Cell* cell = getCell(mc);
		if (!cell) {
			cell = new Cell(convertCoordToInt(mc), mc, m_layer);
			m_cells[(mc.x-m_size.x)][(mc.y-m_size.y)] = cell;
		}
		return cell;
	}

	Cell* CellCache::getCell(const ModelCoordinate& mc) {
		int32_t x = mc.x - m_size.x;
		int32_t y = mc.y - m_size.y;

		if (x < 0 || x >= static_cast<int32_t>(m_width) || y < 0 || y >= static_cast<int32_t>(m_height)) {
			return NULL;
		}

		return m_cells[static_cast<uint32_t>(x)][static_cast<uint32_t>(y)];
	}

	const std::vector<std::vector<Cell*> >& CellCache::getCells() {
		return m_cells;
	}

	void CellCache::removeCell(Cell* cell) {
		if (!m_costsToCells.empty()) {
			removeCellFromCost(cell);
		}
		if (!m_costMultipliers.empty()) {
			resetCostMultiplier(cell);
		}
		if (!m_speedMultipliers.empty()) {
			resetSpeedMultiplier(cell);
		}
		if (!m_narrowCells.empty()) {
			removeNarrowCell(cell);
		}
		if (!m_cellAreas.empty()) {
			removeCellFromArea(cell);
		}
	}

	void CellCache::addInteractOnRuntime(Layer* interact) {
		interact->setInteract(true, m_layer->getId());
		m_layer->addInteractLayer(interact);
		interact->addChangeListener(m_cellListener);
		Rect newsize = calculateCurrentSize();
		if (newsize.x != m_size.x || newsize.y != m_size.y || newsize.w != m_size.w || newsize.h != m_size.h) {
			resize();
		}
		// not optimal but needed if the grids have different geometry
		for(uint32_t y = 0; y < m_height; ++y) {
			for(uint32_t x = 0; x < m_width; ++x) {
				ModelCoordinate mc(m_size.x+x, m_size.y+y);
				Cell* cell = getCell(mc);
				if (cell) {
					// convert coordinates
					ExactModelCoordinate emc(FIFE::intPt2doublePt(mc));
					ModelCoordinate inter_mc = interact->getCellGrid()->toLayerCoordinates(m_layer->getCellGrid()->toMapCoordinates(emc));
					// check interact layer for instances
					std::list<Instance*> interact_instances;
					interact->getInstanceTree()->findInstances(inter_mc, 0, 0, interact_instances);
					if (!interact_instances.empty()) {
						// fill interact Instances into Cell
						cell->addInstances(interact_instances);
					}
				}
			}
		}
	}

	void CellCache::removeInteractOnRuntime(Layer* interact) {
		interact->setInteract(false, "");
		m_layer->removeInteractLayer(interact);
		Rect newsize = calculateCurrentSize();
		if (newsize.x != m_size.x || newsize.y != m_size.y || newsize.w != m_size.w || newsize.h != m_size.h) {
			resize();
		}
		// not optimal but needed if the grids have different geometry
		for(uint32_t y = 0; y < m_height; ++y) {
			for(uint32_t x = 0; x < m_width; ++x) {
				ModelCoordinate mc(m_size.x+x, m_size.y+y);
				Cell* cell = getCell(mc);
				if (cell) {
					// convert coordinates
					ExactModelCoordinate emc(FIFE::intPt2doublePt(mc));
					ModelCoordinate inter_mc = interact->getCellGrid()->toLayerCoordinates(m_layer->getCellGrid()->toMapCoordinates(emc));
					// check interact layer for instances
					std::list<Instance*> interact_instances;
					interact->getInstanceTree()->findInstances(inter_mc, 0, 0, interact_instances);
					if (!interact_instances.empty()) {
						// remove interact Instances from Cell
						for (std::list<Instance*>::iterator it = interact_instances.begin(); it != interact_instances.end(); ++it) {
							cell->removeInstance(*it);
						}
					}
				}
			}
		}
	}

	LayerChangeListener* CellCache::getCellCacheChangeListener() {
		return m_cellListener;
	}

	Layer* CellCache::getLayer() {
		return m_layer;
	}

	const Rect& CellCache::getSize() {
		return m_size;
	}

	void CellCache::setSize(const Rect& rec) {
		resize(rec);
	}

	uint32_t CellCache::getWidth() {
		return m_width;
	}

	uint32_t CellCache::getHeight() {
		return m_height;
	}

	bool CellCache::isInCellCache(const Location& location) const {
		if (m_layer != location.getLayer()) {
			return false;
		}
		int32_t x = location.getLayerCoordinates().x - m_size.x;
		int32_t y = location.getLayerCoordinates().y - m_size.y;

		if (x < 0 || x >= static_cast<int32_t>(m_width) || y < 0 || y >= static_cast<int32_t>(m_height)) {
			return false;
		}
		return true;
	}

	int32_t CellCache::convertCoordToInt(const ModelCoordinate& coord) const {
		ModelCoordinate newcoords(coord.x - m_size.x, coord.y - m_size.y);
		return newcoords.x + newcoords.y*m_width;
	}

	ModelCoordinate CellCache::convertIntToCoord(const int32_t cell) const {
		ModelCoordinate coord((cell % m_width) + m_size.x, (cell / m_width) + m_size.y);
		return coord;
	}

	int32_t CellCache::getMaxIndex() const {
		int32_t max_index = m_width*m_height;
		return max_index;
	}

	void CellCache::setMaxNeighborZ(int32_t z) {
		m_neighborZ = z;
	}

	int32_t CellCache::getMaxNeighborZ() {
		return m_neighborZ;
	}

	void CellCache::setUpdated(bool updated) {
		m_updated = updated;
	}

	bool CellCache::isUpdated() {
		return m_updated;
	}

	std::vector<Cell*> CellCache::getCellsInLine(const ModelCoordinate& pt1, const ModelCoordinate& pt2, bool blocker) {
		std::vector<Cell*> cells;
		std::vector<ModelCoordinate> coords = m_layer->getCellGrid()->getCoordinatesInLine(pt1, pt2);
		for (std::vector<ModelCoordinate>::iterator it = coords.begin(); it != coords.end(); ++it) {
			Cell* c = getCell(*it);
			if (c) {
				if (blocker && c->getCellType() != CTYPE_NO_BLOCKER) {
					return cells;
				}
				cells.push_back(c);
			} else {
				return cells;
			}
		}
		return cells;
	}

	std::vector<Cell*> CellCache::getCellsInRect(const Rect& rec) {
		std::vector<Cell*> cells;

		ModelCoordinate current(rec.x, rec.y);
		ModelCoordinate target(rec.x+rec.w, rec.y+rec.h);
		for (; current.y < target.y; ++current.y) {
			current.x = rec.x;
			for (; current.x < target.x; ++current.x) {
				Cell* c = getCell(current);
				if (c) {
					cells.push_back(c);
				}
			}
		}
		return cells;
	}

	std::vector<Cell*> CellCache::getCellsInCircle(const ModelCoordinate& center, uint16_t radius) {
		std::vector<Cell*> cells;
		//radius power 2
		uint16_t radiusp2 = (radius+1) * radius;

		ModelCoordinate current(center.x-radius, center.y-radius);
		ModelCoordinate target(center.x+radius, center.y+radius);
		for (; current.y < center.y; current.y++) {
			current.x = center.x-radius;
			for (; current.x < center.x; current.x++) {
				Cell* c = getCell(current);
				if (c) {
					uint16_t dx = center.x - current.x;
					uint16_t dy = center.y - current.y;
					uint16_t distance = dx*dx + dy*dy;
					if (distance <= radiusp2) {
						cells.push_back(c);

						current.x = center.x + dx;
						c = getCell(current);
						if (c) cells.push_back(c);

						current.y = center.y + dy;
						c = getCell(current);
						if (c) cells.push_back(c);

						current.x = center.x-dx;
						c = getCell(current);
						if (c) cells.push_back(c);

						current.y = center.y-dy;

					}
				}
			}
		}
		current.x = center.x;
		current.y = center.y-radius;
		for (; current.y <= target.y; current.y++) {
			Cell* c = getCell(current);
			if (c) cells.push_back(c);
		}

		current.y = center.y;
		current.x = center.x-radius;
		for (; current.x <= target.x; current.x++) {
			Cell* c = getCell(current);
			if (c) cells.push_back(c);
		}
		return cells;
	}

	std::vector<Cell*> CellCache::getCellsInCircleSegment(const ModelCoordinate& center, uint16_t radius, int32_t sangle, int32_t eangle) {
		std::vector<Cell*> cells;
		ExactModelCoordinate exactCenter(center.x, center.y);
		std::vector<Cell*> tmpCells = getCellsInCircle(center, radius);
		int32_t s = (sangle + 360) % 360;
		int32_t e = (eangle + 360) % 360;
		bool greater = (s > e) ? true : false;
		for (std::vector<Cell*>::iterator it = tmpCells.begin(); it != tmpCells.end(); ++it) {
			int32_t angle = getAngleBetween(exactCenter, intPt2doublePt((*it)->getLayerCoordinates()));
			if (greater) {
				if (angle >= s || angle <= e) {
					cells.push_back(*it);
				}
			} else {
				if (angle >= s && angle <= e) {
					cells.push_back(*it);
				}
			}
		}
		return cells;
	}

	void CellCache::registerCost(const std::string& costId, double cost) {
		std::pair<std::map<std::string, double>::iterator, bool> insertiter;
		insertiter = m_costsTable.insert(std::pair<std::string, double>(costId, cost));
		if (insertiter.second == false) {
			double& old_cost = insertiter.first->second;
			old_cost = cost;
		}
	}

	void CellCache::unregisterCost(const std::string& costId) {
		std::map<std::string, double>::iterator it = m_costsTable.find(costId);
		if (it != m_costsTable.end()) {
			m_costsTable.erase(it);
			m_costsToCells.erase(costId);
		}
	}

	double CellCache::getCost(const std::string& costId) {
		std::map<std::string, double>::iterator it = m_costsTable.find(costId);
		if (it != m_costsTable.end()) {
			return it->second;
		}
		return 0.0;
	}

	bool CellCache::existsCost(const std::string& costId) {
		std::map<std::string, double>::iterator it = m_costsTable.find(costId);
		if (it != m_costsTable.end()) {
			return true;
		}
		return false;
	}

	std::list<std::string> CellCache::getCosts() {
		std::list<std::string> costs;
		std::map<std::string, double>::iterator it = m_costsTable.begin();
		for (; it != m_costsTable.end(); ++it) {
			costs.push_back((*it).first);
		}
		return costs;
	}

	void CellCache::unregisterAllCosts() {
		m_costsTable.clear();
		m_costsToCells.clear();
	}

	void CellCache::addCellToCost(const std::string& costId, Cell* cell) {
		if (existsCost(costId)) {
			StringCellPair result = m_costsToCells.equal_range(costId);
			StringCellIterator it = result.first;
			for (; it != result.second; ++it) {
				if ((*it).second == cell) {
					return;
				}
			}
			m_costsToCells.insert(std::pair<std::string, Cell*>(costId, cell));
		}
	}

	void CellCache::addCellsToCost(const std::string& costId, const std::vector<Cell*>& cells) {
		std::vector<Cell*>::const_iterator it = cells.begin();
		for (; it != cells.end(); ++it) {
			addCellToCost(costId, *it);
		}
	}

	void CellCache::removeCellFromCost(Cell* cell) {
		StringCellIterator it = m_costsToCells.begin();
		for (; it != m_costsToCells.end();) {
			if ((*it).second == cell) {
				m_costsToCells.erase(it++);
			} else {
				++it;
			}
		}
	}

	void CellCache::removeCellFromCost(const std::string& costId, Cell* cell) {
		StringCellPair result = m_costsToCells.equal_range(costId);
		StringCellIterator it = result.first;
		for (; it != result.second; ++it) {
			if ((*it).second == cell) {
				m_costsToCells.erase(it);
				break;
			}
		}
	}

	void CellCache::removeCellsFromCost(const std::string& costId, const std::vector<Cell*>& cells) {
		std::vector<Cell*>::const_iterator it = cells.begin();
		for (; it != cells.end(); ++it) {
			removeCellFromCost(costId, *it);
		}
	}

	std::vector<Cell*> CellCache::getCostCells(const std::string& costId) {
		std::vector<Cell*> cells;
		StringCellPair result = m_costsToCells.equal_range(costId);
		StringCellIterator it = result.first;
		for (; it != result.second; ++it) {
			cells.push_back((*it).second);
		}
		return cells;
	}

	std::vector<std::string> CellCache::getCellCosts(Cell* cell) {
		std::vector<std::string> costs;
		StringCellIterator it = m_costsToCells.begin();
		for (; it != m_costsToCells.end(); ++it) {
			if ((*it).second == cell) {
				costs.push_back((*it).first);
			}
		}
		return costs;
	}

	bool CellCache::existsCostForCell(const std::string& costId, Cell* cell) {
		StringCellPair result = m_costsToCells.equal_range(costId);
		StringCellIterator it = result.first;
		for (; it != result.second; ++it) {
			if ((*it).second == cell) {
				return true;
			}
		}
		return false;
	}

	double CellCache::getAdjacentCost(const ModelCoordinate& adjacent, const ModelCoordinate& next) {
		double cost = m_layer->getCellGrid()->getAdjacentCost(adjacent, next);
		Cell* nextcell = getCell(next);
		if (nextcell) {
			if (!nextcell->defaultCost()) {
				cost *= nextcell->getCostMultiplier();
			} else {
				cost *= m_defaultCostMulti;
			}
		}
		return cost;
	}

	double CellCache::getAdjacentCost(const ModelCoordinate& adjacent, const ModelCoordinate& next, const std::string& costId) {
		double cost = m_layer->getCellGrid()->getAdjacentCost(adjacent, next);
		Cell* nextcell = getCell(next);
		if (nextcell) {
			if (existsCostForCell(costId, nextcell)) {
				cost *= getCost(costId);
			} else {
				if (!nextcell->defaultCost()) {
					cost *= nextcell->getCostMultiplier();
				} else {
					cost *= m_defaultCostMulti;
				}
			}
		}
		return cost;
	}

	bool CellCache::getCellSpeedMultiplier(const ModelCoordinate& cell, double& multiplier) {
		Cell* nextcell = getCell(cell);
		if (nextcell) {
			if (!nextcell->defaultSpeed()) {
				multiplier = nextcell->getSpeedMultiplier();
				return true;
			}
		}
		multiplier = m_defaultSpeedMulti;
		return false;
	}

	void CellCache::setDefaultCostMultiplier(double multi) {
		m_defaultCostMulti = multi;
	}

	double CellCache::getDefaultCostMultiplier() {
		return m_defaultCostMulti;
	}

	void CellCache::setDefaultSpeedMultiplier(double multi) {
		m_defaultSpeedMulti = multi;
	}

	double CellCache::getDefaultSpeedMultiplier() {
		return m_defaultSpeedMulti;
	}

	bool CellCache::isDefaultCost(Cell* cell) {
		std::map<Cell*, double>::iterator it = m_costMultipliers.find(cell);
		if (it != m_costMultipliers.end()) {
			return false;
		}
		return true;
	}

	void CellCache::setCostMultiplier(Cell* cell, double multi) {
		std::pair<std::map<Cell*, double>::iterator, bool> insertiter =
			m_costMultipliers.insert(std::pair<Cell*, double>(cell, multi));
		if (insertiter.second == false) {
			double& old = insertiter.first->second;
			old = multi;
		}
	}

	double CellCache::getCostMultiplier(Cell* cell) {
		double cost = 1.0;
		std::map<Cell*, double>::iterator it = m_costMultipliers.find(cell);
		if (it != m_costMultipliers.end()) {
			cost = it->second;
		}
		return cost;
	}

	void CellCache::resetCostMultiplier(Cell* cell) {
		m_costMultipliers.erase(cell);
	}

	bool CellCache::isDefaultSpeed(Cell* cell) {
		std::map<Cell*, double>::iterator it = m_speedMultipliers.find(cell);
		if (it != m_speedMultipliers.end()) {
			return false;
		}
		return true;
	}

	void CellCache::setSpeedMultiplier(Cell* cell, double multi) {
		std::pair<std::map<Cell*, double>::iterator, bool> insertiter =
			m_speedMultipliers.insert(std::pair<Cell*, double>(cell, multi));
		if (insertiter.second == false) {
			double& old = insertiter.first->second;
			old = multi;
		}
	}

	double CellCache::getSpeedMultiplier(Cell* cell) {
		double speed = 1.0;
		std::map<Cell*, double>::iterator it = m_speedMultipliers.find(cell);
		if (it != m_speedMultipliers.end()) {
			speed = it->second;
		}
		return speed;
	}

	void CellCache::resetSpeedMultiplier(Cell* cell) {
		m_speedMultipliers.erase(cell);
	}

	void CellCache::addTransition(Cell* cell) {
		m_transitions.push_back(cell);
	}

	void CellCache::removeTransition(Cell* cell) {
		std::vector<Cell*>::iterator it = m_transitions.begin();
		for (; it != m_transitions.end(); ++it) {
			if (cell == *it) {
				m_transitions.erase(it);
				break;
			}
		}
	}

	std::vector<Cell*> CellCache::getTransitionCells(Layer* layer) {
		if (!layer) {
			return m_transitions;
		}
		std::vector<Cell*> cells;
		std::vector<Cell*>::iterator it = m_transitions.begin();
		for (; it != m_transitions.end(); ++it) {
			TransitionInfo* trans = (*it)->getTransition();
			if (trans) {
				if (trans->m_layer == layer) {
					cells.push_back(*it);
				}
			}
		}
		return cells;
	}

	Zone* CellCache::createZone() {
		uint32_t id = 0;
		bool search = true;
		while (search) {
			bool found = false;
			if (!m_zones.empty()) {
				for (std::vector<Zone*>::iterator i = m_zones.begin(); i != m_zones.end(); ++i) {
					if ((*i)->getId() == id) {
						found = true;
						++id;
						break;
					}
				}
			}
			search = found;
		}
		Zone* zi = new Zone(id);
		m_zones.push_back(zi);

		return zi;
	}

	const std::vector<Zone*>& CellCache::getZones() {
		return m_zones;
	}

	Zone* CellCache::getZone(uint32_t id) {
		Zone* zi = 0;
		for (std::vector<Zone*>::iterator i = m_zones.begin(); i != m_zones.end(); ++i) {
			if ((*i)->getId() == id) {
				zi = (*i);
				break;
			}
		}

		if (!zi) {
			zi = new Zone(id);
			m_zones.push_back(zi);
		}

		return zi;
	}

	void CellCache::removeZone(Zone* zone) {
		for (std::vector<Zone*>::iterator i = m_zones.begin(); i != m_zones.end(); ++i) {
			if (*i == zone) {
				delete *i;
				m_zones.erase(i);
				break;
			}
		}
	}

	void CellCache::splitZone(Cell* cell) {
		Zone* currentZone = cell->getZone();
		if (!currentZone) {
			return;
		}

		Zone* newZone = createZone();
		std::stack<Cell*> cellstack;
		const std::vector<Cell*>& neighbors = cell->getNeighbors();
		for (std::vector<Cell*>::const_iterator nit = neighbors.begin(); nit != neighbors.end(); ++nit) {
			Cell* nc = *nit;
			if (nc->isInserted() && !nc->isZoneProtected() &&
				nc->getCellType() != CTYPE_STATIC_BLOCKER && nc->getCellType() != CTYPE_CELL_BLOCKER) {
					cellstack.push(nc);
					break;
			}
		}

		while(!cellstack.empty()) {
			Cell* c = cellstack.top();
			cellstack.pop();

			currentZone->removeCell(c);
			newZone->addCell(c);
			c->setInserted(true);
			if (c->isZoneProtected()) {
				continue;
			}
			const std::vector<Cell*>& neigh = c->getNeighbors();
			for (std::vector<Cell*>::const_iterator nit = neigh.begin(); nit != neigh.end(); ++nit) {
				Cell* nc = *nit;
				if (nc->getZone() == currentZone && nc->isInserted() &&
					nc->getCellType() != CTYPE_STATIC_BLOCKER && nc->getCellType() != CTYPE_CELL_BLOCKER) {
					cellstack.push(nc);
					nc->setInserted(false);
				}
			}
		}
		if (currentZone->getCellCount() == 0) {
			removeZone(currentZone);
		}
	}

	void CellCache::mergeZones(Zone* zone1, Zone* zone2) {
		if (!zone1 || !zone2) {
			return;
		}
		Zone* addZone = zone2;
		Zone* oldZone = zone1;
		if (zone1->getCellCount() > zone2->getCellCount()) {
			addZone = zone1;
			oldZone = zone2;
		}
		addZone->mergeZone(oldZone);
		removeZone(oldZone);
	}

	void CellCache::addNarrowCell(Cell* cell) {
		std::pair<std::set<Cell*>::iterator, bool> insertiter = m_narrowCells.insert(cell);
		if (insertiter.second) {
			cell->addChangeListener(m_cellZoneListener);
		}
	}

	const std::set<Cell*>& CellCache::getNarrowCells() {
		return m_narrowCells;
	}

	void CellCache::removeNarrowCell(Cell* cell) {
		std::set<Cell*>::iterator it = m_narrowCells.find(cell);
		if (it != m_narrowCells.end()) {
			(*it)->removeChangeListener(m_cellZoneListener);
			m_narrowCells.erase(it);
		}
	}

	void CellCache::resetNarrowCells() {
		std::set<Cell*>::const_iterator it = m_narrowCells.begin();
		for (; it != m_narrowCells.end(); ++it) {
			(*it)->removeChangeListener(m_cellZoneListener);
		}
		m_narrowCells.clear();
	}

	bool CellCache::isSearchNarrowCells() {
		return m_searchNarrow;
	}

	void CellCache::setSearchNarrowCells(bool search) {
		m_searchNarrow = search;
	}

	void CellCache::addCellToArea(const std::string& id, Cell* cell) {
		m_cellAreas.insert(std::pair<std::string, Cell*>(id, cell));
	}

	void CellCache::addCellsToArea(const std::string& id, const std::vector<Cell*>& cells) {
		std::vector<Cell*>::const_iterator it = cells.begin();
		for (; it != cells.end(); ++it) {
			addCellToArea(id, *it);
		}
	}

	void CellCache::removeCellFromArea(Cell* cell) {
		StringCellIterator it = m_cellAreas.begin();
		while (it != m_cellAreas.end()) {
			if ((*it).second == cell) {
				m_cellAreas.erase(it++);
			} else {
				++it;
			}
		}
	}

	void CellCache::removeCellFromArea(const std::string& id, Cell* cell) {
		StringCellPair result = m_cellAreas.equal_range(id);
		StringCellIterator it = result.first;
		for (; it != result.second; ++it) {
			if ((*it).second == cell) {
				m_cellAreas.erase(it);
				break;
			}
		}
	}

	void CellCache::removeCellsFromArea(const std::string& id, const std::vector<Cell*>& cells) {
		std::vector<Cell*>::const_iterator it = cells.begin();
		for (; it != cells.end(); ++it) {
			removeCellFromArea(id, *it);
		}
	}

	void CellCache::removeArea(const std::string& id) {
		m_cellAreas.erase(id);
	}

	bool CellCache::existsArea(const std::string& id) {
		StringCellIterator it = m_cellAreas.find(id);
		if (it == m_cellAreas.end()) {
			return false;
		}
		return true;
	}

	std::vector<std::string> CellCache::getAreas() {
		std::vector<std::string> areas;
		std::string last("");
		StringCellIterator it = m_cellAreas.begin();
		for (; it != m_cellAreas.end(); ++it) {
			if (last != (*it).first) {
				last = (*it).first;
				areas.push_back(last);
			}
		}
		return areas;
	}

	std::vector<std::string> CellCache::getCellAreas(Cell* cell) {
		std::vector<std::string> areas;
		StringCellIterator it = m_cellAreas.begin();
		for (; it != m_cellAreas.end(); ++it) {
			if ((*it).second == cell) {
				areas.push_back((*it).first);
			}
		}
		return areas;
	}

	std::vector<Cell*> CellCache::getAreaCells(const std::string& id) {
		std::vector<Cell*> cells;
		StringCellPair result = m_cellAreas.equal_range(id);
		StringCellIterator it = result.first;
		for (; it != result.second; ++it) {
			cells.push_back((*it).second);
		}
		return cells;
	}

	bool CellCache::isCellInArea(const std::string& id, Cell* cell) {
		StringCellPair result = m_cellAreas.equal_range(id);
		StringCellIterator it = result.first;
		for (; it != result.second; ++it) {
			if ((*it).second == cell) {
				return true;
			}
		}
		return false;
	}

	Rect CellCache::calculateCurrentSize() {
		// set base size
		ModelCoordinate min, max;
		m_layer->getMinMaxCoordinates(min, max);
		Rect newsize(min.x, min.y, max.x, max.y);

		const std::vector<Layer*>& interacts = m_layer->getInteractLayers();
		if (!interacts.empty()) {
			std::vector<Layer*>::const_iterator layit = interacts.begin();
			for(; layit != interacts.end(); ++layit) {
				// set size
				(*layit)->getMinMaxCoordinates(min, max, m_layer);
				newsize.w = std::max(max.x, newsize.w);
				newsize.h = std::max(max.y, newsize.h);
				newsize.x = std::min(min.x, newsize.x);
				newsize.y = std::min(min.y, newsize.y);
			}
		}
		return newsize;
	}

	void CellCache::setStaticSize(bool staticSize) {
		m_staticSize = staticSize;
	}

	bool CellCache::isStaticSize() {
		return m_staticSize;
	}

	void CellCache::setBlockingUpdate(bool update) {
		m_blockingUpdate = update;
	}

	void CellCache::setFowUpdate(bool update) {
		m_fowUpdate = update;
	}

	void CellCache::setSizeUpdate(bool update) {
		m_sizeUpdate = update;
	}

	void CellCache::update() {
		m_updated = m_fowUpdate;
		m_fowUpdate = false;
		if (m_sizeUpdate) {
			resize();
			m_sizeUpdate = false;
		}
		m_blockingUpdate = false;
	}
} // FIFE
