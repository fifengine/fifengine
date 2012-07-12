/***************************************************************************
 *   Copyright (C) 2005-2012 by the FIFE team                              *
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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/time/timemanager.h"

#include "librocketmanager.h"

namespace FIFE {
	
	LibRocketManager::LibRocketManager() {
	}
	
	LibRocketManager::~LibRocketManager() {
	}
	
	float LibRocketManager::GetElapsedTime() {
		static TimeManager *timeManager = TimeManager::instance();
		
		return timeManager->getTime() / 1000.0f;
	}
	
	void LibRocketManager::turn() {
	}
	
	void LibRocketManager::resizeTopContainer(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	}
	
	Rocket::Core::ElementDocument* LibRocketManager::loadDocument(const std::string& documentPath) {
		
	}
	
	void LibRocketManager::unloadDocument(Rocket::Core::ElementDocument* document) {
	}
	
	bool LibRocketManager::onSdlEvent(SDL_Event& evt) {
	}
};