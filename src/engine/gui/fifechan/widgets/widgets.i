// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include <fifechan.hpp>
#include "gui/fifechan/widgets/animationicon.h"
#include "gui/fifechan/widgets/clicklabel.h"
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

	
	%feature("notabstract") AnimationIcon;
	class AnimationIcon: public Icon {
	public:
		AnimationIcon();
		AnimationIcon(FIFE::AnimationPtr animation);
		virtual ~AnimationIcon();
		void setAnimation(FIFE::AnimationPtr animation);
		FIFE::AnimationPtr getAnimation() const;
		void setRepeating(bool repeat);
		bool isRepeating() const;
		void play();
		bool isPlaying() const;
		void pause();
		void stop();
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
		void setAlignment(Graphics::Alignment alignment);
		Graphics::Alignment getAlignment() const;
		void setOpaque(bool opaque);
		bool isOpaque() const;
		bool isTextWrapping() const;
		void setTextWrapping(bool);
		virtual void adjustSize();
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