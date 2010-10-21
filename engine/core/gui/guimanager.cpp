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

// Standard C++ library includes
#include <iostream>

// 3rd party library includes
#include <boost/filesystem/convenience.hpp>
#include <guichan/sdl/sdlinput.hpp>
#include <guichan/key.hpp>
#include <guichan/focushandler.hpp>
#include <guichan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "video/renderbackend.h"
#include "gui/base/gui_imageloader.h"
#include "gui/base/gui_font.h"
#include "gui/console/console.h"
#include "video/fonts/fontbase.h"
#include "video/fonts/truetypefont.h"
#include "video/fonts/subimagefont.h"
#include "eventchannel/key/ec_key.h"
#include "eventchannel/key/ec_keyevent.h"
#include "eventchannel/mouse/ec_mouseevent.h"

#include "guimanager.h"


namespace FIFE {
	static Logger _log(LM_GUI);

	GUIManager::GUIManager(ImagePool& pool) :
		m_gcn_gui(new gcn::Gui()),
		m_focushandler(0),
		m_gcn_topcontainer(new gcn::Container()),
		m_imgloader(new GuiImageLoader(pool)) ,
		m_input(new gcn::SDLInput()),
		m_console(0),
		m_fonts(),
		m_pool(pool),
		m_logic_executed(false) {

		m_gcn_gui->setInput(m_input);
		gcn::Image::setImageLoader(m_imgloader);

		m_gcn_gui->setTop(m_gcn_topcontainer);
		m_focushandler = m_gcn_topcontainer->_getFocusHandler();

		m_gcn_topcontainer->setOpaque(false);
		m_gcn_topcontainer->setFocusable(false);
		m_had_mouse = false;
	}

	GUIManager::~GUIManager() {
		delete m_console;
		delete m_gcn_topcontainer;
		delete m_imgloader;
		delete m_input;
		delete m_gcn_gui;
		std::vector<GuiFont*>::iterator i = m_fonts.begin();
		while (i != m_fonts.end()) {
			delete *i;
			++i;
		}
	}

	bool GUIManager::onSdlEvent(SDL_Event& evt) {
		if (!m_input) {
			FL_WARN(_log, "GUIManager, GuichanGUI->getInput == 0 ... discarding events!");
			return false;
		}

		switch(evt.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				if( m_gcn_topcontainer->getWidgetAt(evt.button.x,evt.button.y) ) {
					m_input->pushInput(evt);
					return true;
				}
				m_focushandler->focusNone();
				return false;

			case SDL_MOUSEMOTION:
				if( m_gcn_topcontainer->getWidgetAt(evt.button.x,evt.button.y) ) {
					m_had_mouse = true;
					m_input->pushInput(evt);
					return true;
				}
				if( m_had_mouse ) {
					// We only keep the mouse if a widget/window has requested
					// dragging.
					m_had_mouse = bool(m_focushandler->getDraggedWidget());
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

			case SDL_ACTIVEEVENT:
				// Actually Guichan doesn't care (it should!)
				// so at least don't swallow mouse_focus events up.
				return false;

			default:
				return false;
		}
	}

	void GUIManager::resizeTopContainer(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
		m_gcn_topcontainer->setDimension(gcn::Rectangle(x, y, width, height));
	}

	gcn::Gui* GUIManager::getGuichanGUI() const {
		return m_gcn_gui;
	}

	void GUIManager::add(gcn::Widget* widget) {
		if( !m_widgets.count(widget) ) {
			m_gcn_topcontainer->add(widget);
			m_widgets.insert(widget);
		}
	}

	void GUIManager::remove(gcn::Widget* widget) {
		if( m_widgets.count(widget) ) {
			m_widgets.erase(widget);
			m_gcn_topcontainer->remove(widget);
		}
	}

	void GUIManager::init(gcn::Graphics* graphics, int screenWidth, int screenHeight) {
		m_gcn_gui->setGraphics(graphics);
		resizeTopContainer(0, 0, screenWidth, screenHeight);
		m_console = new Console();
	}

	GuiFont* GUIManager::createFont(const std::string& path, unsigned int size, const std::string& glyphs) {
		std::string fontpath = path;
		std::string fontglyphs = glyphs;
		int fontsize = size;

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

		AbstractFont* font = NULL;
		GuiFont* guifont = NULL;
		if( boost::filesystem::extension(fontpath) == ".ttf" ) {
			font = new TrueTypeFont(fontpath, fontsize);
		} else {
			font = new SubImageFont(fontpath, fontglyphs, m_pool);
		}
		guifont = new GuiFont(font);

		m_fonts.push_back(guifont);
		return guifont;
	}

	void GUIManager::releaseFont(GuiFont* font) {
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

	void GUIManager::invalidateFonts() {
		std::vector<GuiFont*>::iterator it = m_fonts.begin();
		while (it != m_fonts.end()) {
			(*it)->invalidate();
			++it;
		}
	}

	GuiFont* GUIManager::setDefaultFont(const std::string& path, unsigned int size, const std::string& glyphs) {
		m_fontpath = path;
		m_fontsize = size;
		m_fontglyphs = glyphs;

		GuiFont* defaultfont = createFont();
		gcn::Widget::setGlobalFont(defaultfont);
		if (m_console) {
			m_console->reLayout();
		}

		return defaultfont;
	}

	void GUIManager::turn() {
		if (!m_logic_executed)
			m_gcn_gui->logic();
		m_logic_executed = false;
		m_gcn_gui->draw();
	}

	KeyEvent GUIManager::translateKeyEvent(const gcn::KeyEvent& gcnevt) {
		KeyEvent keyevt;
		if(gcnevt.getType() == gcn::KeyEvent::PRESSED)
			keyevt.setType(KeyEvent::PRESSED);
		else if(gcnevt.getType() == gcn::KeyEvent::RELEASED)
			keyevt.setType(KeyEvent::RELEASED);
		else
			throw EventException("Invalid event type in fillKeyEvent");
		keyevt.setShiftPressed(gcnevt.isShiftPressed());
		keyevt.setControlPressed(gcnevt.isControlPressed());
		keyevt.setAltPressed(gcnevt.isAltPressed());
		keyevt.setMetaPressed(gcnevt.isMetaPressed());
		keyevt.setNumericPad(gcnevt.isNumericPad());

		// Convert from guichan keyval to FIFE keyval
		int keyval = gcnevt.getKey().getValue();
		keyval = convertGuichanKeyToFifeKey(keyval);

		keyevt.setKey(Key(static_cast<Key::KeyType>(keyval), keyval));

		return keyevt;
	}

	MouseEvent GUIManager::translateMouseEvent(const gcn::MouseEvent& gcnevt) {
		MouseEvent mouseevt;
		mouseevt.setShiftPressed(gcnevt.isShiftPressed());
		mouseevt.setControlPressed(gcnevt.isControlPressed());
		mouseevt.setAltPressed(gcnevt.isAltPressed());
		mouseevt.setMetaPressed(gcnevt.isMetaPressed());
		mouseevt.setX(gcnevt.getX());
		mouseevt.setY(gcnevt.getY());

		switch(gcnevt.getType()) {
			case gcn::MouseEvent::PRESSED:
				mouseevt.setType(MouseEvent::PRESSED);
				break;
			case gcn::MouseEvent::RELEASED:
				mouseevt.setType(MouseEvent::RELEASED);
				break;
			case gcn::MouseEvent::MOVED:
				mouseevt.setType(MouseEvent::MOVED);
				break;
			case gcn::MouseEvent::CLICKED:
				mouseevt.setType(MouseEvent::CLICKED);
				break;
			case gcn::MouseEvent::ENTERED:
				mouseevt.setType(MouseEvent::ENTERED);
				break;
			case gcn::MouseEvent::EXITED:
				mouseevt.setType(MouseEvent::EXITED);
				break;
			case gcn::MouseEvent::DRAGGED:
				mouseevt.setType(MouseEvent::DRAGGED);
				break;
			case gcn::MouseEvent::WHEEL_MOVED_DOWN:
				mouseevt.setType(MouseEvent::WHEEL_MOVED_DOWN);
				break;
			case gcn::MouseEvent::WHEEL_MOVED_UP:
				mouseevt.setType(MouseEvent::WHEEL_MOVED_UP);
				break;
			default:
				mouseevt.setType(MouseEvent::UNKNOWN_EVENT);
		}

		switch(gcnevt.getButton()) {
			case gcn::MouseInput::LEFT:
				mouseevt.setButton(MouseEvent::LEFT);
				break;
			case gcn::MouseInput::RIGHT:
				mouseevt.setButton(MouseEvent::RIGHT);
				break;
			case gcn::MouseInput::MIDDLE:
				mouseevt.setButton(MouseEvent::MIDDLE);
				break;
			default:
				mouseevt.setButton(MouseEvent::UNKNOWN_BUTTON);
				break;
		}
		return mouseevt;
	}


	int GUIManager::convertGuichanKeyToFifeKey(int value) {

		switch (value) {
			case gcn::Key::TAB:
				value = Key::TAB;
				break;
			case gcn::Key::LEFT_ALT:
				value = Key::LEFT_ALT;
				break;
			case gcn::Key::RIGHT_ALT:
				value = Key::RIGHT_ALT;
				break;
			case gcn::Key::LEFT_SHIFT:
				value = Key::LEFT_SHIFT;
				break;
			case gcn::Key::RIGHT_SHIFT:
				value = Key::RIGHT_SHIFT;
				break;
			case gcn::Key::LEFT_CONTROL:
				value = Key::LEFT_CONTROL;
				break;
			case gcn::Key::RIGHT_CONTROL:
				value = Key::RIGHT_CONTROL;
				break;
			case gcn::Key::BACKSPACE:
				value = Key::BACKSPACE;
				break;
			case gcn::Key::PAUSE:
				value = Key::PAUSE;
				break;
			case gcn::Key::SPACE:
				value = Key::SPACE;
				break;
			case gcn::Key::ESCAPE:
				value = Key::ESCAPE;
				break;
			case gcn::Key::DELETE:
				value = Key::DELETE;
				break;
			case gcn::Key::INSERT:
				value = Key::INSERT;
				break;
			case gcn::Key::HOME:
				value = Key::HOME;
				break;
			case gcn::Key::END:
				value = Key::END;
				break;
			case gcn::Key::PAGE_UP:
				value = Key::PAGE_UP;
				break;
			case gcn::Key::PRINT_SCREEN:
				value = Key::PRINT_SCREEN;
				break;
			case gcn::Key::PAGE_DOWN:
				value = Key::PAGE_DOWN;
				break;
			case gcn::Key::F1:
				value = Key::F1;
				break;
			case gcn::Key::F2:
				value = Key::F2;
				break;
			case gcn::Key::F3:
				value = Key::F3;
				break;
			case gcn::Key::F4:
				value = Key::F4;
				break;
			case gcn::Key::F5:
				value = Key::F5;
				break;
			case gcn::Key::F6:
				value = Key::F6;
				break;
			case gcn::Key::F7:
				value = Key::F7;
				break;
			case gcn::Key::F8:
				value = Key::F8;
				break;
			case gcn::Key::F9:
				value = Key::F9;
				break;
			case gcn::Key::F10:
				value = Key::F10;
				break;
			case gcn::Key::F11:
				value = Key::F11;
				break;
			case gcn::Key::F12:
				value = Key::F12;
				break;
			case gcn::Key::F13:
				value = Key::F13;
				break;
			case gcn::Key::F14:
				value = Key::F14;
				break;
			case gcn::Key::F15:
				value = Key::F15;
				break;
			case gcn::Key::NUM_LOCK:
				value = Key::NUM_LOCK;
				break;
			case gcn::Key::CAPS_LOCK:
				value = Key::CAPS_LOCK;
				break;
			case gcn::Key::SCROLL_LOCK:
				value = Key::SCROLL_LOCK;
				break;
			case gcn::Key::RIGHT_META:
				value = Key::RIGHT_META;
				break;
			case gcn::Key::LEFT_META:
				value = Key::LEFT_META;
				break;
			case gcn::Key::LEFT_SUPER:
				value = Key::LEFT_SUPER;
				break;
			case gcn::Key::RIGHT_SUPER:
				value = Key::RIGHT_SUPER;
				break;
			case gcn::Key::ALT_GR:
				value = Key::ALT_GR;
				break;
			case gcn::Key::UP:
				value = Key::UP;
				break;
			case gcn::Key::DOWN:
				value = Key::DOWN;
				break;
			case gcn::Key::LEFT:
				value = Key::LEFT;
				break;
			case gcn::Key::RIGHT:
				value = Key::RIGHT;
				break;
			case gcn::Key::ENTER:
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
