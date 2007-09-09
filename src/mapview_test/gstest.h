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

#ifndef FIFE_MAPVIEW_GSTEST_H
#define FIFE_MAPVIEW_GSTEST_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "input/listener.h"

#include "gamestate.h"

namespace FIFE { namespace map {
	class Map;
	class View;
	class Control;
} };

namespace gcn {
	class Label;
}

class GSTest : public FIFE::GameState, public FIFE::input::Listener {
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
		FIFE::map::Map* m_map;
		FIFE::map::View* m_mapview;
		FIFE::map::Control* m_mapcontrol;

		gcn::Label* m_fpslabel;

		GSTest(const GSTest&);
		GSTest& operator=(const GSTest&);
};

#endif
