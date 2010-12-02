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
#include "model/structures/instance.h"
%}

%include "model/metamodel/modelcoords.i"
%include "model/metamodel/abstractvisual.i"
%include "util/structures/utilstructures.i"
%include "util/base/utilbase.i"
%include "location.i"

namespace FIFE {

	class Layer;
	class Action;
	class Object;
	class Instance;

	%feature("director") InstanceActionListener;
	class InstanceActionListener {
	public:
		virtual ~InstanceActionListener() {};
		virtual void onInstanceActionFinished(Instance* instance, Action* action) = 0;
	};

	enum InstanceChangeType {
		ICHANGE_NO_CHANGES = 0x0000,
		ICHANGE_LOC = 0x0001,
		ICHANGE_FACING_LOC = 0x0002,
		ICHANGE_SPEED = 0x0004,
		ICHANGE_ACTION = 0x0008,
		ICHANGE_TIME_MULTIPLIER = 0x0010,
		ICHANGE_SAYTEXT = 0x0020,
		ICHANGE_ROTATION = 0x0040,
	};
	typedef unsigned int InstanceChangeInfo;
	
	%feature("director") InstanceChangeListener;
	class InstanceChangeListener {
	public:
		virtual ~InstanceChangeListener() {};
		virtual void onInstanceChanged(Instance* instance, InstanceChangeInfo info) = 0;
	};

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
		void setRotation(int);
		int getRotation() const;
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
		Location& getFacingLocationRef();
		unsigned int getActionRuntime();
		void setActionRuntime(unsigned int time_offset);
		void move(const std::string& action_name, const Location& target, const double speed);
		void act(const std::string& action_name, const Location& direction, bool repeating=false);
		void follow(const std::string& action_name, Instance* leader, const double speed);
		void say(const std::string& text, unsigned int duration=0);
		void setTimeMultiplier(float multip);
		float getTimeMultiplier();
		unsigned int getRuntime();
		void refresh();
		InstanceChangeInfo getChangeInfo();
		
		void setVisual(AbstractVisual* visual);
		template<typename T> T* getVisual() const;
	};
}

namespace std {
	%template(InstanceVector) vector<FIFE::Instance*>;
	%template(InstanceList) list<FIFE::Instance*>;	
}

