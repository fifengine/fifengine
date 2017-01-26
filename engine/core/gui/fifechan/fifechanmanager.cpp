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

// Standard C++ library includes

// 3rd party library includes
#include <fifechan/sdl/sdlinput.hpp>
#include <fifechan/key.hpp>
#include <fifechan/focushandler.hpp>
#include <fifechan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#ifdef HAVE_OPENGL
#include "gui/fifechan/base/opengl/opengl_gui_graphics.h"
#endif
#include "gui/fifechan/base/sdl/sdl_gui_graphics.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "video/renderbackend.h"
#include "gui/fifechan/base/gui_imageloader.h"
#include "gui/fifechan/base/gui_font.h"
#include "gui/fifechan/console/console.h"
#include "video/fonts/fontbase.h"
#include "video/fonts/truetypefont.h"
#include "video/fonts/subimagefont.h"
#include "eventchannel/key/ec_key.h"
#include "eventchannel/key/ec_keyevent.h"
#include "eventchannel/mouse/ec_mouseevent.h"
#include "vfs/fife_boost_filesystem.h"

#include "fifechanmanager.h"

namespace FIFE {
	static Logger _log(LM_GUI);

	FifechanManager::FifechanManager() :
		m_fcn_gui(new fcn::Gui()),
		m_focushandler(0),
		m_fcn_topcontainer(new fcn::Container()),
		m_imgloader(new GuiImageLoader()) ,
		m_input(new fcn::SDLInput()),
		m_console(0),
		m_cursor(0),
		m_defaultfont(0),
		m_fonts(),
		m_logic_executed(false),
		m_enabled_console(true) {

		m_fcn_gui->setInput(m_input);
		fcn::Image::setImageLoader(m_imgloader);

		m_fcn_gui->setTop(m_fcn_topcontainer);
		m_focushandler = m_fcn_topcontainer->_getFocusHandler();

		m_fcn_topcontainer->setOpaque(false);
		m_fcn_topcontainer->setFocusable(false);
		m_had_mouse = false;
		m_had_widget = false;
		m_lastMotionX = 0;
		m_lastMotionY = 0;
	}

	FifechanManager::~FifechanManager() {
		delete m_console;
		delete m_fcn_topcontainer;
		delete m_imgloader;
		delete m_input;
		delete m_fcn_gui;
		delete m_gui_graphics;
		std::vector<GuiFont*>::iterator i = m_fonts.begin();
		while (i != m_fonts.end()) {
			delete *i;
			++i;
		}
	}

	bool FifechanManager::onSdlEvent(SDL_Event& evt) {
		if (!m_input) {
			FL_WARN(_log, "FifechanManager, FifechanGUI->getInput == 0 ... discarding events!");
			return false;
		}

		bool overWidget = m_fcn_topcontainer->getWidgetAt(m_lastMotionX, m_lastMotionY) != 0;

		switch(evt.type) {
			case SDL_MOUSEWHEEL:
			case SDL_MOUSEBUTTONDOWN:
				m_had_widget = overWidget;
			case SDL_MOUSEBUTTONUP:
				// Always send the button up/down events to fifechan 
				m_input->pushInput(evt);

				// Button was pressed over a widget and still is over a widget
				// so we mark the event as processed.
				if( m_had_widget && overWidget ) {
					return true;
				}

				// Button wasn't pressed over a widget so we want to release focus
				// no matter what.
				if (!m_had_widget) {
					m_focushandler->focusNone();
				}

				// Button up was processed by fifechan but there was no widget under
				// the mouse at the time.  Don't mark it as processed here so the
				// other listeners have a chance to process the event.
				return false;

			case SDL_MOUSEMOTION:
				m_lastMotionX = evt.motion.x;
				m_lastMotionY = evt.motion.y;
				if (m_fcn_topcontainer->getWidgetAt(evt.motion.x,evt.motion.y)) {
					m_had_mouse = true;
					m_input->pushInput(evt);
					return true;
				}
				if( m_had_mouse ) {
					// We only keep the mouse if a widget/window has requested
					// dragging.
					m_had_mouse = m_focushandler->getDraggedWidget() != 0;
					m_input->pushInput(evt);
					return true;
				}
				return false;

			case SDL_KEYDOWN:
			case SDL_KEYUP:
				if(m_focushandler->getFocused()) {
					m_input->pushInput(evt);
					return true;
				}
				return false;

			case SDL_TEXTINPUT:
				// don't consume TEXTINPUT
				m_input->pushInput(evt);
				return false;

			case SDL_WINDOWEVENT:
				// don't consume WINDOWEVENTS
				m_input->pushInput(evt);
				return false;

			default:
				return false;
		}
	}

	void FifechanManager::resizeTopContainer(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		m_fcn_topcontainer->setDimension(fcn::Rectangle(x, y, width, height));
		invalidateFonts();
		if (m_console) {
			m_console->reLayout();
		}
	}

	fcn::Gui* FifechanManager::getFifechanGUI() const {
		return m_fcn_gui;
	}

	void FifechanManager::add(fcn::Widget* widget) {
		if( !m_widgets.count(widget) ) {
			m_fcn_topcontainer->add(widget);
			m_widgets.insert(widget);
		}
	}

	void FifechanManager::remove(fcn::Widget* widget) {
		if( m_widgets.count(widget) ) {
			m_widgets.erase(widget);
			m_fcn_topcontainer->remove(widget);
		}
	}

	void FifechanManager::setConsoleEnabled(bool console) {
		m_enabled_console = console;
	}

	bool FifechanManager::isConsoleEnabled() const {
		return m_enabled_console;
	}

	void FifechanManager::init(const std::string& backend, int32_t screenWidth, int32_t screenHeight) {
		if( backend == "SDL" ) {
			m_gui_graphics = new SdlGuiGraphics();
		}
#ifdef HAVE_OPENGL
		else if (backend == "OpenGL") {
			m_gui_graphics = new OpenGLGuiGraphics();
		}
#endif
        else {
			//should never get here
			assert(0);
		}

		m_fcn_gui->setGraphics(m_gui_graphics);
		if (m_enabled_console) {
			m_console = new Console();
		}

		resizeTopContainer(0, 0, screenWidth, screenHeight);
	}

	GuiFont* FifechanManager::createFont(const std::string& path, uint32_t size, const std::string& glyphs) {
		std::string fontpath = path;
		std::string fontglyphs = glyphs;
		int32_t fontsize = size;

		// Set default settings if necessary
		if(fontpath == "") {
			fontpath = m_fontpath;
		}
		if(fontsize == 0) {
			fontsize = m_fontsize;
		}
		if(fontglyphs == "") {
			fontglyphs = m_fontglyphs;
		}

		IFont* font = NULL;
		GuiFont* guifont = NULL;
		if( bfs::extension(fontpath) == ".ttf" || bfs::extension(fontpath) == ".ttc" ) {
			font = new TrueTypeFont(fontpath, fontsize);
		} else {
			font = new SubImageFont(fontpath, fontglyphs);
		}
		guifont = new GuiFont(font);

		m_fonts.push_back(guifont);
		return guifont;
	}

	void FifechanManager::releaseFont(GuiFont* font) {
		std::vector<GuiFont*>::iterator i = m_fonts.begin();
		while (i != m_fonts.end()) {
			if ((*i) == font) {
				m_fonts.erase(i);
				delete font;
				return;
			}
			++i;
		}
	}

	void FifechanManager::invalidateFonts() {
		std::vector<GuiFont*>::iterator it = m_fonts.begin();
		while (it != m_fonts.end()) {
			(*it)->invalidate();
			++it;
		}
	}

	GuiFont* FifechanManager::setDefaultFont(const std::string& path, uint32_t size, const std::string& glyphs) {
		m_fontpath = path;
		m_fontsize = size;
		m_fontglyphs = glyphs;

		m_defaultfont = createFont();
		fcn::Widget::setGlobalFont(m_defaultfont);
		if (m_console) {
			m_console->reLayout();
		}

		return m_defaultfont;
	}

	void FifechanManager::turn() {
		if (!m_logic_executed)
			m_fcn_gui->logic();
		m_logic_executed = false;
		m_fcn_gui->draw();
	}

	KeyEvent FifechanManager::translateKeyEvent(const fcn::KeyEvent& fcnevt) {
		KeyEvent keyevt;
		if(fcnevt.getType() == fcn::KeyEvent::Pressed)
			keyevt.setType(KeyEvent::PRESSED);
		else if(fcnevt.getType() == fcn::KeyEvent::Released)
			keyevt.setType(KeyEvent::RELEASED);
		else {
			FL_WARN(_log, LMsg("FifechanManager::translateKeyEvent() - ") << "Unknown event type: " << fcnevt.getType());
			keyevt.setType(KeyEvent::UNKNOWN);
		}
		keyevt.setShiftPressed(fcnevt.isShiftPressed());
		keyevt.setControlPressed(fcnevt.isControlPressed());
		keyevt.setAltPressed(fcnevt.isAltPressed());
		keyevt.setMetaPressed(fcnevt.isMetaPressed());
		keyevt.setNumericPad(fcnevt.isNumericPad());

		// Convert from fifechan keyval to FIFE keyval
		int32_t keyval = fcnevt.getKey().getValue();
		keyval = convertFifechanKeyToFifeKey(keyval);

		keyevt.setKey(Key(static_cast<Key::KeyType>(keyval)));

		return keyevt;
	}

	MouseEvent FifechanManager::translateMouseEvent(const fcn::MouseEvent& fcnevt) {
		MouseEvent mouseevt;
		mouseevt.setShiftPressed(fcnevt.isShiftPressed());
		mouseevt.setControlPressed(fcnevt.isControlPressed());
		mouseevt.setAltPressed(fcnevt.isAltPressed());
		mouseevt.setMetaPressed(fcnevt.isMetaPressed());
		mouseevt.setX(fcnevt.getX());
		mouseevt.setY(fcnevt.getY());

		switch(fcnevt.getType()) {
			case fcn::MouseEvent::Pressed:
				mouseevt.setType(MouseEvent::PRESSED);
				break;
			case fcn::MouseEvent::Released:
				mouseevt.setType(MouseEvent::RELEASED);
				break;
			case fcn::MouseEvent::Moved:
				mouseevt.setType(MouseEvent::MOVED);
				break;
			case fcn::MouseEvent::Clicked:
				mouseevt.setType(MouseEvent::CLICKED);
				break;
			case fcn::MouseEvent::Entered:
				mouseevt.setType(MouseEvent::ENTERED);
				break;
			case fcn::MouseEvent::Exited:
				mouseevt.setType(MouseEvent::EXITED);
				break;
			case fcn::MouseEvent::Dragged:
				mouseevt.setType(MouseEvent::DRAGGED);
				break;
			case fcn::MouseEvent::WheelMovedDown:
				mouseevt.setType(MouseEvent::WHEEL_MOVED_DOWN);
				break;
			case fcn::MouseEvent::WheelMovedUp:
				mouseevt.setType(MouseEvent::WHEEL_MOVED_UP);
				break;
			default:
				mouseevt.setType(MouseEvent::UNKNOWN_EVENT);
		}

		switch(fcnevt.getButton()) {
			case fcn::MouseInput::Left:
				mouseevt.setButton(MouseEvent::LEFT);
				break;
			case fcn::MouseInput::Right:
				mouseevt.setButton(MouseEvent::RIGHT);
				break;
			case fcn::MouseInput::Middle:
				mouseevt.setButton(MouseEvent::MIDDLE);
				break;
			case fcn::MouseInput::X1:
				mouseevt.setButton(MouseEvent::X1);
				break;
			case fcn::MouseInput::X2:
				mouseevt.setButton(MouseEvent::X2);
				break;
			default:
				mouseevt.setButton(MouseEvent::UNKNOWN_BUTTON);
				break;
		}
		return mouseevt;
	}

	void FifechanManager::setTabbingEnabled(bool tabbing) {
		m_fcn_gui->setTabbingEnabled(tabbing);
	}
	
	bool FifechanManager::isTabbingEnabled() const {
		return m_fcn_gui->isTabbingEnabled();
	}

	int32_t FifechanManager::convertFifechanKeyToFifeKey(int32_t value) {

		switch (value) {
			case fcn::Key::Tab:
				value = Key::TAB;
				break;
			case fcn::Key::LeftAlt:
				value = Key::LEFT_ALT;
				break;
			case fcn::Key::RightAlt:
				value = Key::RIGHT_ALT;
				break;
			case fcn::Key::LeftShift:
				value = Key::LEFT_SHIFT;
				break;
			case fcn::Key::RightShift:
				value = Key::RIGHT_SHIFT;
				break;
			case fcn::Key::LeftControl:
				value = Key::LEFT_CONTROL;
				break;
			case fcn::Key::RightControl:
				value = Key::RIGHT_CONTROL;
				break;
			case fcn::Key::Backspace:
				value = Key::BACKSPACE;
				break;
			case fcn::Key::Pause:
				value = Key::PAUSE;
				break;
			case fcn::Key::Space:
				value = Key::SPACE;
				break;
			case fcn::Key::Escape:
				value = Key::ESCAPE;
				break;
			case fcn::Key::Delete:
				value = Key::DELETE;
				break;
			case fcn::Key::Insert:
				value = Key::INSERT;
				break;
			case fcn::Key::Home:
				value = Key::HOME;
				break;
			case fcn::Key::End:
				value = Key::END;
				break;
			case fcn::Key::PageUp:
				value = Key::PAGE_UP;
				break;
			case fcn::Key::PrintScreen:
				value = Key::PRINT_SCREEN;
				break;
			case fcn::Key::PageDown:
				value = Key::PAGE_DOWN;
				break;
			case fcn::Key::F1:
				value = Key::F1;
				break;
			case fcn::Key::F2:
				value = Key::F2;
				break;
			case fcn::Key::F3:
				value = Key::F3;
				break;
			case fcn::Key::F4:
				value = Key::F4;
				break;
			case fcn::Key::F5:
				value = Key::F5;
				break;
			case fcn::Key::F6:
				value = Key::F6;
				break;
			case fcn::Key::F7:
				value = Key::F7;
				break;
			case fcn::Key::F8:
				value = Key::F8;
				break;
			case fcn::Key::F9:
				value = Key::F9;
				break;
			case fcn::Key::F10:
				value = Key::F10;
				break;
			case fcn::Key::F11:
				value = Key::F11;
				break;
			case fcn::Key::F12:
				value = Key::F12;
				break;
			case fcn::Key::F13:
				value = Key::F13;
				break;
			case fcn::Key::F14:
				value = Key::F14;
				break;
			case fcn::Key::F15:
				value = Key::F15;
				break;
			case fcn::Key::NumLock:
				value = Key::NUM_LOCK;
				break;
			case fcn::Key::CapsLock:
				value = Key::CAPS_LOCK;
				break;
			case fcn::Key::ScrollLock:
				value = Key::SCROLL_LOCK;
				break;
			case fcn::Key::LeftSuper:
				value = Key::LEFT_SUPER;
				break;
			case fcn::Key::RightSuper:
				value = Key::RIGHT_SUPER;
				break;
			case fcn::Key::AltGr:
				value = Key::ALT_GR;
				break;
			case fcn::Key::Up:
				value = Key::UP;
				break;
			case fcn::Key::Down:
				value = Key::DOWN;
				break;
			case fcn::Key::Left:
				value = Key::LEFT;
				break;
			case fcn::Key::Right:
				value = Key::RIGHT;
				break;
			case fcn::Key::Enter:
				value = Key::ENTER;
				break;

			default:
				// Convert from unicode to lowercase letters
				if (value >= 1 && value <= 26) {
					// Control characters
					value = value - 1 + 'a';
				} else if (value >= 'A' && value <= 'Z') {
					value = value - 'A' + 'a';
				}

				// FIXME: Accented characters (á) will not get converted properly.
				break;
		}

		return value;
	}
}
