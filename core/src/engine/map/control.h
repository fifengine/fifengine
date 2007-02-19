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

#ifndef FIFE_MAP_CONTROL_H
#define FIFE_MAP_CONTROL_H

// Standard C++ library includes
#include <string>
#include <set>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE { 
	class ScriptEngine;
	class SettingsManager;

namespace map {

	class Map;
	class Camera;
	class View;
	class ObjectManager;
	class Runner;

	class Control {
		public:
			Control();
			~Control();
	
			void load(const std::string& filename);

			void start();
			void pause();
			void stop();

			bool isRunning() const;
			void turn();

			void activateElevation(size_t);

			View* getView();
			Map* getMap();

			void addCamera(Camera* camera);
			void removeCamera(Camera* camera);

		protected:
			std::string m_map_filename;
			Map* m_map;
			View* m_view;
			Runner* m_runner;
			ObjectManager* m_om;
			ScriptEngine* m_scriptengine;
			SettingsManager* m_settings;
			std::set<Camera*> m_cameras;

			void resetCameras();
			void registerCommands();
	};

} } // FIFE::map

#endif // FIFE_MAPCONTROL_H
