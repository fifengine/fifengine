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
#include "eventchannel/base/ec_event.h"
#include "eventchannel/base/ec_inputevent.h"
#include "eventchannel/command/ec_command.h"
#include "eventchannel/command/ec_commandids.h"
#include "eventchannel/command/ec_icommandlistener.h"
#include "eventchannel/drop/ec_dropevent.h"
#include "eventchannel/drop/ec_idroplistener.h"
#include "eventchannel/key/ec_key.h"
#include "eventchannel/key/ec_keyevent.h"
#include "eventchannel/key/ec_ikeylistener.h"
#include "eventchannel/key/ec_ikeyfilter.h"
#include "eventchannel/text/ec_text.h"
#include "eventchannel/text/ec_textevent.h"
#include "eventchannel/text/ec_itextlistener.h"
#include "eventchannel/source/ec_eventsourcetypes.h"
#include "eventchannel/source/ec_ieventsource.h"
#include "eventchannel/mouse/ec_mouseevent.h"
#include "eventchannel/mouse/ec_imousefilter.h"
#include "eventchannel/mouse/ec_imouselistener.h"
#include "eventchannel/sdl/ec_isdleventlistener.h"
#include "eventchannel/eventmanager.h"
%}

%include "eventchannel/key/ec_key.h"
%include "eventchannel/text/ec_text.h"
%include "eventchannel/source/ec_eventsourcetypes.h"
%include "eventchannel/command/ec_commandids.h"

namespace FIFE {
	%feature("director") IEventSource;
	class IEventSource {
	public:
		virtual EventSourceType getEventSourceType() = 0;
		virtual ~IEventSource();
	};

	class Event {
	public:
		virtual void consume();
		virtual bool isConsumed() const;
		virtual IEventSource* getSource();
		virtual int32_t getTimeStamp() const;
		virtual std::string getDebugString() const;
		virtual const std::string& getName() const;
		virtual ~Event() {}
	private:
		Event();
	};

	class InputEvent: public Event {
	public:
		virtual bool isConsumedByWidgets() const;
		virtual bool isAltPressed() const;
		virtual bool isControlPressed() const;
		virtual bool isMetaPressed() const;
		virtual bool isShiftPressed() const;
		virtual ~InputEvent() {}
	private:
		InputEvent();
	};

	class Command: public Event {
	public:
		Command();
		virtual ~Command();

		CommandType getCommandType();
		void setCommandType(CommandType type);

		int32_t getCode() const;
		void setCode(int32_t code);

		virtual void setSource(IEventSource* source);
		virtual void setTimeStamp(int32_t timestamp);
	};

	%feature("director") ICommandListener;
	class ICommandListener {
	public:
		virtual void onCommand(Command& command) = 0;
		virtual ~ICommandListener() {}
	};

	class KeyEvent: public InputEvent  {
	public:
		enum KeyEventType {
			UNKNOWN = -1,
			PRESSED = 0,
			RELEASED
		};
		virtual KeyEventType getType() const;
		virtual bool isNumericPad() const;
		virtual const Key& getKey() const;
		virtual ~KeyEvent();
	private:
		KeyEvent();
	};

	%feature("director") IKeyListener;
	class IKeyListener {
	public:
		virtual void keyPressed(KeyEvent& evt) = 0;
		virtual void keyReleased(KeyEvent& evt) = 0;
		virtual ~IKeyListener();
	};

	class TextEvent: public InputEvent  {
	public:
		enum TextEventType {
			UNKNOWN = 0,
			INPUT = 1,
			EDIT = 2
		};
		virtual TextEventType getType() const;
		virtual const Text& getText() const;
		virtual ~TextEvent();
	private:
		TextEvent();
	};

	%feature("director") ITextListener;
	class ITextListener {
	public:
		virtual void textInput(TextEvent& evt) = 0;
		virtual void textEdit(TextEvent& evt) = 0;
		virtual ~ITextListener();
	};
	
	%feature("director") ISdlEventListener;
	class ISdlEventListener {
	public:
		virtual bool onSdlEvent(SDL_Event& evt) = 0;
		virtual ~ISdlEventListener();
	};

	class MouseEvent: public InputEvent {
	public:
		enum MouseEventType {
			UNKNOWN_EVENT = -1,
			MOVED = 0,
			PRESSED,
			RELEASED,
			WHEEL_MOVED_DOWN,
			WHEEL_MOVED_UP,
			WHEEL_MOVED_RIGHT,
			WHEEL_MOVED_LEFT,
			CLICKED,
			ENTERED,
			EXITED,
			DRAGGED
		};

		enum MouseButtonType {
			EMPTY = 0,
			LEFT = 1,
			RIGHT = 2,
			MIDDLE = 4,
			X1 = 8,
			X2 = 16,
			UNKNOWN_BUTTON = 32
		};
		virtual int32_t getX() const;
		virtual int32_t getY() const;
		virtual MouseEventType getType() const;
		virtual MouseButtonType getButton() const;
		virtual ~MouseEvent();
	private:
		MouseEvent();
	};

	%feature("director") IMouseListener;
	class IMouseListener {
	public:
		virtual void mouseEntered(MouseEvent& evt) = 0;
		virtual void mouseExited(MouseEvent& evt) = 0;
		virtual void mousePressed(MouseEvent& evt) = 0;
		virtual void mouseReleased(MouseEvent& evt) = 0;
		virtual void mouseClicked(MouseEvent& evt) = 0;
		virtual void mouseWheelMovedUp(MouseEvent& evt) = 0;
		virtual void mouseWheelMovedDown(MouseEvent& evt) = 0;
		virtual void mouseWheelMovedRight(MouseEvent& evt) = 0;
		virtual void mouseWheelMovedLeft(MouseEvent& evt) = 0;
		virtual void mouseMoved(MouseEvent& evt) = 0;
		virtual void mouseDragged(MouseEvent& evt) = 0;
		virtual ~IMouseListener();
	};

	%feature("director") IKeyFilter;
	class IKeyFilter {
	public:
		virtual bool isFiltered(const KeyEvent& evt) = 0;
		virtual ~IKeyFilter();
	};

	class DropEvent: public InputEvent  {
	public:
		virtual const std::string& getPath() const;
		virtual ~DropEvent();
	private:
		DropEvent();
	};

	%feature("director") IDropListener;
	class IDropListener {
	public:
		virtual void fileDropped(DropEvent& evt) = 0;
		virtual ~IDropListener();
	};

	%feature("director") IMouseFilter;
	class IMouseFilter {
	public:
		virtual bool isFiltered(const MouseEvent& evt) = 0;
		virtual ~IMouseFilter();
	};

	class EventManager {
	public:
		EventManager();
		virtual ~EventManager();
		void addCommandListener(ICommandListener* listener);
		void addCommandListenerFront(ICommandListener* listener);
		void removeCommandListener(ICommandListener* listener);
		void addKeyListener(IKeyListener* listener);
		void addKeyListenerFront(IKeyListener* listener);
		void removeKeyListener(IKeyListener* listener);
		void addTextListener(ITextListener* listener);
		void addTextListenerFront(ITextListener* listener);
		void removeTextListener(ITextListener* listener);
		void addMouseListener(IMouseListener* listener);
		void addMouseListenerFront(IMouseListener* listener);
		void removeMouseListener(IMouseListener* listener);
		void addDropListener(IDropListener* listener);
		void addDropListenerFront(IDropListener* listener);
		void removeDropListener(IDropListener* listener);

		void addSdlEventListener(ISdlEventListener* listener);
		void addSdlEventListenerFront(ISdlEventListener* listener);
		void removeSdlEventListener(ISdlEventListener* listener);
		
		EventSourceType getEventSourceType();
		void dispatchCommand(Command& command);
		void setKeyFilter(IKeyFilter* keyFilter);
		void setMouseFilter(IMouseFilter* mouseFilter);

		void setMouseSensitivity(float sensitivity);
		float getMouseSensitivity() const;
		void setMouseAccelerationEnabled(bool acceleration);
		bool isMouseAccelerationEnabled() const;

		bool isClipboardText() const;
		std::string getClipboardText() const;
		void setClipboardText(const std::string& text);
	};
};
