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
#include "util/log/logger.h"
#include "model/metamodel/object.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"

#include "route.h"

namespace FIFE {

	static Logger _log(LM_STRUCTURES);

	Route::Route(const Location& start, const Location& end):
		m_status(ROUTE_CREATED),
		m_startNode(start),
		m_endNode(end),
		m_walked(0),
		m_sessionId(-1),
		m_rotation(0),
		m_replanned(false),
		m_ignoresBlocker(false),
		m_costId(""),
		m_object(NULL) {
	}

	Route::~Route() {
	}

	void Route::setRouteStatus(RouteStatusInfo status) {
		if (m_status != status) {
			m_status = status;
		}
	}

	RouteStatusInfo Route::getRouteStatus() {
		return m_status;
	}

	void Route::setStartNode(const Location& node) {
		m_startNode = node;
		if (m_status != ROUTE_CREATED) {
			m_status = ROUTE_CREATED;
			if (!m_path.empty()) {
				m_path.clear();
			}
			m_walked = 1;
		}
	}

	const Location& Route::getStartNode() {
		return m_startNode;
	}

	void Route::setEndNode(const Location& node) {
		if (m_status != ROUTE_CREATED) {
			m_status = ROUTE_CREATED;
			if (!m_path.empty()) {
				m_startNode = *m_current;
				m_path.clear();
			}
			m_walked = 1;
		}
		m_endNode = node;
	}

	const Location& Route::getEndNode() {
		return m_endNode;
	}

	const Location& Route::getCurrentNode() {
		if (m_path.empty()) {
			return m_startNode;
		}
		if (m_current == m_path.end()) {
			return m_path.back();
		}
		return *m_current;
	}

	const Location& Route::getPreviousNode() {
		if (m_path.empty()) {
			return m_startNode;
		}
		if (m_current != m_path.begin()) {
			--m_current;
			const Location& loc = *m_current;
			++m_current;
			return loc;
		}
		return *m_current;
	}

	const Location& Route::getNextNode() {
		if (m_path.empty()) {
			return m_startNode;
		}
		if (m_current != m_path.end()) {
			++m_current;
			if (m_current != m_path.end()) {
				const Location& loc = *m_current;
				--m_current;
				return loc;
			}
			--m_current;
		}
		return *m_current;
	}

	bool Route::walkToNextNode(int32_t step) {
		if (m_path.empty() || step == 0) {
			return false;
		}

		int32_t pos = static_cast<int32_t>(m_walked) + step;
		if (pos > static_cast<int32_t>(m_path.size()) || pos < 0) {
			return false;
		}
		if (step > 0) {
			for (int32_t i = 0; i < step; ++i, ++m_current);
		} else {
			for (int32_t i = 0; i > step; --i, --m_current);
		}
		m_walked += step;

		return true;
	}

	bool Route::reachedEnd() {
		if (m_path.empty()) {
			return true;
		}
		return m_current == m_path.end();
	}

	void Route::setPath(const Path& path) {
		m_path = path;
		if (!m_path.empty()) {
			m_status = ROUTE_SOLVED;
			m_current = m_path.begin();
			m_startNode = m_path.front();
			m_endNode = m_path.back();
		}
		if (!isMultiCell()) {
			m_replanned = false;
		}
		m_walked = 1;
	}

	Path Route::getPath() {
		return m_path;
	}

	void Route::cutPath(uint32_t length) {
		if (length == 0) {
			if (!m_path.empty()) {
				m_startNode = *m_current;
				m_endNode = *m_current;
				m_path.clear();
				m_current = m_path.end();
			}
			m_status = ROUTE_CREATED;
			m_walked = 1;
			m_replanned = true;
			return;
		} else if (length >= m_path.size()) {
			return;
		}

		uint32_t newend = m_walked + length - 1;
		if (newend > m_path.size()) {
			return;
		}

		m_path.resize(newend);
		m_endNode = m_path.back();
		m_replanned = true;
	}

	void Route::setReplanned(bool replanned) {
		m_replanned = replanned;
	}

	bool Route::isReplanned() {
		return m_replanned;
	}

	uint32_t Route::getPathLength() {
		return m_path.size();
	}

	uint32_t Route::getWalkedLength() {
		return m_walked;
	}

	void Route::setSessionId(int32_t id) {
		m_sessionId = id;
	}

	int32_t Route::getSessionId() {
		return m_sessionId;
	}

	void Route::setRotation(int32_t rotation) {
		m_rotation = rotation;
	}

	int32_t Route::getRotation() {
		return m_rotation;
	}

	void Route::setCostId(const std::string& cost) {
		m_costId = cost;
	}

	const std::string& Route::getCostId() {
		return m_costId;
	}

	bool Route::isMultiCell() {
		if (m_object) {
			return m_object->isMultiObject();
		}
		return false;
	}

	void Route::setOccupiedArea(const std::vector<ModelCoordinate>& area) {
		m_area = area;
	}

	const std::vector<ModelCoordinate>& Route::getOccupiedArea() {
		return m_area;
	}

	std::vector<ModelCoordinate> Route::getOccupiedCells(int32_t rotation) {
		if (m_object) {
			return m_object->getMultiObjectCoordinates(rotation);
		}
		std::vector<ModelCoordinate> coords;
		return coords;
	}

	int32_t Route::getZStepRange() {
		if (!m_object) {
			return -1;
		}
		return m_object->getZStepRange();
	}

	bool Route::isAreaLimited() {
		if (m_object) {
			if (!m_object->getWalkableAreas().empty()) {
				return true;
			}
		}
		return false;
	}

	const std::list<std::string> Route::getLimitedAreas() {
		std::list<std::string> areas;
		if (m_object) {
			areas = m_object->getWalkableAreas();
		}
		return areas;
	}

	void Route::setDynamicBlockerIgnored(bool ignore) {
		m_ignoresBlocker = ignore;
	}

	bool Route::isDynamicBlockerIgnored() {
		return m_ignoresBlocker;
	}

	Path Route::getBlockingPathLocations() {
		Path p;
		if (!m_path.empty()) {
			for (PathIterator it = m_path.begin(); it != m_path.end(); ++it) {
				Layer* layer = (*it).getLayer();
				if (layer->cellContainsBlockingInstance((*it).getLayerCoordinates())) {
					p.push_back(*it);
				}
			}
		}
		return p;
	}

	void Route::setObject(Object* obj) {
		m_object = obj;
	}

	Object* Route::getObject() {
		return m_object;
	}
} // FIFE
