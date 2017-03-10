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
#include "view/visual.h"
%}

namespace FIFE {
	class OverlayColors {
	public:
		OverlayColors();
		OverlayColors(ImagePtr image);
		OverlayColors(AnimationPtr animation);
		~OverlayColors();
		void setColorOverlayImage(ImagePtr image);
		ImagePtr getColorOverlayImage();
		void setColorOverlayAnimation(AnimationPtr animation);
		AnimationPtr getColorOverlayAnimation();
		void changeColor(const Color& source, const Color& target);
		const std::map<Color, Color>& getColors();
		void resetColors();
	};

	class Visual2DGfx {
	public:
		virtual ~Visual2DGfx();
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
		void addStaticColorOverlay(uint32_t angle, const OverlayColors& colors);
		OverlayColors* getStaticColorOverlay(int32_t angle);
		void removeStaticColorOverlay(int32_t angle);
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
		void setTransparency(uint8_t transparency);
		uint8_t getTransparency();
		void setVisible(bool visible);
		bool isVisible();
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
		void addAnimationOverlay(uint32_t angle, int32_t order, AnimationPtr animationptr);
		void removeAnimationOverlay(uint32_t angle, int32_t order);
		std::map<int32_t, AnimationPtr> getAnimationOverlay(int32_t angle);
		void addColorOverlay(uint32_t angle, const OverlayColors& colors);
		OverlayColors* getColorOverlay(int32_t angle);
		void removeColorOverlay(int32_t angle);
		void addColorOverlay(uint32_t angle, int32_t order, const OverlayColors& colors);
		OverlayColors* getColorOverlay(int32_t angle, int32_t order);
		void removeColorOverlay(int32_t angle, int32_t order);
		void getActionImageAngles(std::vector<int32_t>& angles);
		void convertToOverlays(bool color);
		bool isAnimationOverlay();
		bool isColorOverlay();
	private:
		ActionVisual();
	};
	%template(get2dGfxVisual) Action::getVisual<ActionVisual>;
}
