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

// Standard C++ library includes
#include <iostream>

// 3rd party library includes
#include <guichan/sdl/sdlinput.hpp>
#include <guichan/focushandler.hpp>
#include <guichan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/logger.h"
#include "video/renderbackend.h"
#include "gui/base/gui_imageloader.h"
#include "util/settingsmanager.h"
#include "gui/console/console.h"
#include "gui/fonts/fontbase.h"
#include "eventchannel/widget/ec_widgetevent.h"

#include "guimanager.h"


namespace FIFE {
	static Logger _log(LM_GUI);

	GUIManager::GUIManager(IWidgetListener* widgetlistener, ImagePool& pool) : 
		m_gcn_gui(new gcn::Gui()), 
		m_focushandler(0),
        	m_gcn_topcontainer(new gcn::Container()), 
        	m_imgloader(new GuiImageLoader(pool)) , 
        	m_input(new gcn::SDLInput()),
		m_widgetlistener(widgetlistener),
		m_pool(pool) {
		m_font = 0;
		m_console = 0;

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
		delete m_font;
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

	void GUIManager::setGlobalFont(gcn::Font* font) {
		if( font == 0) {
			gcn::Widget::setGlobalFont(m_font);
		} else {
			gcn::Widget::setGlobalFont(font);
		}
	}

	void GUIManager::turn() {
		m_gcn_gui->logic();
		m_gcn_gui->draw();
	}

	void GUIManager::action(const gcn::ActionEvent & event) {
		WidgetEvent wevt;
		wevt.setId(event.getId());
		m_widgetlistener->onWidgetAction(wevt);
	}

	void GUIManager::evaluateKeyEventConsumption(IKeyEvent& evt) {
		gcn::Widget* w = m_focushandler->getFocused();
		if (w) {
			evt.consume();
		}
	}

	void GUIManager::evaluateMouseEventConsumption(IMouseEvent& evt) {
		gcn::Widget* w = m_gcn_topcontainer->getWidgetAt(evt.getX(), evt.getY());
		if (w && w->isVisible()) {
			evt.consume();
		}
	}

	void GUIManager::mousePressed(IMouseEvent& evt) {
 		evaluateMouseEventConsumption(evt);
		if (!evt.isConsumed()) {
			m_focushandler->focusNone();
		}
	}
}
