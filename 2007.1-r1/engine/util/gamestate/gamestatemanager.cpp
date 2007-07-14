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
#include <algorithm>
#include <functional>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/debugutils.h"
#include "util/exception.h"

#include "gamestatemanager.h"
#include "gamestate.h"

namespace FIFE {

	GameStateManager::GameStateManager() : m_gamestates(), m_gamestates_active() {}

	GameStateManager::~GameStateManager() {}

	void GameStateManager::registerState(GameState* gs) {
		const std::string& name = gs->getName();
		if (gameStateExists(name)) {
			PANIC_PRINT("name already used: " << name);
			return;
		}

		m_gamestates.insert(std::make_pair(name, gs));
		Log("GameStateManager") << "registered gamestate: " << gs->getName();
	}

	void GameStateManager::unregisterState(GameState* gs) {
		if (!gs) {
			PANIC_PRINT("Unregistering gs == NULL");
		}

		if (isActive(gs->getName())) {
			Warn("GSTEST") << "Unregistering _active_ GameState " << gs->getName() << ", deactivating it first.";
			deactivate(gs->getName());
		}

		type_gamestates::iterator i = m_gamestates.find(gs->getName());
		if (i == m_gamestates.end()) {
			PANIC_PRINT("tried to unregister unknown gamestate: " << gs->getName());
			return;
		}

		m_gamestates.erase(i);
		Log("GameStateManager") << "unregistered gamestate: " << gs->getName();
	}

	bool GameStateManager::gameStateExists(const std::string& name) const {
		return m_gamestates.find(name) != m_gamestates.end();
	}

	GameState* GameStateManager::getGameState(const std::string& name) const {
		type_gamestates::const_iterator i = m_gamestates.find(name);
		if (i == m_gamestates.end())
			throw NotFound("Gamestate not found: " + name);

		return i->second;
	}

	void GameStateManager::activate(const std::string& name) {
		if (isActive(name)) {
			Warn("GameStateManager") << "Activating GameState " << name << " which is already active!";
			return;
		}

		GameState* gs = getGameState(name);
		m_gamestates_active.push_back(gs);
		gs->activate();
	}

	void GameStateManager::deactivate(const std::string & name) {
		GameState* gs = getGameState(name);
		Log("GameStateManager") << "looking for " << name;
		type_gamestates_active::iterator i = m_gamestates_active.begin();
		type_gamestates_active::iterator e = m_gamestates_active.end();
		while (i != e) {
			if (*i == gs) {
				Log("GameStateManager") << "HAVE FOUND IT! " << name;
				gs->deactivate();
				m_gamestates_active.erase(i);
				return;
			}
			i++;
		}
		Log("GameStateManager") << "ERROR: should not be reached!";
	}

	bool GameStateManager::isActive(const std::string& name) const {
		GameState *gs = getGameState(name);
		type_gamestates_active::const_iterator end = m_gamestates_active.end();
		for (type_gamestates_active::const_iterator it = m_gamestates_active.begin(); it != end; ++it) {
			if (gs == *it) {
				return true;
			}
		}
		return false;
	}

	void GameStateManager::turn() {
		// Needs a copy because if one gamestate activates/deactivates another gamestate the iterators would be invalid
		type_gamestates_active active_copy = m_gamestates_active;
		std::for_each(active_copy.begin(), active_copy.end(), std::mem_fun(&GameState::turn));
	}

}//FIFE
