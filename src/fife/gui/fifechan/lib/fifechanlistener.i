// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fifechan
%{
#include <fifechan.hpp>
#include <fifechan/events/actionevent.hpp>
#include <fifechan/events/event.hpp>
#include <fifechan/events/keyevent.hpp>
%}

namespace fcn {

	%feature("director") MouseListener;
	class MouseListener {
	public:
		virtual ~MouseListener();

		virtual void mouseEntered(fcn::MouseEvent& mouseEvent);
		virtual void mouseExited(fcn::MouseEvent& mouseEvent);
		virtual void mousePressed(fcn::MouseEvent& mouseEvent);
		virtual void mouseReleased(fcn::MouseEvent& mouseEvent);
		virtual void mouseClicked(fcn::MouseEvent& mouseEvent);
		virtual void mouseWheelMovedUp(fcn::MouseEvent& mouseEvent);
		virtual void mouseWheelMovedDown(fcn::MouseEvent& mouseEvent);
		virtual void mouseWheelMovedRight(fcn::MouseEvent& mouseEvent);
		virtual void mouseWheelMovedLeft(fcn::MouseEvent& mouseEvent);
		virtual void mouseMoved(fcn::MouseEvent& mouseEvent);
		virtual void mouseDragged(fcn::MouseEvent& mouseEvent);
	protected:
		MouseListener() { }
	};

	%feature("director") KeyListener;
	class KeyListener {
	public:
		virtual ~KeyListener();

		virtual void keyPressed(fcn::KeyEvent& keyEvent);
		virtual void keyReleased(fcn::KeyEvent& keyEvent);

	protected:
		KeyListener() { }
	};

	%feature("director") ActionListener;
	class ActionListener {
	public:
		virtual ~ActionListener() { }
		virtual void action(const fcn::ActionEvent& actionEvent) = 0;
	};
	
	%feature("director") WidgetListener;
	class WidgetListener {
	public:
		virtual ~WidgetListener() { }
		
		virtual void widgetResized(const fcn::Event& event);
		virtual void widgetMoved(const fcn::Event& event);
		virtual void widgetHidden(const fcn::Event& event);
		virtual void widgetShown(const fcn::Event& event);
		virtual void ancestorMoved(const fcn::Event& event);
		virtual void ancestorHidden(const fcn::Event& event);
		virtual void ancestorShown(const fcn::Event& event);
		
	protected:
		WidgetListener() { }
	};
}

