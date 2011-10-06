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
#include "view/visual.h"
%}

namespace FIFE {
	class Visual2DGfx {
	public:
		virtual ~Visual2DGfx();
		void setTransparency(uint8_t transparency);
		uint8_t getTransparency();
		void setVisible(bool visible);
		uint32_t isVisible();
	private:
		Visual2DGfx();
	};

	%apply std::vector<int32_t> &OUTPUT { std::vector<int32_t>& angles };
	class ObjectVisual: public Visual2DGfx {
	public:
		static ObjectVisual* create(Object* object);
		virtual ~ObjectVisual();
		void addStaticImage(uint32_t angle, int32_t image_index);
		int32_t getStaticImageIndexByAngle(int32_t angle);
		int32_t getClosestMatchingAngle(int32_t angle);
		void getStaticImageAngles(std::vector<int32_t>& angles);
	private:
		ObjectVisual();
	};
	%template(get2dGfxVisual) Object::getVisual<ObjectVisual>;
	%clear std::vector<int32_t> angles;
	
	class InstanceVisual: public Visual2DGfx {
	public:
		static InstanceVisual* create(Instance* instance);
		virtual ~InstanceVisual();
		void setStackPosition(int32_t stackposition);
		int32_t getStackPosition();
	private:
		InstanceVisual();
	};
	%template(get2dGfxVisual) Instance::getVisual<InstanceVisual>;
	
	%apply std::vector<int32_t> &OUTPUT { std::vector<int32_t>& angles };
	class ActionVisual: public Visual2DGfx {
	public:
		static ActionVisual* create(Action* action);
		virtual ~ActionVisual();
		void addAnimation(uint32_t angle, AnimationPtr animationptr);
		AnimationPtr getAnimationByAngle(int32_t angle);
		void getActionImageAngles(std::vector<int32_t>& angles);
	private:
		ActionVisual();
	};
	%template(get2dGfxVisual) Action::getVisual<ActionVisual>;
}
