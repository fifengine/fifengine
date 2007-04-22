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

#include "menutest.h"
#include <SDL.h>

#include "debugutils.h"
#include "imagecache.h"
#include "inputmanager.h"
#include "engine.h"
#include "settingsmanager.h"
#include "scriptbase.h"

#include "video/gui/guimanager.h"
#include "video/rendermanager.h"
#include "video/renderbackend.h"

#include <guichan.hpp>

MenuTest::MenuTest(const std::string &init_with_script) : FIFE::GameState("MenuGameState"), m_fontfile(), m_fpslabel(0) , m_luafile(init_with_script){
	setFontFile(FIFE::SettingsManager::instance()->read<std::string>("FontFile", "lib/fonts/FreeMono.ttf"));

	//FIFE::InputManager* inputmanager = FIFE::InputManager::instance();

}


MenuTest::~MenuTest() {
	delete m_fpslabel;

}

MenuTest::MenuTest(const MenuTest&) : FIFE::GameState("MenuGameState"), m_fontfile() {}

MenuTest& MenuTest::operator=(const MenuTest&) {
	return *this;
}

void MenuTest::setFontFile(const std::string& map) {
	m_fontfile = map;
}

void MenuTest::activate() {
	FIFE::GUIManager* guimanager = FIFE::GUIManager::instance();

	m_fpslabel = new gcn::Label("fps counter                     ");
	m_fpslabel->setPosition(0, 0);
	guimanager->add(m_fpslabel);
  try {
		FIFE::ScriptEngine::instance()->runFile(m_luafile);
	}
	catch (FIFE::Exception & e) {
    FIFE::Log("menutest") << e.getMessage();
		std::exit(1);
	}

}

void MenuTest::deactivate() {
	delete m_fpslabel;
	m_fpslabel = 0;
}

void MenuTest::turn() {

	static int counter = -1;
	static Uint32 starttime = SDL_GetTicks();
	static Uint32 time;

	++counter;
	time = SDL_GetTicks();

	if (time - starttime > 1000) {
		double fps = counter*(1000/double(time - starttime));
		m_fpslabel->setCaption("fps: " + boost::lexical_cast<std::string>(fps));
		starttime = time;
		counter = 0;
	}
}

void MenuTest::handleKeyEvent(const SDL_KeyboardEvent& ev) {
	if (ev.state != SDL_PRESSED)
		return;

	switch (ev.keysym.sym) {
			default:
			break;
	}
}

