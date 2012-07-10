/***************************************************************************
 *   Copyright (C) 2005-2012 by the FIFE team                              *
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
#include <iostream>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"
#include "util/base/exception.h"
#include "util/math/fife_math.h"
#include "util/time/timemanager.h"
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/ipather.h"
#include "model/metamodel/action.h"
#include "model/metamodel/timeprovider.h"
#include "model/structures/layer.h"
#include "model/structures/map.h"
#include "model/structures/instancetree.h"
#include "view/visual.h"
#include "pathfinder/route.h"

#include "instance.h"

namespace FIFE {
	static Logger _log(LM_INSTANCE);

	class ActionInfo {
	public:
		ActionInfo(IPather* pather, const Location& curloc):
			m_action(NULL),
			m_target(NULL),
			m_speed(0),
			m_repeating(false),
			m_action_start_time(0),
			m_action_offset_time(0),
			m_prev_call_time(0),
			m_pather(pather),
			m_leader(NULL),
			m_route(NULL),
			m_delete_route(true) {}

		~ActionInfo() {
			if (m_route) {
				int32_t sessionId = m_route->getSessionId();
				if (sessionId != -1) {
					m_pather->cancelSession(sessionId);
				}
				if (m_delete_route) {
					delete m_route;
				} else {
					m_route->setSessionId(-1);
				}
			}
			delete m_target;
		}

		// Current action, owned by object
		Action* m_action;
		// target location for ongoing movement
		Location* m_target;
		// current movement speed
		double m_speed;
		// should action be repeated? used only for non-moving actions, moving ones repeat until movement is finished
		bool m_repeating;
		// action start time (ticks)
		uint32_t m_action_start_time;
		// action offset time (ticks) for resuming an action
		uint32_t m_action_offset_time;
		// ticks since last call
		uint32_t m_prev_call_time;
		// pather
		IPather* m_pather;
		// leader for follow activity
		Instance* m_leader;
		// pointer to route that contain path and additional information
		Route* m_route;
		bool m_delete_route;
	};

	class SayInfo {
	public:
		SayInfo(const std::string& txt, uint32_t duration):
			m_txt(txt),
			m_duration(duration),
			m_start_time(0) {}

		std::string m_txt;
		uint32_t m_duration;
		uint32_t m_start_time;
	};

	Instance::InstanceActivity::InstanceActivity(Instance& source):
		m_location(source.m_location),
		m_oldLocation(source.m_location),
		m_rotation(source.m_rotation),
		m_oldRotation(source.m_rotation),
		m_action(),
		m_speed(0),
		m_timeMultiplier(1.0),
		m_sayText(""),
 		m_changeListeners(),
		m_actionListeners(),
		m_actionInfo(NULL),
		m_sayInfo(NULL),
		m_timeProvider(NULL),
		m_blocking(source.m_blocking) {
	}

	Instance::InstanceActivity::~InstanceActivity() {
		delete m_actionInfo;
		delete m_sayInfo;
		delete m_timeProvider;
	}

	void Instance::InstanceActivity::update(Instance& source) {
		source.m_changeInfo = ICHANGE_NO_CHANGES;
		if (m_location != source.m_location) {
			source.m_changeInfo |= ICHANGE_LOC;
			if (m_location.getLayerCoordinates() != source.m_location.getLayerCoordinates()) {
				m_oldLocation.setLayer(m_location.getLayer());
				m_oldLocation.setLayerCoordinates(m_location.getLayerCoordinates());
				source.m_changeInfo |= ICHANGE_CELL;
			}
			m_location = source.m_location;
		}
		if (m_rotation != source.m_rotation) {
			m_oldRotation = m_rotation;
			source.m_changeInfo |= ICHANGE_ROTATION;
			m_rotation = source.m_rotation;
		}
		if (m_actionInfo && (m_speed != m_actionInfo->m_speed)) {
			source.m_changeInfo |= ICHANGE_SPEED;
			m_speed = m_actionInfo->m_speed;
		}
		if (m_actionInfo && (m_action != m_actionInfo->m_action)) {
			source.m_changeInfo |= ICHANGE_ACTION;
			m_action = m_actionInfo->m_action;
		}
		if (m_timeProvider && (m_timeMultiplier != m_timeProvider->getMultiplier())) {
			source.m_changeInfo |= ICHANGE_TIME_MULTIPLIER;
			m_timeMultiplier = m_timeProvider->getMultiplier();
		}
		if (m_sayInfo && (m_sayText != m_sayInfo->m_txt)) {
			source.m_changeInfo |= ICHANGE_SAYTEXT;
			m_sayText = m_sayInfo->m_txt;
		}
		if (m_blocking != source.m_blocking) {
			source.m_changeInfo |= ICHANGE_BLOCK;
			m_blocking = source.m_blocking;
		}

		if (source.m_changeInfo != ICHANGE_NO_CHANGES) {
			std::vector<InstanceChangeListener*>::iterator i = m_changeListeners.begin();
			while (i != m_changeListeners.end()) {
				if (NULL != *i)
				{
					(*i)->onInstanceChanged(&source, source.m_changeInfo);
				}
				++i;
			}
			// Really remove "removed" listeners.
			m_changeListeners.erase(
				std::remove(m_changeListeners.begin(),m_changeListeners.end(),
					(InstanceChangeListener*)NULL),
				m_changeListeners.end());
		}
	}

	Instance::Instance(Object* object, const Location& location, const std::string& identifier):
		m_id(identifier),
		m_rotation(0),
		m_activity(NULL),
		m_changeInfo(ICHANGE_NO_CHANGES),
		m_object(object),
		m_location(location),
		m_visual(NULL),
		m_blocking(object->isBlocking()),
		m_overrideBlocking(false),
		m_isVisitor(false),
		m_visitorRadius(0),
		m_cellStackPos(object->getCellStackPosition()),
		m_specialCost(object->isSpecialCost()),
		m_cost(object->getCost()),
		m_costId(object->getCostId()) {
		// create multi object instances
		if (object->isMultiObject()) {
			uint32_t count = 0;
			Layer* layer = m_location.getLayer();
			const ExactModelCoordinate& emc = m_location.getExactLayerCoordinatesRef();
			const std::set<Object*>& multis = object->getMultiParts();
			std::set<Object*>::const_iterator it = multis.begin();
			for (; it != multis.end(); ++it, ++count) {
				if (*it == m_object) {
					continue;
				}
				std::vector<ModelCoordinate> partcoords = (*it)->getMultiPartCoordinates(m_rotation);
				std::vector<ModelCoordinate>::iterator coordit = partcoords.begin();
				for (; coordit != partcoords.end(); ++coordit) {
					ExactModelCoordinate tmp_emc(emc.x+(*coordit).x, emc.y+(*coordit).y, emc.z+(*coordit).z);
					std::ostringstream counter;
					counter << count;
					Instance* instance = layer->createInstance(*it, tmp_emc, identifier+counter.str());
					InstanceVisual* instVisual = InstanceVisual::create(instance);
					m_multiInstances.push_back(instance);
					instance->addDeleteListener(this);
				}
			}
		}
	}

	Instance::~Instance() {
		std::vector<InstanceDeleteListener *>::iterator itor;
		for(itor = m_deleteListeners.begin(); itor != m_deleteListeners.end(); ++itor) {
			if (*itor != NULL) {
				(*itor)->onInstanceDeleted(this);
			}
		}

		if(m_activity && m_activity->m_actionInfo) {
			// Don't ditribute onActionFinished in case we're already
			// deleting.
			m_activity->m_actionListeners.clear();
			finalizeAction();
		}

		if (!m_multiInstances.empty()) {
			std::vector<Instance*>::iterator it = m_multiInstances.begin();
			for (; it != m_multiInstances.end(); ++it) {
				(*it)->removeDeleteListener(this);
			}
		}

		delete m_activity;
		delete m_visual;
	}

	void Instance::initializeChanges() {
		if (!m_activity) {
			m_activity = new InstanceActivity(*this);
		}
		if (m_location.getLayer()) {
			m_location.getLayer()->setInstanceActivityStatus(this, true);
		}
	}

	bool Instance::isActive() const {
		return (m_activity != 0);
	}

	Object* Instance::getObject() {
		return m_object;
	}

	void Instance::setLocation(const Location& loc) {
		// ToDo: Handle the case when the layers are different
		if(m_location != loc) {
			if(isActive()) {
				refresh();
				if (m_location.getLayerCoordinates() != loc.getLayerCoordinates()) {
					m_location.getLayer()->getInstanceTree()->removeInstance(this);
					m_location = loc;
					m_location.getLayer()->getInstanceTree()->addInstance(this);
				} else {
					m_location = loc;
				}
			} else {
				initializeChanges();
				if (m_location.getLayerCoordinates() != loc.getLayerCoordinates()) {
					m_location.getLayer()->getInstanceTree()->removeInstance(this);
					m_location = loc;
					m_location.getLayer()->getInstanceTree()->addInstance(this);
				} else {
					m_location = loc;
				}
			}
		}
	}

	Location Instance::getLocation() const {
		return m_location;
	}

	Location& Instance::getLocationRef() {
		return m_location;
	}

	void Instance::setRotation(int32_t rotation) {
		while (rotation < 0) {
			rotation += 360;
		}
		rotation %= 360;
		if(m_rotation != rotation) {
			if(isActive()) {
				refresh();
				m_rotation = rotation;
			} else {
				initializeChanges();
				m_rotation = rotation;
			}
		}
	}

	int32_t Instance::getRotation() const {
		return m_rotation;
	}

	void Instance::setId(const std::string& identifier) {
		m_id = identifier;
	}

	const std::string& Instance::getId() {
		return m_id;
	}

	void Instance::setBlocking(bool blocking) {
		if (m_overrideBlocking) {
			m_blocking = blocking;
		}
	}

	bool Instance::isBlocking() const {
		return m_blocking;
	}

	void Instance::setOverrideBlocking(bool overblock) {
		m_overrideBlocking = overblock;
	}

	bool Instance::isOverrideBlocking() const {
		return m_overrideBlocking;
	}

	void Instance::addActionListener(InstanceActionListener* listener) {
		initializeChanges();
		m_activity->m_actionListeners.push_back(listener);
	}

	void Instance::removeActionListener(InstanceActionListener* listener) {
		if (!m_activity) {
			return;
		}
		std::vector<InstanceActionListener*>::iterator i = m_activity->m_actionListeners.begin();
		while (i != m_activity->m_actionListeners.end()) {
			if ((*i) == listener) {
				*i = NULL;
				return;
			}
			++i;
		}
		FL_WARN(_log, "Cannot remove unknown listener");
	}

	void Instance::addChangeListener(InstanceChangeListener* listener) {
		initializeChanges();
		m_activity->m_changeListeners.push_back(listener);
	}

	void Instance::callOnActionFrame(Action* action, int32_t frame) {
		if (!m_activity) {
			return;
		}

		std::vector<InstanceActionListener*>::iterator i = m_activity->m_actionListeners.begin();
		while (i != m_activity->m_actionListeners.end()) {
			if(*i) {
				(*i)->onInstanceActionFrame(this, action, frame);
			}
			++i;
		}
	}

	void Instance::removeChangeListener(InstanceChangeListener* listener) {
		if (!m_activity) {
			return;
		}
		std::vector<InstanceChangeListener*>::iterator i = m_activity->m_changeListeners.begin();
		while (i != m_activity->m_changeListeners.end()) {
			if ((*i) == listener) {
				*i = NULL;
				return;
			}
			++i;
		}
		FL_WARN(_log, "Cannot remove unknown listener");
	}
	void Instance::initializeAction(const std::string& actionName) {
		assert(m_object);

		initializeChanges();
		const Action *old_action = m_activity->m_actionInfo ? m_activity->m_actionInfo->m_action : NULL;
		if (m_activity->m_actionInfo) {
			delete m_activity->m_actionInfo;
			m_activity->m_actionInfo = NULL;
		}
		m_activity->m_actionInfo = new ActionInfo(m_object->getPather(), m_location);
		m_activity->m_actionInfo->m_action = m_object->getAction(actionName);
		if (!m_activity->m_actionInfo->m_action) {
			delete m_activity->m_actionInfo;
			m_activity->m_actionInfo = NULL;
			throw NotFound(std::string("action ") + actionName + " not found");
		}
		m_activity->m_actionInfo->m_prev_call_time = getRuntime();
		if (m_activity->m_actionInfo->m_action != old_action) {
			m_activity->m_actionInfo->m_action_start_time = m_activity->m_actionInfo->m_prev_call_time;
	    }
		if (isMultiObject()) {
			std::vector<Instance*>::iterator multi_it = m_multiInstances.begin();
			for (; multi_it != m_multiInstances.end(); ++multi_it) {
				(*multi_it)->initializeAction(actionName);
			}
		}
	}

	void Instance::move(const std::string& actionName, const Location& target, const double speed, const std::string& costId) {
		// if new move is identical with the old then return
		if (m_activity) {
			if (m_activity->m_actionInfo) {
				if (m_activity->m_actionInfo->m_target) {
					if (m_activity->m_actionInfo->m_target->getLayerCoordinates() == target.getLayerCoordinates() &&
						Mathd::Equal(speed, m_activity->m_actionInfo->m_speed) &&
						m_activity->m_actionInfo->m_action == m_object->getAction(actionName) &&
						costId == m_activity->m_actionInfo->m_route->getCostId()) {

						return;
					}
				}
			}
		}
		initializeAction(actionName);
		m_activity->m_actionInfo->m_target = new Location(target);
		m_activity->m_actionInfo->m_speed = speed;
		setFacingLocation(target);
		FL_DBG(_log, LMsg("starting action ") <<  actionName << " from" << m_location << " to " << target << " with speed " << speed);

		Route* route = m_activity->m_actionInfo->m_route;
		if (!route) {
			route = new Route(m_location, *m_activity->m_actionInfo->m_target);
			if (costId != "") {
				route->setCostId(costId);
			}
			if (isMultiCell()) {
				route->setObject(m_object);
				route->setOccupiedArea(m_location.getLayer()->getCellGrid()->
					toMultiCoordinates(m_location.getLayerCoordinates(), m_object->getMultiObjectCoordinates(m_rotation)));
			}
			m_activity->m_actionInfo->m_route = route;
			if (!m_activity->m_actionInfo->m_pather->solveRoute(route)) {
				finalizeAction();
			}
		}
	}

	void Instance::follow(const std::string& actionName, Instance* leader, const double speed) {
		initializeAction(actionName);
		m_activity->m_actionInfo->m_target = new Location(leader->getLocationRef());
		m_activity->m_actionInfo->m_speed = speed;
		m_activity->m_actionInfo->m_leader = leader;
		leader->addDeleteListener(this);
		setFacingLocation(*m_activity->m_actionInfo->m_target);
		FL_DBG(_log, LMsg("starting action ") <<  actionName << " from" << m_location << " to " << *m_activity->m_actionInfo->m_target << " with speed " << speed);
	}

	void Instance::follow(const std::string& actionName, Route* route, const double speed) {
		// if new follow is identical with the old then return
		if (m_activity) {
			if (m_activity->m_actionInfo) {
				if (m_activity->m_actionInfo->m_target) {
					if (m_activity->m_actionInfo->m_target->getLayerCoordinates() == route->getEndNode().getLayerCoordinates() &&
						Mathd::Equal(speed, m_activity->m_actionInfo->m_speed) &&
						m_activity->m_actionInfo->m_action == m_object->getAction(actionName) &&
						route->getCostId() == m_activity->m_actionInfo->m_route->getCostId()) {

						return;
					}
				}
			}
		}
		initializeAction(actionName);
		m_activity->m_actionInfo->m_target = new Location(route->getEndNode());
		m_activity->m_actionInfo->m_speed = speed;
		m_activity->m_actionInfo->m_route = route;
		m_activity->m_actionInfo->m_delete_route = false;
		setFacingLocation(*m_activity->m_actionInfo->m_target);
		if (isMultiCell()) {
			route->setObject(m_object);
			route->setOccupiedArea(m_location.getLayer()->getCellGrid()->
				toMultiCoordinates(m_location.getLayerCoordinates(), m_object->getMultiObjectCoordinates(m_rotation)));
		}
		FL_DBG(_log, LMsg("starting action ") <<  actionName << " from" << m_location << " to " << *m_activity->m_actionInfo->m_target << " with speed " << speed);
	}

	void Instance::cancelMovement(uint32_t length) {
		if (m_activity) {
			ActionInfo* info = m_activity->m_actionInfo;
			if (info) {
				Route* route = info->m_route;
				if (route) {
					route->cutPath(length);
				}
			}
		}
	}

	Route* Instance::getRoute() {
		if (m_activity) {
			ActionInfo* info = m_activity->m_actionInfo;
			if (info) {
				return info->m_route;
			}
		}
		return NULL;
	}

	void Instance::setVisitor(bool visit) {
		m_isVisitor = visit;
	}

	bool Instance::isVisitor() {
		return m_isVisitor;
	}

	void Instance::setVisitorRadius(uint16_t radius) {
		m_visitorRadius = radius;
	}

	uint16_t Instance::getVisitorRadius() {
		return m_visitorRadius;
	}

	void Instance::setCellStackPosition(uint8_t stack) {
		m_cellStackPos = stack;
	}

	uint8_t Instance::getCellStackPosition() {
		return m_cellStackPos;
	}

	bool Instance::isSpecialCost() {
		return m_specialCost;
	}

	const std::vector<Instance*>& Instance::getMultiInstances() {
		return m_multiInstances;
	}

	void Instance::act(const std::string& actionName, const Location& direction, bool repeating) {
		initializeAction(actionName);
		m_activity->m_actionInfo->m_repeating = repeating;
		setFacingLocation(direction);
	}

	void Instance::act(const std::string& actionName, int32_t rotation, bool repeating) {
		initializeAction(actionName);
		m_activity->m_actionInfo->m_repeating = repeating;
		setRotation(rotation);
	}

	void Instance::act(const std::string& actionName, bool repeating) {
		initializeAction(actionName);
		m_activity->m_actionInfo->m_repeating = repeating;
	}

	void Instance::say(const std::string& text, uint32_t duration) {
		initializeChanges();
		delete m_activity->m_sayInfo;
		m_activity->m_sayInfo = NULL;

		if (text != "") {
			m_activity->m_sayInfo = new SayInfo(text, duration);
			m_activity->m_sayInfo->m_start_time = getRuntime();
		}
	}

	const std::string* Instance::getSayText() const {
		if (m_activity && m_activity->m_sayInfo) {
			return &m_activity->m_sayInfo->m_txt;
		}
		return NULL;
	}

	bool Instance::processMovement() {
		ActionInfo* info = m_activity->m_actionInfo;
		Route* route = info->m_route;
		Location target;
		if (info->m_leader) {
			target = info->m_leader->getLocationRef();
		} else {
			target = *info->m_target;
		}
		if (!route) {
			route = new Route(m_location, *info->m_target);
			info->m_route = route;
			if (isMultiCell()) {
				route->setObject(m_object);
				route->setOccupiedArea(m_location.getLayer()->getCellGrid()->
					toMultiCoordinates(m_location.getLayerCoordinates(), m_object->getMultiObjectCoordinates(m_rotation)));
			}
			if (!info->m_pather->solveRoute(route)) {
				return true;
			}
		// update target if needed
		} else if (route->getEndNode().getLayerCoordinates() != target.getLayerCoordinates()) {
			if (route->isReplanned() || isMultiCell()) {
				*info->m_target = route->getEndNode();
				route->setReplanned(false);
				if (isMultiCell()) {
					route->setOccupiedArea(m_location.getLayer()->getCellGrid()->
						toMultiCoordinates(m_location.getLayerCoordinates(), m_object->getMultiObjectCoordinates(m_rotation)));
				}
			} else {
				route->setStartNode(m_location);
				route->setEndNode(target);
				if (!info->m_pather->solveRoute(route)) {
					return true;
				}
			}
		}

		if (route->getRouteStatus() == ROUTE_SOLVED) {
			// timeslice for this movement
			uint32_t timedelta = m_activity->m_timeProvider->getGameTime() - info->m_prev_call_time;
			// how far we can travel
			double distance_to_travel = (static_cast<double>(timedelta) / 1000.0) * info->m_speed;
			// location for this movement
			Location nextLocation = m_location;
			int32_t rotation = m_rotation;
			bool can_follow = info->m_pather->followRoute(m_location, route, distance_to_travel, nextLocation, rotation);
			if (can_follow) {
				setRotation(rotation);
				// move to another layer
				if (m_location.getLayer() != nextLocation.getLayer()) {
					m_location.getLayer()->getMap()->addInstanceForTransfer(this, nextLocation);
					if (!m_multiInstances.empty()) {
						std::vector<Instance*>::iterator it = m_multiInstances.begin();
						for (; it != m_multiInstances.end(); ++it) {
							Location newloc = nextLocation;
							std::vector<ModelCoordinate> tmpcoords = m_location.getLayer()->getCellGrid()->
								toMultiCoordinates(nextLocation.getLayerCoordinates(), (*it)->getObject()->getMultiPartCoordinates(m_rotation));
							newloc.setLayerCoordinates(tmpcoords.front());
							m_location.getLayer()->getMap()->addInstanceForTransfer(*it, newloc);
						}
					}
					return false;
				}
				// move to another cell
				if (m_location.getLayerCoordinates() != nextLocation.getLayerCoordinates()) {
					m_location.getLayer()->getInstanceTree()->removeInstance(this);
					m_location = nextLocation;
					m_location.getLayer()->getInstanceTree()->addInstance(this);
				} else {
					// move on this cell
					m_location = nextLocation;
				}
				return false;
			}
			// move to another layer
			if (m_location.getLayer() != nextLocation.getLayer()) {
				m_location.getLayer()->getMap()->addInstanceForTransfer(this, nextLocation);
				if (!m_multiInstances.empty()) {
					std::vector<Instance*>::iterator it = m_multiInstances.begin();
					for (; it != m_multiInstances.end(); ++it) {
						Location newloc = nextLocation;
						std::vector<ModelCoordinate> tmpcoords = m_location.getLayer()->getCellGrid()->
							toMultiCoordinates(nextLocation.getLayerCoordinates(), (*it)->getObject()->getMultiPartCoordinates(m_rotation));
						newloc.setLayerCoordinates(tmpcoords.front());
						m_location.getLayer()->getMap()->addInstanceForTransfer(*it, newloc);
					}
				}
				return true;
			}
			m_location = nextLocation;
			// need new route?
			if (route->getEndNode().getLayerCoordinates() != m_location.getLayerCoordinates()) {
				if (m_location.getLayerDistanceTo(target) > 1.5) {
					route->setStartNode(m_location);
					route->setEndNode(target);
					route->setOccupiedArea(m_location.getLayer()->getCellGrid()->
						toMultiCoordinates(m_location.getLayerCoordinates(), m_object->getMultiObjectCoordinates(m_rotation)));
					return !info->m_pather->solveRoute(route);
				}
			}
			return true;
		} else if (route->getRouteStatus() == ROUTE_FAILED) {
			return true;
		}
		return false;
	}

	InstanceChangeInfo Instance::update() {
		if (!m_activity) {
			return ICHANGE_NO_CHANGES;
		}
		// remove DeleteListeners
		m_deleteListeners.erase(std::remove(m_deleteListeners.begin(),m_deleteListeners.end(),
				(InstanceDeleteListener*)NULL),	m_deleteListeners.end());
		m_activity->update(*this);
		if (!m_activity->m_timeProvider) {
			bindTimeProvider();
		}
		ActionInfo* info = m_activity->m_actionInfo;
		if (info) {
//			FL_DBG(_log, "updating instance");

			if (info->m_target) {
//				FL_DBG(_log, "action contains target for movement");
				bool movement_finished = processMovement();
				if (movement_finished) {
//					FL_DBG(_log, "movement finished");
					finalizeAction();
				}
			} else {
//				FL_DBG(_log, "action does not contain target for movement");
				if (m_activity->m_timeProvider->getGameTime() - info->m_action_start_time + info->m_action_offset_time >= info->m_action->getDuration()) {
					if (info->m_repeating) {
						info->m_action_start_time = m_activity->m_timeProvider->getGameTime();
						// prock: offset no longer needed
						info->m_action_offset_time = 0;
					} else if (!m_object->isMultiPart()) {
						finalizeAction();
					}
				}
			}

			// previous code may invalidate actioninfo.
			if( m_activity->m_actionInfo ) {
				m_activity->m_actionInfo->m_prev_call_time = m_activity->m_timeProvider->getGameTime();
			}
		}
		if (m_activity->m_sayInfo) {
			if (m_activity->m_sayInfo->m_duration > 0) {
				if (m_activity->m_timeProvider->getGameTime() >= m_activity->m_sayInfo->m_start_time + m_activity->m_sayInfo->m_duration) {
					say("");
				}
			}
		} else if (!m_activity->m_actionInfo && m_changeInfo == ICHANGE_NO_CHANGES && m_activity->m_actionListeners.empty()) {
			// delete superfluous activity
			delete m_activity;
			m_activity = 0;
			return ICHANGE_NO_CHANGES;
		}
		return m_changeInfo;
	}

	void Instance::finalizeAction() {
		FL_DBG(_log, "finalizing action");
		assert(m_activity);
		assert(m_activity->m_actionInfo);

		if( m_activity->m_actionInfo->m_leader ) {
			m_activity->m_actionInfo->m_leader->removeDeleteListener(this);
		}

		Action* action = m_activity->m_actionInfo->m_action;
		delete m_activity->m_actionInfo;
		m_activity->m_actionInfo = NULL;

		std::vector<InstanceActionListener*>::iterator i = m_activity->m_actionListeners.begin();
		while (i != m_activity->m_actionListeners.end()) {
			if(*i)
				(*i)->onInstanceActionFinished(this, action);
			++i;
		}
		m_activity->m_actionListeners.erase(
			std::remove(m_activity->m_actionListeners.begin(),
				m_activity->m_actionListeners.end(),
				(InstanceActionListener*)NULL),
			m_activity->m_actionListeners.end());

		if (isMultiObject()) {
			std::vector<Instance*>::iterator multi_it = m_multiInstances.begin();
			for (; multi_it != m_multiInstances.end(); ++multi_it) {
				(*multi_it)->finalizeAction();
			}
		}
	}

	Action* Instance::getCurrentAction() const {
		if (m_activity && m_activity->m_actionInfo) {
			return m_activity->m_actionInfo->m_action;
		}
		return NULL;
	}

	Location Instance::getTargetLocation() const {
		if (m_activity && m_activity->m_actionInfo && m_activity->m_actionInfo->m_target) {
			return *m_activity->m_actionInfo->m_target;
		}
		return m_location;
	}

	double Instance::getMovementSpeed() const {
		if (m_activity && m_activity->m_actionInfo) {
			return m_activity->m_actionInfo->m_speed;
		}
		return 0;
	}

	void Instance::setFacingLocation(const Location& loc) {
		setRotation(getAngleBetween(m_location, loc));
	}

	Location Instance::getFacingLocation() {
		return getFacing(m_location, m_rotation);
	}

	Location& Instance::getOldLocationRef() {
		if (m_activity) {
			return m_activity->m_oldLocation;
		}
		return m_location;
	}

	int32_t Instance::getOldRotation() const {
		if (m_activity) {
			return m_activity->m_oldRotation;
		}
		return m_rotation;
	}

	uint32_t Instance::getActionRuntime() {
		if (m_activity && m_activity->m_actionInfo) {
			if(!m_activity->m_timeProvider)
				bindTimeProvider();
			return m_activity->m_timeProvider->getGameTime() - m_activity->m_actionInfo->m_action_start_time + m_activity->m_actionInfo->m_action_offset_time;
		}
		return getRuntime();
	}

	void Instance::setActionRuntime(uint32_t time_offset) {
		m_activity->m_actionInfo->m_action_offset_time = time_offset;
	}

	void Instance::bindTimeProvider() {
		float multiplier = 1.0;
		if (m_activity->m_timeProvider) {
			multiplier = m_activity->m_timeProvider->getMultiplier();
		}
		delete m_activity->m_timeProvider;
		m_activity->m_timeProvider = NULL;

		if (m_location.getLayer()) {
			Map* map = m_location.getLayer()->getMap();
			if (map) {
				m_activity->m_timeProvider = new TimeProvider(map->getTimeProvider());
			}
		}
		if (!m_activity->m_timeProvider) {
			m_activity->m_timeProvider = new TimeProvider(NULL);
		}
		m_activity->m_timeProvider->setMultiplier(multiplier);
	}

	void Instance::refresh() {
		initializeChanges();
		bindTimeProvider();
	}

	void Instance::setTimeMultiplier(float multip) {
		initializeChanges();
		if (!m_activity->m_timeProvider) {
			bindTimeProvider();
		}
		m_activity->m_timeProvider->setMultiplier(multip);
	}

	float Instance::getTimeMultiplier() {
		if (m_activity && m_activity->m_timeProvider) {
			return m_activity->m_timeProvider->getMultiplier();
		}
		return 1.0;
	}

	float Instance::getTotalTimeMultiplier() {
		if (m_activity && m_activity->m_timeProvider) {
			return m_activity->m_timeProvider->getTotalMultiplier();
		}
		if (m_location.getLayer()) {
			Map* map = m_location.getLayer()->getMap();
			if (map && map->getTimeProvider()) {
				return map->getTimeProvider()->getTotalMultiplier();
			}
		}
		return 1.0;
	}

	uint32_t Instance::getRuntime() {
		if (m_activity) {
			if(!m_activity->m_timeProvider)
				bindTimeProvider();
			return m_activity->m_timeProvider->getGameTime();
		}
		if (m_location.getLayer()) {
			Map* map = m_location.getLayer()->getMap();
			if (map && map->getTimeProvider()) {
				return map->getTimeProvider()->getGameTime();
			}
		}
		return TimeManager::instance()->getTime();
	}

	void Instance::setCost(const std::string& id, double cost) {
		m_specialCost = true;
		m_costId = id;
		m_cost = cost;
	}

	void Instance::resetCost() {
		m_specialCost = false;
	}

	double Instance::getCost() {
		if (m_specialCost) {
			return m_cost;
		}
		return m_object->getCost();
	}

	const std::string& Instance::getCostId() {
		if (m_specialCost) {
			return m_costId;
		}
		return m_object->getCostId();
	}

	bool Instance::isMultiCell() {
		return m_object->isMultiObject();
	}

	bool Instance::isMultiObject() {
		return !m_multiInstances.empty();
	}

	void Instance::updateMultiInstances() {
		if (!m_multiInstances.empty()) {
			// use map coords for rotation and movement
			// instances are changed on InstanceTree but not on CellCache
			Location loc = m_location;
			const ExactModelCoordinate anchor = m_location.getMapCoordinates();
			const ExactModelCoordinate& offset = m_object->getRotationAnchor();
			loc.setExactLayerCoordinates(offset);
			const ExactModelCoordinate anchor_offset = loc.getMapCoordinates();
			int32_t rot = m_rotation;
			if (m_object->isRestrictedRotation()) {
				rot = m_object->getRestrictedRotation(m_rotation);
			}
			double mcos = Mathd::Cos(double(rot) * (Mathd::pi()/180.0));
			double msin = Mathd::Sin(double(rot) * (Mathd::pi()/180.0));
			std::vector<Instance*>::iterator it = m_multiInstances.begin();
			for (; it != m_multiInstances.end(); ++it) {
				// use rotation 0 to get the "default" coordinate
				std::vector<ModelCoordinate> mcv = (*it)->getObject()->getMultiPartCoordinates(0);
				loc.setLayerCoordinates(mcv.front());
				ExactModelCoordinate emc = loc.getMapCoordinates();
				ExactModelCoordinate nemc(emc.x-anchor_offset.x, emc.y-anchor_offset.y);
				emc.x = ((nemc.x * mcos + nemc.y * msin) + anchor_offset.x) + anchor.x;
				emc.y = ((-nemc.x * msin + nemc.y * mcos) + anchor_offset.y) + anchor.y;
				loc.setMapCoordinates(emc);
				(*it)->setLocation(loc);
				(*it)->setRotation(rot);
			}
		}
	}

	void Instance::addDeleteListener(InstanceDeleteListener *listener) {
		m_deleteListeners.push_back(listener);
	}

	void Instance::removeDeleteListener(InstanceDeleteListener *listener) {
		if (!m_deleteListeners.empty()) {
			std::vector<InstanceDeleteListener*>::iterator itor;
			itor = std::find(m_deleteListeners.begin(), m_deleteListeners.end(), listener);
			if(itor != m_deleteListeners.end()) {
				if ((*itor) == listener) {
					*itor = NULL;
					return;
				}
			} else {
				FL_WARN(_log, "Cannot remove unknown listener");
			}
		}
	}

	void Instance::onInstanceDeleted(Instance* instance) {
		if(m_activity && m_activity->m_actionInfo &&
			m_activity->m_actionInfo->m_leader == instance) {
				m_activity->m_actionInfo->m_leader = NULL;
		}
		if (isMultiObject()) {
			std::vector<Instance*>::iterator multi_it = m_multiInstances.begin();
			for (; multi_it != m_multiInstances.end(); ++multi_it) {
				if (*multi_it == instance) {
					m_multiInstances.erase(multi_it);
					break;
				}
			}
		}
	}
}
