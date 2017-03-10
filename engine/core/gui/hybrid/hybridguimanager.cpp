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

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"

#include "hybridguimanager.h"

namespace FIFE {

	static Logger _log(LM_GUI);

	HybridGuiManager::HybridGuiManager() {
	}

	HybridGuiManager::~HybridGuiManager() {
		std::vector<IGUIManager*>::iterator currManager(m_guiManagers.begin());
		std::vector<IGUIManager*>::iterator endManager(m_guiManagers.end());

		for(; currManager != endManager; ++currManager) {
			delete (*currManager);
		}

		m_guiManagers.clear();
	}

	void HybridGuiManager::addGuiManager(IGUIManager* guiManager) {
		m_guiManagers.push_back(guiManager);
	}

	void HybridGuiManager::removeGuiManager(IGUIManager* guiManager) {
		std::vector<IGUIManager*>::iterator currManager(m_guiManagers.begin());
		std::vector<IGUIManager*>::iterator endManager(m_guiManagers.end());

		for(; currManager != endManager;) {
			if((*currManager) == guiManager) {
				m_guiManagers.erase(currManager);
				return;
			}

			++currManager;
		}

		FL_WARN(_log, LMsg() << "Tyring to remove a non-existing gui manager from a hybrid gui manager.");
	}

	void HybridGuiManager::turn() {
		//call turn in reverse order because we want prior gui managers to draw on top
		//of latter ones.
		std::vector<IGUIManager*>::reverse_iterator currManager(m_guiManagers.rbegin());
		std::vector<IGUIManager*>::reverse_iterator endManager(m_guiManagers.rend());

		for(; currManager != endManager; ++currManager) {
			(*currManager)->turn();
		}
	}

	bool HybridGuiManager::onSdlEvent(SDL_Event& event) {

		bool consumed = false;

		std::vector<IGUIManager*>::iterator currManager(m_guiManagers.begin());
		std::vector<IGUIManager*>::iterator endManager(m_guiManagers.end());

		for(; currManager != endManager; ++currManager) {
			consumed = (*currManager)->onSdlEvent(event);

			if (consumed) {
				return true;
			}
		}

		return false;
	}

	void HybridGuiManager::resizeTopContainer(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		std::vector<IGUIManager*>::iterator currManager(m_guiManagers.begin());
		std::vector<IGUIManager*>::iterator endManager(m_guiManagers.end());

		for(; currManager != endManager; ++currManager) {
			(*currManager)->resizeTopContainer(x, y, width, height);
		}
	}
}
