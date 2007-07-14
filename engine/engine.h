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

#ifndef FIFE_ENGINE_H
#define FIFE_ENGINE_H

// Standard C++ library includes
#include <map>
#include <string>
#include <vector>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	namespace audio {
		class Manager;
	}
	class RenderBackend;
	class GUIManager;
	class VFS;
	class EventManager;
	class TimeManager;
	class ImageCache;
	class SettingsManager;

	class Engine {
		public:
			/** Constructor
			 */
			Engine();

			/** Destructor
			 */
			virtual ~Engine();

			void initializePumping();
			void finalizePumping();

			/** Runs one cycle for the engine
			 */
			void pump();

			audio::Manager* getAudioManager();
			EventManager* getEventManager();
			VFS* getVFS();
			TimeManager* getTimeManager();
			SettingsManager* getSettingsManager();
			GUIManager* getGuiManager();

		private:
			/** Initializes the engine
			 */
			void init();

			RenderBackend* m_renderbackend;
			GUIManager* m_guimanager;
			EventManager* m_eventmanager;
			TimeManager* m_timemanager;
			ImageCache* m_imagecache;
	};

}//FIFE

#endif
