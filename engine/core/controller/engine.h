/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_ENGINE_H
#define FIFE_ENGINE_H

// Standard C++ library includes
#include <map>
#include <string>
#include <vector>

// Platform specific includes
#ifdef USE_COCOA
#include <objc/runtime.h>
#endif

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
	class Cursor;
	class SoundClipPool;


	/** Engine acts as a controller to the whole system
	 * Responsibilities of the engine are:
	 *  - Construct and initialize engine internals
	 *  - Clean-up when the program ends
	 *  - Act as an entry point to the engine subsystems
	 */
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
		void init();

		/** Explicit destruction of engine
		 */
		void destroy();
		
		/** Initializes the continuous processing of the engine
		 * Call this only once in your program
		 */
		void initializePumping();
		
		/** Finalizes the continuous processing of the engine
		 * Call this only once in your program, after you have called
		 * initializePumping + (pump() * N times)
		 */
		void finalizePumping();

		/** Runs one cycle for the engine
		 */
		void pump();

		/** Provides access point to the SoundManager
		 */
		SoundManager* getSoundManager() { return m_soundmanager; }
		
		/** Provides access point to the EventManager
		 */
		EventManager* getEventManager() { return m_eventmanager; }
		
		/** Provides access point to the TimeManager
		 */
		TimeManager* getTimeManager() { return m_timemanager; }
		
		/** Provides access point to the GuiManager
		 */
		GUIManager* getGuiManager() { return m_guimanager; }
		
		/** Provides access point to the ImagePool
		 */
		ImagePool* getImagePool() { return m_imagepool; }
		
		/** Provides access point to the AnimationPool
		 */
		AnimationPool* getAnimationPool() { return m_animpool; }

		/** Provides access point to the SoundClipPool
		 */
		SoundClipPool* getSoundClipPool() { return m_soundclippool; }
		
		/** Provides access point to the RenderBackend
		 */
		RenderBackend* getRenderBackend() { return m_renderbackend; }
		
		/** Provides access point to the Model
		 */
		Model* getModel() { return m_model; }
		
		/** Provides access point to the View
		 */
		View* getView() { return m_view; }
		
		/** Provides access point to the LogManager
		 */
		LogManager* getLogManager() { return m_logmanager; }
		
		/** Returns default font used in the engine
		 */
		GuiFont* getDefaultFont() { return m_defaultfont; }
		
		/** Provides access point to the VFS
		 */
		VFS* getVFS() { return m_vfs; }
		
		/** Returns cursor used in the engine
		 */
		Cursor* getCursor() { return m_cursor; }

	private:
		void preInit();
		
		RenderBackend* m_renderbackend;
		GUIManager* m_guimanager;
		EventManager* m_eventmanager;
		SoundManager* m_soundmanager;
		TimeManager* m_timemanager;
		ImagePool* m_imagepool;
		AnimationPool* m_animpool;
		SoundClipPool* m_soundclippool;
		VFS* m_vfs;
		Model* m_model;
		gcn::Graphics* m_gui_graphics;
		View* m_view;
		LogManager* m_logmanager;
		GuiFont* m_defaultfont;
		Cursor* m_cursor;
		bool m_destroyed;
		
		EngineSettings m_settings;

#ifdef USE_COCOA
		objc_object *m_autoreleasePool;
#endif

	};

}//FIFE

#endif
