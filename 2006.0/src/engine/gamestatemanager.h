/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
#ifndef FIFE_GAMESTATEMANAGER_H
#define FIFE_GAMESTATEMANAGER_H

#include "singleton.h"
#include <map>
#include <vector>
#include <string>

namespace FIFE {
	static const std::string DBGTYPE = "GameStates";

	class GameState;
	class GameStateManager : public Singleton<GameStateManager> {
		public:
			void registerState(GameState*);
			void unregisterState(GameState*);

			bool gameStateExists(const std::string& name) const;
			GameState* getGameState(const std::string& name) const;
			void activate(const std::string& name);

			void turn();

		private:
			typedef std::map<std::string, GameState*> type_gamestates;
			typedef std::vector<GameState*> type_gamestates_active;
			type_gamestates m_gamestates;
			type_gamestates_active m_gamestates_active;

			SINGLEFRIEND(GameStateManager);

	};

}//FIFE

#endif
