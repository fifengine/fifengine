/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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
#include <fifechan.hpp>
#include "gui/fifechan/widgets/twobutton.h"
#include "gui/fifechan/widgets/togglebutton.h"
#include "gui/fifechan/widgets/clicklabel.h"
#include "gui/fifechan/widgets/icon2.h"
#include "gui/fifechan/widgets/percentagebar.h"
#include "gui/fifechan/widgets/resizablewindow.h"
#include "gui/fifechan/widgets/dockarea.h"
#include "gui/fifechan/widgets/panel.h"
%}

namespace fcn {
	class ActionListener;
	class Color;
	class Font;
	class Image;
	class KeyListener;
	class MouseListener;
	class WidgetListener;
	class Widget;

		
	%feature("notabstract") TwoButton;
	class TwoButton: public Widget {
	public:
		TwoButton(Image *up_image = 0, Image *down_image = 0, Image *hover_image = 0, const char * caption = "");
		~TwoButton();
		virtual void setCaption(const std::string& caption);
		virtual const std::string& getCaption() const;
		virtual void setAlignment(Graphics::Alignment alignment);
		virtual Graphics::Alignment getAlignment() const;
		void setUpImage(Image* image);
		void setDownImage(Image* image);
		void setHoverImage(Image* image);
		void setDownOffset(int32_t x, int32_t y);
		int32_t getDownXOffset();
		int32_t getDownYOffset();
	};
	
	%feature("notabstract") ToggleButton;
	class ToggleButton: public Widget {
	public:
		ToggleButton(Image *up_image = 0, Image *down_image = 0, Image *hover_image = 0, const char * caption = "", const char * group = "");
		~ToggleButton();
		virtual void setCaption(const std::string& caption);
		virtual const std::string& getCaption() const;
		virtual void setAlignment(Graphics::Alignment alignment);
		virtual Graphics::Alignment getAlignment() const;
		void setUpImage(Image* image);
		void setDownImage(Image* image);
		void setHoverImage(Image* image);
		void setDownOffset(int32_t x, int32_t y);
		int32_t getDownXOffset() const;
		int32_t getDownYOffset() const;
		bool isToggled() const;
		void setToggled(bool toggled);
		void setGroup(const std::string &group);
		const std::string &getGroup() const;
	};
	
	%feature("notabstract") ClickLabel;
	%rename(Label) ClickLabel;
	class ClickLabel: public Widget {
	public:
		ClickLabel();
		ClickLabel(const std::string& caption);
		virtual ~ClickLabel();
		virtual void setCaption(const std::string& caption);
		virtual const std::string& getCaption() const;
		bool isTextWrapping() const;
		void setTextWrapping(bool);
		virtual void setWidth(int32_t width);
		virtual void adjustSize();
	};

	%feature("notabstract") Icon2;
	%rename(Icon) Icon2;
	class Icon2: public Widget {
	public:
		Icon2(Image* image);
		virtual ~Icon2();
		void setImage(Image* image);
		bool isScaling() const;
		void setScaling(bool scale);
		bool isTiling() const;
		void setTiling(bool tile);
	};
	
	%feature("notabstract") PercentageBar;
	class PercentageBar: public Widget {
	public:
		PercentageBar();
		virtual ~PercentageBar();
		virtual void setForegroundImage(Image* image);
        virtual void setOrientation(PercentageBar::Orientation orientation);
        virtual PercentageBar::Orientation getOrientation() const;
        virtual int32_t getValue() const;
        virtual void setValue(int32_t value);
		
		enum Orientation
		{
			HORIZONTAL = 0,
			VERTICAL
		};
	};

	%feature("notabstract") ResizableWindow;
	class ResizableWindow : public Window {
	public:
		enum CursorDirections {
			CURSOR_DIRECTION_L = 0,
			CURSOR_DIRECTION_R = 1,
			CURSOR_DIRECTION_T = 2,
			CURSOR_DIRECTION_B = 5,
			CURSOR_DIRECTION_LT = 3,
			CURSOR_DIRECTION_RT = 4,
			CURSOR_DIRECTION_LB = 6,
			CURSOR_DIRECTION_RB = 7
		};

		ResizableWindow();
		ResizableWindow(bool resizable);
		ResizableWindow(const std::string& caption, bool resizable=true);
		virtual ~ResizableWindow();
		void setResizableBorderDistance(int32_t border);
		int32_t getResizableBorderDistance() const;
		void setResizable(bool resizable);
		bool isResizable() const;
		void setTopResizable(bool resizable);
		bool isTopResizable() const;
		void setRightResizable(bool resizable);
		bool isRightResizable() const;
		void setBottomResizable(bool resizable);
		bool isBottomResizable() const;
		void setLeftResizable(bool resizable);
		bool isLeftResizable() const;
		void setShove(bool shove);
		bool getShove() const;
		void set(CursorDirections direction, uint32_t cursor_id=0);
		void set(CursorDirections direction, FIFE::ImagePtr image);
		void set(CursorDirections direction, FIFE::AnimationPtr anim);
		FIFE::MouseCursorType getType(CursorDirections direction) const;
		uint32_t getId(CursorDirections direction) const;
		FIFE::ImagePtr getImage(CursorDirections direction);
		FIFE::AnimationPtr getAnimation(CursorDirections direction);
	};

	%feature("notabstract") DockArea;
	class DockArea : public ResizableWindow {
	public:
		DockArea();
		DockArea(bool active);
		virtual ~DockArea();

		void setActiveDockArea(bool active);
		bool isActiveDockArea() const;
		void setTopSide(bool side);
		bool isTopSide() const;
		void setRightSide(bool side);
		bool isRightSide() const;
		void setBottomSide(bool side);
		bool isBottomSide() const;
		void setLeftSide(bool side);
		bool isLeftSide() const;
		void dockWidget(Widget* widget);
		void undockWidget(Widget* widget);
		void setHighlighted(bool highlighted);
		bool isHighlighted() const;
		void setHighlightColor(const Color& color);
		const Color& getHighlightColor() const;
	};

	%feature("notabstract") Panel;
	class Panel : public ResizableWindow {
	public:
		Panel();
		Panel(bool dockable);
		virtual ~Panel();
        
		void setDockable(bool dockable);
		bool isDockable() const;
		void setDocked(bool docked);
		bool isDocked() const;
	};
}



