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

%module fifechan
%{
#include <fifechan.hpp>
#include <fifechan/actionevent.hpp>
#include <fifechan/event.hpp>
#include <fifechan/keyevent.hpp>
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


