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
#include <guichan.hpp>
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "guichan_addon/console.h"
#include "input/manager.h"
#include "map/control.h"
#include "map/map.h"
#include "map/elevation.h"
#include "map/factory.h"
#include "map/view.h"
#include "vfs/vfs.h"
#include "video/gui/guimanager.h"
#include "video/renderbackend.h"
#include "video/rendermanager.h"
#include "debugutils.h"
#include "engine.h"
#include "imagecache.h"
#include "settingsmanager.h"

#include "gstest.h"

GSTest::GSTest(const std::string &init_with_script) : 
	FIFE::GameState("TestGameState"), m_mapfile(), m_map(0),
	m_mapcontrol(new FIFE::map::Control()) {
}


GSTest::~GSTest() {
	delete m_mapview;
	delete m_map;
	delete m_mapcontrol;
}

GSTest::GSTest(const GSTest&) : FIFE::GameState("TestGameState"), m_mapfile(), m_map(0) {}

GSTest& GSTest::operator=(const GSTest&) {
	return *this;
}

void GSTest::setMapFile(const std::string& map) {
	m_mapfile = map;
}

void GSTest::setFontFile(const std::string& map) {
}

void GSTest::activate() {
	m_mapcontrol->load(m_mapfile);
	m_mapcontrol->start();
	/*if (!m_map) {
		FIFE::Log("gstest") << "couldn't load map: " << m_mapfile;
		exit(1);
	}*/
}

void GSTest::deactivate() {
	m_mapcontrol->stop();
	delete m_map;
	m_map = 0;
}

void GSTest::turn() {
}

void GSTest::handleKeyEvent(const SDL_KeyboardEvent& ev) {
}

void GSTest::handleMouseButtonEvent(const SDL_MouseButtonEvent& event) {
}

void GSTest::writeToDisk(){
	FIFE::file_format* dump_format = FIFE::VFS::instance()->findFileFormat(".dm1");
	if( dump_format ){
		std::string output_file_name = "test";
		if( !m_map ){
			return;
		}
		if(!FIFE::VFS::instance()->fileWrite(reinterpret_cast<void*>(m_map), output_file_name, dump_format)){
				std::cout << "GSTEST: couldn't write file" << std::endl;
		}
	}
}
