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
#ifndef FIFE_INPUTMANAGER_H
#define FIFE_INPUTMANAGER_H

#include "singleton.h"
#include <stdint.h>
#include <map>
#include <vector>

#include "SDL_events.h"


namespace FIFE {

	class InputListener;
	class InputListenerRaw;
	class InputManager : public Singleton<InputManager> {
		public:

			void registerKeyListener(uint8_t key, InputListener* listener);
			void unregisterKeyListener(InputListener*);
			void unregisterListener(InputListener*);

			void setForcedListener(InputListenerRaw* listener);

			void handleEvents();

		private:
			typedef std::multimap<uint8_t, InputListener*> type_listeners_key;
			type_listeners_key m_listeners_key;

			InputListenerRaw* m_forced_listener;

			void handleKeyEvent(const SDL_KeyboardEvent&);

			SINGLEFRIEND(InputManager);

	};

}//FIFE

#endif
