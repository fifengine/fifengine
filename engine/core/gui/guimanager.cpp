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
#include "util/log/logger.h"
#include "video/renderbackend.h"
#include "gui/base/gui_imageloader.h"
#include "gui/base/gui_font.h"
#include "gui/console/console.h"
#include "video/fonts/fontbase.h"
#include "video/fonts/truetypefont.h"
#include "video/fonts/subimagefont.h"
#include "eventchannel/widget/ec_widgetevent.h"
#include "eventchannel/key/ec_keyevent.h"
#include "eventchannel/mouse/ec_mouseevent.h"

#include "guimanager.h"


namespace FIFE {
	static Logger _log(LM_GUI);

	GUIManager::GUIManager(IWidgetListener* widgetlistener, ImagePool& pool) : 
		m_gcn_gui(new gcn::Gui()), 
		m_focushandler(0),
        	m_gcn_topcontainer(new gcn::Container()), 
        	m_imgloader(new GuiImageLoader(pool)) , 
        	m_input(new gcn::SDLInput()),
        	m_console(0),
        	m_fonts(),
		m_widgetlistener(widgetlistener),
		m_pool(pool),
		m_logic_executed(false) {
		
		m_gcn_gui->setTop(m_gcn_topcontainer);
		m_gcn_topcontainer->setOpaque(false);
		m_gcn_gui->setInput(m_input);

		gcn::Image::setImageLoader(m_imgloader);
		m_focushandler = m_gcn_topcontainer->_getFocusHandler();
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

	void GUIManager::onSdlEvent(SDL_Event& evt) {
		gcn::SDLInput *input = dynamic_cast<gcn::SDLInput*>(m_gcn_gui->getInput());
		if (!input) {
			FL_WARN(_log, "GUIManager, GuichanGUI->getInput == 0 ... discarding events!");
			return;
		}
		input->pushInput(evt);
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
		if (!m_logic_executed) {
			// Due to a BUG in Guichan we need to catch GCN exceptions
			// This is a potentially dangerous workaround put in place
			// until we upgrade to Guichan 0.8.0
			// See here: http://code.google.com/p/guichan/issues/detail?id=24
			try {
				m_gcn_gui->logic();
			} catch( const gcn::Exception& e) {
				FL_WARN(_log, LMsg("GUIManager, discarding gcn::Exception: ") << e.getMessage());
			}
		}
		m_gcn_gui->draw();
		m_logic_executed = false;
	}

	void GUIManager::action(const gcn::ActionEvent & event) {
		WidgetEvent wevt;
		wevt.setId(event.getId());
		wevt.setSourceWidget(event.getSource());
		m_widgetlistener->onWidgetAction(wevt);
	}

	void GUIManager::evaluateKeyEventConsumption(KeyEvent& evt) {
		gcn::Widget* w = m_focushandler->getFocused();
		if (w) {
			evt.consumedByWidgets();
		}
	}

	void GUIManager::evaluateMouseEventConsumption(MouseEvent& evt) {
		gcn::Widget* w = m_gcn_topcontainer->getWidgetAt(evt.getX(), evt.getY());
		if (w && w->isVisible()) {
			evt.consumedByWidgets();
		}
	}

	void GUIManager::mousePressed(MouseEvent& evt) {
 		evaluateMouseEventConsumption(evt);
		if (!evt.isConsumedByWidgets()) {
			m_focushandler->focusNone();
		}
	}
	
	void GUIManager::mouseDragged(MouseEvent& evt) {
		try {
			m_gcn_gui->logic();
			m_logic_executed = true;
		} catch( const gcn::Exception& e) {
			FL_WARN(_log, LMsg("GUIManager, discarding gcn::Exception: ") << e.getMessage());
		}
		evaluateMouseEventConsumption(evt);
	}
}
