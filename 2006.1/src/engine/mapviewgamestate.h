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

#ifndef MAPVIEW_GAMESTATE_H
#define MAPVIEW_GAMESTATE_H

#include "gamestate.h"
#include "inputlistener.h"
#include "scriptbase.h"
#include <string>

namespace FIFE {
	class Map;
	class MapView;

	/** A GameState for showing a map.
	 * This class implements a game state used for showing a Map in the map view.
	 * Currently this proxies a @c MapView and translates input events to
	 * setXXX calls.
	 * You can also set a mapfile, that will be loaded once this state gets activated.
	 * It also does some housekeeping with maps, mapview and the scriptengine.
	 */
	class MapViewGameState : public FIFE::GameState, public FIFE::InputListener {
		public:
			/** Constructor
			 * Create a mapview, register as inputlistener and register the mapview
			 * instance within the scripting engine.
			 * It also currently sets the overlay image for the hexgrid selection tool.
			 */
			MapViewGameState();

			/** Destructor
			 */
			virtual ~MapViewGameState();

			/** Set the mapfile that will be loaded when this state gets activeted
			 * @param map The map filename.
			 */
			void setMapFile(const std::string& map);
	
			/** Activate this game state
			 * Load the mapfile, set the map in the mapview and set 0 as active elevation.
			 * If the map file couldn't be loaded, exit the program.
			 * Also push an inputcontext "map_view" that will get the events.
			 */
			virtual void activate();

			/** Deactivate this game state.
			 * Delete the map, deactivate the mapview and pop the inputcontext.
			 */
			virtual void deactivate();

			/** Just be!
			 * Calls @c mapview->render()
			 */
			virtual void turn();
	
			/** Handle a event
			 * Modifies the MapView according to incoming events.
			 */
			virtual void handleEvent(int event);

			/** Handle a mouse click on the mapview
			 */
			virtual void handleMouseButtonEvent(const SDL_MouseButtonEvent& event);

			/** Get the current map
			 */
			Map* getMap() { return m_map; }

			/** Get the map view
			 */
			MapView* getMapView() { return m_mapview; }
			
		private:
			std::string m_mapfile;
			Map* m_map;
			MapView* m_mapview;
			ScriptEngine* m_scriptengine;
	
			/// Private copy constructors to prevent accidental use
			MapViewGameState(const MapViewGameState&);
			MapViewGameState & operator=(const MapViewGameState&);
	};

}
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
