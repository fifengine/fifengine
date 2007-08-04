/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

%module(directors="1") eventchannel
%{
#include "eventchannel/base/ec_ievent.h"
#include "eventchannel/base/ec_iinputevent.h"
#include "eventchannel/command/ec_command.h"
#include "eventchannel/command/ec_commandids.h"
#include "eventchannel/command/ec_icommandlistener.h"
#include "eventchannel/key/ec_key.h"
#include "eventchannel/key/ec_ikeyevent.h"
#include "eventchannel/key/ec_keyevent.h"
#include "eventchannel/key/ec_ikeylistener.h"
#include "eventchannel/source/ec_eventsourcetypes.h"
#include "eventchannel/source/ec_ieventsource.h"
#include "eventchannel/mouse/ec_mouseevent.h"
#include "eventchannel/mouse/ec_imouselistener.h"
#include "eventchannel/widget/ec_widgetevent.h"
#include "eventchannel/widget/ec_iwidgetlistener.h"
#include "eventchannel/manager/eventmanager.h"
%}

%include "eventchannel/key/ec_ikey.h"
%include "eventchannel/source/ec_eventsourcetypes.h"
%include "eventchannel/command/ec_commandids.h"

namespace std {
   %template(IntVector) vector<int>;
}

namespace FIFE {
	%feature("director") IEventSource;
	class IEventSource {
	public:
		virtual EventSourceType getEventSourceType() = 0;
		virtual ~IEventSource();
	};

	class IEvent {
	public:
		virtual void consume() = 0;
		virtual bool isConsumed() const = 0;
		virtual IEventSource* getSource() = 0;
		virtual int getTimeStamp() const = 0;
		virtual std::string getDebugString() const = 0;
		virtual const std::string& getName() const = 0;
		virtual ~IEvent() {}
	};

	class IInputEvent: public IEvent {
	public:
		virtual bool isAltPressed() const = 0;
		virtual bool isControlPressed() const = 0;
		virtual bool isMetaPressed() const = 0;
		virtual bool isShiftPressed() const = 0;
		virtual ~IInputEvent() {}
	};

	class ICommand: public IEvent {
	public:
		virtual CommandType getCommandType() = 0;
		virtual int getCode() = 0;
		virtual ~ICommand();
	};
	%feature("notabstract") Command;
	class Command: public ICommand {
	public:
		Command();
		virtual ~Command();

		CommandType getCommandType();
		void setCommandType(CommandType type);

		int getCode();
		void setCode(int code);

		virtual void consume();
		virtual bool isConsumed() const;
		virtual IEventSource* getSource();
		virtual void setSource(IEventSource* source);
		virtual int getTimeStamp() const;
		virtual void setTimeStamp(int timestamp);
	};

	%feature("director") ICommandListener;
	class ICommandListener {
	public:
		virtual void onCommand(ICommand& command) = 0;
		virtual ~ICommandListener() {}
	};

	class IKeyEvent: public IInputEvent  {
	public:
		enum KeyEventType {
			UNKNOWN = -1,
			PRESSED = 0,
			RELEASED
		};
		virtual KeyEventType getType() const = 0;
		virtual bool isNumericPad() const = 0;
		virtual const IKey& getKey() const = 0;
		virtual ~IKeyEvent();
	};

	%feature("director") IKeyListener;
	class IKeyListener {
	public:
		virtual void keyPressed(IKeyEvent& evt) = 0;
		virtual void keyReleased(IKeyEvent& evt) = 0;
		virtual ~IKeyListener();
	};

	class IMouseEvent: public IInputEvent {
	public:
		enum MouseEventType
		{
			UNKNOWN_EVENT = -1,
			MOVED = 0,
			PRESSED,
			RELEASED,
			WHEEL_MOVED_DOWN,
			WHEEL_MOVED_UP,
			CLICKED,
			ENTERED,
			EXITED,
			DRAGGED
		};

		enum MouseButtonType
		{
			EMPTY = 0,
			LEFT = 1,
			RIGHT = 2,
			MIDDLE = 4,
			UNKNOWN_BUTTON = 8
		};
		virtual int getX() const = 0;
        virtual int getY() const = 0;
		virtual MouseEventType getType() const = 0;
		virtual MouseButtonType getButton() const = 0;
		virtual ~IMouseEvent();
	};

	%feature("director") IMouseListener;
	class IMouseListener {
	public:
		virtual void mouseEntered(IMouseEvent& evt) = 0;
		virtual void mouseExited(IMouseEvent& evt) = 0;
		virtual void mousePressed(IMouseEvent& evt) = 0;
		virtual void mouseReleased(IMouseEvent& evt) = 0;
		virtual void mouseClicked(IMouseEvent& evt) = 0;
		virtual void mouseWheelMovedUp(IMouseEvent& evt) = 0;
		virtual void mouseWheelMovedDown(IMouseEvent& evt) = 0;
		virtual void mouseMoved(IMouseEvent& evt) = 0;
		virtual void mouseDragged(IMouseEvent& evt) = 0;
		virtual ~IMouseListener();
	};

	class IWidgetEvent: public IEvent {
	public:
		virtual const std::string& getId() const = 0;
		virtual ~IWidgetEvent();
	};

	%feature("director") IWidgetListener;
	class IWidgetListener {
	public:
		virtual void onWidgetAction(IWidgetEvent& evt) = 0;
		virtual ~IWidgetListener();
	};

	class EventManager {
	public:
		EventManager();
		virtual ~EventManager();
		void addCommandListener(ICommandListener* listener);
		void removeCommandListener(ICommandListener* listener);
		void addKeyListener(IKeyListener* listener);
		void removeKeyListener(IKeyListener* listener);
		void addMouseListener(IMouseListener* listener);
		void removeMouseListener(IMouseListener* listener);
		void addWidgetListener(IWidgetListener* listener);
		void removeWidgetListener(IWidgetListener* listener);
		EventSourceType getEventSourceType();
		void dispatchCommand(ICommand& command);
		void setNonConsumableKeys(const std::vector<int>& keys);
		std::vector<int> getNonConsumableKeys();
	};
};