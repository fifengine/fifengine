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

#ifndef GSTEST_H
#define GSTEST_H

#include "gamestate.h"
#include "inputlistener.h"
#include "scriptbase.h"
#include <string>

namespace FIFE {
	class Map;
	class MapView;
};

namespace gcn {
	class Label;
}

class GSTest : public FIFE::GameState, public FIFE::InputListener {
	public:
		GSTest(const std::string &init_with_script);
		virtual ~GSTest();

		void setMapFile(const std::string& map);
		void setFontFile(const std::string& map);

		virtual void activate();
		virtual void deactivate();
		virtual void turn();

		virtual void handleKeyEvent(const SDL_KeyboardEvent& event);
		virtual void handleMouseButtonEvent(const SDL_MouseButtonEvent& event);
	private:
		std::string m_mapfile;
		std::string m_fontfile;
		FIFE::Map* m_map;
		FIFE::MapView* m_mapview;
		FIFE::ScriptEngine* m_scriptengine;

		gcn::Label* m_fpslabel;

		GSTest(const GSTest&);
		GSTest& operator=(const GSTest&);
};

#endif
