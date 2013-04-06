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

%module fife
%{
#include "model/structures/instance.h"
%}

%include "model/metamodel/modelcoords.i"
%include "model/metamodel/ivisual.i"
%include "util/structures/utilstructures.i"
%include "util/base/utilbase.i"
%include "location.i"

namespace FIFE {

	class Layer;
	class Action;
	class Object;
	class Instance;
	class Route;
	class OverlayColors;

	%feature("director") InstanceActionListener;
	class InstanceActionListener {
	public:
		virtual ~InstanceActionListener() {};
		virtual void onInstanceActionFinished(Instance* instance, Action* action) = 0;
		virtual void onInstanceActionFrame(Instance* instance, Action* action, int32_t frame) = 0;
	};

	enum InstanceChangeType {
		ICHANGE_NO_CHANGES = 0x0000,
		ICHANGE_LOC = 0x0001,
		ICHANGE_ROTATION = 0x0002,
		ICHANGE_SPEED = 0x0004,
		ICHANGE_ACTION = 0x0008,
		ICHANGE_TIME_MULTIPLIER = 0x0010,
		ICHANGE_SAYTEXT = 0x0020,
		ICHANGE_BLOCK = 0x0040,
		ICHANGE_CELL = 0x0080,
		ICHANGE_TRANSPARENCY = 0x0100,
		ICHANGE_VISIBLE = 0x0200,
		ICHANGE_STACKPOS = 0x0400
	};
	typedef uint32_t InstanceChangeInfo;
	
	%feature("director") InstanceChangeListener;
	class InstanceChangeListener {
	public:
		virtual ~InstanceChangeListener() {};
		virtual void onInstanceChanged(Instance* instance, InstanceChangeInfo info) = 0;
	};

	enum VisitorShapeType {
		ITYPE_NO_SHAPE = 0,
		ITYPE_QUAD_SHAPE,
		ITYPE_CIRCLE_SHAPE
	};
	typedef uint8_t VisitorShapeInfo;

	class Instance : public FifeClass {
	public:
		Instance(Object* object, const Location& location, const std::string& identifier="");
		virtual ~Instance();
		const std::string& getId();
		void setId(const std::string& identifier="");
		Object* getObject();
		void setLocation(const Location& loc);
		Location getLocation() const;
		Location& getLocationRef();
		Location getTargetLocation() const;
		void setRotation(int32_t);
		int32_t getRotation() const;
		int32_t getOldRotation() const;
		void setBlocking(bool blocking);
		bool isBlocking() const;
		void setOverrideBlocking(bool overblock);
		bool isOverrideBlocking() const;
		void addActionListener(InstanceActionListener* listener);
		void removeActionListener(InstanceActionListener* listener);
		void addChangeListener(InstanceChangeListener* listener);
		void removeChangeListener(InstanceChangeListener* listener);
		Action* getCurrentAction() const;
		double getMovementSpeed() const;
		void setFacingLocation(const Location& loc);
		Location getFacingLocation();
		uint32_t getActionRuntime();
		void setActionRuntime(uint32_t time_offset);
		void move(const std::string& actionName, const Location& target, const double speed, const std::string& costId = "");
		void act(const std::string& actionName, const Location& direction, bool repeating=false);
		void act(const std::string& actionName, int32_t rotation, bool repeating=false);
		void act(const std::string& actionName, bool repeating=false);
		void follow(const std::string& actionName, Instance* leader, const double speed);
		void follow(const std::string& actionName, Route* route, const double speed);
		void cancelMovement(uint32_t length = 1);
		void say(const std::string& text, uint32_t duration=0);
		void setTimeMultiplier(float multip);
		float getTimeMultiplier();
		uint32_t getRuntime();
		void refresh();
		InstanceChangeInfo getChangeInfo();
		
		void setVisual(IVisual* visual);
		template<typename T> T* getVisual() const;

		Route* getRoute();
		void setVisitor(bool visit);
		bool isVisitor();
		void setVisitorShape(VisitorShapeInfo info);
		VisitorShapeInfo getVisitorShape();
		void setVisitorRadius(uint16_t radius);
		uint16_t getVisitorRadius();
		void setCellStackPosition(uint8_t stack);
		uint8_t getCellStackPosition();
		bool isSpecialCost();
		void setCost(const std::string& id, double cost);
		void resetCost();
		double getCost();
		std::string getCostId();

		bool isMultiCell();

		void addStaticColorOverlay(uint32_t angle, const OverlayColors& colors);
		OverlayColors* getStaticColorOverlay(int32_t angle);
		void removeStaticColorOverlay(int32_t angle);
		bool isStaticColorOverlay();
	};
}

namespace std {
	%template(InstanceVector) vector<FIFE::Instance*>;
	%template(InstanceList) list<FIFE::Instance*>;	
}

