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
#include "model/structures/trigger.h"
#include "model/structures/triggercontroller.h"
%}

namespace FIFE {
	class Map;
	class Layer;
	class Instance;

	enum TriggerCondition {
		CELL_TRIGGER_ENTER = 0,
		CELL_TRIGGER_EXIT,
		CELL_TRIGGER_BLOCKING_CHANGE,
		INSTANCE_TRIGGER_LOCATION,
		INSTANCE_TRIGGER_ROTATION,
		INSTANCE_TRIGGER_SPEED,
		INSTANCE_TRIGGER_ACTION,
		INSTANCE_TRIGGER_TIME_MULTIPLIER,
		INSTANCE_TRIGGER_SAYTEXT,
		INSTANCE_TRIGGER_BLOCK,
		INSTANCE_TRIGGER_CELL,
		INSTANCE_TRIGGER_TRANSPARENCY,
		INSTANCE_TRIGGER_VISIBLE,
		INSTANCE_TRIGGER_STACKPOS,
		INSTANCE_TRIGGER_VISUAL,
		INSTANCE_TRIGGER_DELETE
	};

	%feature("director") ITriggerListener;
	class ITriggerListener {
	public:
		virtual ~ITriggerListener() {};

		virtual void onTriggered() = 0;
	};

	class Trigger : public FifeClass {
	public:
		virtual ~Trigger();

		void addTriggerListener(ITriggerListener* listener);
		void removeTriggerListener(ITriggerListener* listener);
		void reset();
		const std::string& getName() const;
		bool isTriggered();
		void setTriggered();
		void addTriggerCondition(TriggerCondition type);
		const std::vector<TriggerCondition>& getTriggerConditions();
		void removeTriggerCondition(TriggerCondition type);
		void enableForInstance(Instance* instance);
		const std::vector<Instance*>& getEnabledInstances();
		void disableForInstance(Instance* instance);
		void enableForAllInstances();
		bool isEnabledForAllInstances();
		void disableForAllInstances();
		void assign(Layer* layer, const ModelCoordinate& pt);
		void remove(Layer* layer, const ModelCoordinate& pt);
		void assign(Cell* cell);
		void remove(Cell* cell);
		void attach(Instance* instance);
		void detach();
		Instance* getAttached();
		void moveTo(const ModelCoordinate& newPos, const ModelCoordinate& oldPos);
		const std::vector<Cell*>& getAssignedCells();
	private:
		Trigger();
		Trigger(const std::string& name);
	};

	class TriggerController : public FifeClass {
	public:
		virtual ~TriggerController();

		Trigger* createTrigger(const std::string& triggerName);
		Trigger* createTriggerOnCoordinate(const std::string& triggerName, Layer* layer, const ModelCoordinate& pt);
		Trigger* createTriggerOnCoordinates(const std::string& triggerName, Layer* layer, const std::vector<ModelCoordinate>& coords);
		Trigger* createTriggerOnRect(const std::string& triggerName, Layer* layer, const Rect& rec);
		Trigger* createTriggerOnLocation(const std::string& triggerName, const Location& loc);
		Trigger* createTriggerOnLocations(const std::string& triggerName, const std::vector<Location>& locs);
		Trigger* createTriggerOnCell(const std::string& triggerName, Cell* cell);
		Trigger* createTriggerOnCells(const std::string& triggerName, const std::vector<Cell*>& cells);
		Trigger* createTriggerOnInstance(const std::string& triggerName, Instance* instance);
		Trigger* getTrigger(const std::string& triggerName);
		void deleteTrigger(const std::string& triggerName);
		void removeTriggerFromCoordinate(const std::string& triggerName, Layer* layer, const ModelCoordinate& pt);
		void removeTriggerFromCoordinates(const std::string& triggerName, Layer* layer, const std::vector<ModelCoordinate>& coords);
		void removeTriggerFromRect(const std::string& triggerName, Layer* layer, const Rect& rec);
		void removeTriggerFromLocation(const std::string& triggerName, const Location& loc);
		void removeTriggerFromLocations(const std::string& triggerName, const std::vector<Location>& locs);
		void removeTriggerFromCell(const std::string& triggerName, Cell* cell);
		void removeTriggerFromCells(const std::string& triggerName, const std::vector<Cell*>& cells);
		void removeTriggerFromInstance(const std::string& triggerName, Instance* instance);
		std::vector<Trigger*> getAllTriggers();
		std::vector<std::string> getAllTriggerNames();
	private:
		TriggerController(Map* map);
	};
} //FIFE

namespace std {
	%template(TriggerConditionVector) vector<FIFE::TriggerCondition>;
	%template(TriggerVector) vector<FIFE::Trigger*>;
}