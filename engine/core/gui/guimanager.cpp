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
		gcn::SDLInput *input = dynamic_cast<gcn::SDLInput*>(m_gcn_gui->getInput());
		if (!input) {
			FL_WARN(_log, "GUIManager, GuichanGUI->getInput == 0 ... discarding events!");
			return false;
		}

		switch(evt.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				if( m_gcn_topcontainer->getWidgetAt(evt.button.x,evt.button.y) ) {
					input->pushInput(evt);
					return true;
				}
				m_focushandler->focusNone();
				return false;

			case SDL_MOUSEMOTION:
				if( m_gcn_topcontainer->getWidgetAt(evt.button.x,evt.button.y) ) {
					m_had_mouse = true;
					input->pushInput(evt);
					return true;
				}
				if( m_had_mouse ) {
					// We only keep the mouse if a widget/window has requested
					// dragging.
					m_had_mouse = bool(m_focushandler->getDraggedWidget());
					input->pushInput(evt);
					return true;
				}
				return false;

			case SDL_KEYDOWN:
			case SDL_KEYUP:
				if(m_focushandler->getFocused()) {
					input->pushInput(evt);
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

		// Guichan has different values from FIFE for modifier keys
		// Left alt: Guichan = -1000, FIFE = 302
		int keyval = gcnevt.getKey().getValue();
		if (keyval < 0) keyval += 1302;
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

}
