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
#ifndef FIFE_GAMESTATE_H
#define FIFE_GAMESTATE_H

#include <string>

namespace FIFE {

	class GameState {
		public:
			GameState(const std::string& name);
			virtual ~GameState();

			const std::string& getName() const;

			virtual void activate(); // called when gamestate gets activated
			virtual void deactivate(); // called when gamestate gets deactivated
			virtual void turn(); // called while gamestate is active

		private:
			std::string m_name;
	};

}//FIFE

#endif
