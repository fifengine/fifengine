/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#include "guimanager.h"
#include "settingsmanager.h"
#include "gcnimageloader.h"
#include "guichan_addon/sdltruetypefont.hpp"
#include "guichan_addon/console.h"
#include "video/rendermanager.h"
#include "video/renderbackend.h"
#include "video/screen.h"

#include <guichan.hpp>
#include <guichan/sdl/sdlinput.hpp>

namespace FIFE {

	GUIManager::GUIManager() : m_gcn_gui(new gcn::Gui()), 
        m_gcn_topcontainer(new gcn::Container()), 
        m_gcn_imgloader(new GCNImageLoader()) , 
        m_input(new gcn::SDLInput()) {
		m_gcn_gui->setTop(m_gcn_topcontainer);
		m_gcn_topcontainer->setOpaque(false);
		m_gcn_gui->setInput(m_input);

		gcn::Image::setImageLoader(m_gcn_imgloader);
	}

	GUIManager::~GUIManager() {
		delete m_console;
		delete m_gcn_topcontainer;
		delete m_gcn_imgloader;
		delete m_input;
		delete m_gcn_gui;
	}

	void GUIManager::resizeTopContainer(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
		m_gcn_topcontainer->setDimension(gcn::Rectangle(x, y, width, height));
	}

	gcn::Gui* GUIManager::getGuichanGUI() const {
		return m_gcn_gui;
	}

	void GUIManager::add(gcn::Widget* widget) {
		m_gcn_topcontainer->add(widget);
	}

	void GUIManager::remove(gcn::Widget* widget) {
		m_gcn_topcontainer->remove(widget);
	}

	void GUIManager::init() {
		Screen* screen = RenderManager::instance()->getCurrentBackend()->getMainScreen();
		m_gcn_gui->setGraphics(screen);
		resizeTopContainer(0, 0, screen->getWidth(), screen->getHeight());

		SettingsManager* settings = SettingsManager::instance();
		std::string fontfile = settings->read<std::string>("GUIFont", "content/fonts/FreeMono.ttf");
		unsigned int fontsize = settings->read<unsigned int>("GUIFontSize", 16);

		gcn::SDLTrueTypeFont* font = new gcn::SDLTrueTypeFont(fontfile, fontsize);
		gcn::Widget::setGlobalFont(font);

		m_console = new Console();

// 		gcn::Icon* testicon = new gcn::Icon(new gcn::Image("art/intrface/about.frm"));
// 		testicon->setPosition(screen->getWidth() -  testicon->getWidth() - 10, 10);
// 		add(testicon);

	}

	void GUIManager::turn() {
		m_gcn_gui->logic();
		m_gcn_gui->draw();
	}
}
