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
#include "enginesettings.h"

namespace gcn {
	class Graphics;
}

namespace FIFE {

	class SoundManager;
	class RenderBackend;
	class GUIManager;
	class VFS;
	class VFSSourceFactory;
	class EventManager;
	class TimeManager;
	class ImagePool;
	class AnimationPool;
	class View;
	class Model;
	class LogManager;
	class GuiFont;

	class Engine {
	public:
		/** Constructor
		 */
		Engine();

		/** Destructor
		 */
		virtual ~Engine();

		/** Gets settings class for engine
		 */
		EngineSettings& getSettings();
		
		/** Initializes the engine
		 */
		void init() throw(NotSet);
		
		void initializePumping();
		void finalizePumping();

		/** Runs one cycle for the engine
		 */
		void pump();

		SoundManager* getSoundManager() { return m_soundmanager; }
		EventManager* getEventManager() { return m_eventmanager; }
		TimeManager* getTimeManager() { return m_timemanager; }
		GUIManager* getGuiManager() { return m_guimanager; }
		ImagePool* getImagePool() { return m_imagepool; }
		AnimationPool* getAnimationPool() { return m_animpool; }
		RenderBackend* getRenderBackend() { return m_renderbackend; }
		Model* getModel() { return m_model; }
		View* getView() { return m_view; }
		LogManager* getLogManager() { return m_logmanager; }
		GuiFont* getDefaultFont() { return m_defaultfont; }

	private:
		void preInit();
		
		RenderBackend* m_renderbackend;
		GUIManager* m_guimanager;
		EventManager* m_eventmanager;
		SoundManager* m_soundmanager;
		TimeManager* m_timemanager;
		ImagePool* m_imagepool;
		AnimationPool* m_animpool;
		VFSSourceFactory* m_vfs_sourcefactory;
		VFS* m_vfs;
		Model* m_model;
		gcn::Graphics* m_gui_graphics;
		View* m_view;
		LogManager* m_logmanager;
		GuiFont* m_defaultfont;
		
		EngineSettings m_settings;
	};

}//FIFE

#endif
