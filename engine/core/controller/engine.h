/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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
#include "video/devicecaps.h"

namespace FIFE {

	class SoundManager;
	class RenderBackend;
	class IGUIManager;
	class VFS;
	class VFSSourceFactory;
	class EventManager;
	class TimeManager;
	class Model;
	class LogManager;
	class Cursor;
	class RendererBase;
	class Image;
	class ImageManager;
	class AnimationManager;
	class SoundClipManager;
	class OffRenderer;
	class TargetRenderer;

	class IEngineChangeListener {
	public:
		virtual ~IEngineChangeListener() {}

		/** Screen mode has been changed
		 */
		virtual void onScreenModeChanged(const ScreenMode& newmode) = 0;
	};

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

		/** Gets device capabilities
		 */
		const DeviceCaps& getDeviceCaps() const;

		/** Changes the screen mode.
		 * This should be called instead of the renderer's setScreenMode() function.
		 * It takes care of any objects that need to be re-created after switching
		 * screen modes.
		 *
		 * @param mode A valid ScreenMode retrieved from FIFE::DeviceCaps::getNearestScreenMode()
		 */
		void changeScreenMode(const ScreenMode& mode);

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
		SoundManager* getSoundManager() const { return m_soundmanager; }

		/** Provides access point to the EventManager
		 */
		EventManager* getEventManager() const { return m_eventmanager; }

		/** Provides access point to the TimeManager
		 */
		TimeManager* getTimeManager() const { return m_timemanager; }


		/** Sets the GUI Manager to use.  Engine takes
		 * ownership of the manager so DONT DELETE IT!
		 */
		void setGuiManager(IGUIManager* guimanager) { m_guimanager = guimanager; }

		/** Provides access point to the GUI Manager
		 */
		IGUIManager* getGuiManager() const { return m_guimanager; }

		/** Provides access point to the ImageManager
		 */
		ImageManager* getImageManager() const { return m_imagemanager; }

		/** Provides access point to the ImageManager
		 */
		AnimationManager* getAnimationManager() const { return m_animationmanager; }

		/** Provides access point to the SoundClipManager
		 */
		SoundClipManager* getSoundClipManager() const { return m_soundclipmanager; }

		/** Provides access point to the RenderBackend
		 */
		RenderBackend* getRenderBackend() const { return m_renderbackend; }

		/** Provides access point to the Model
		 */
		Model* getModel() const { return m_model; }

		/** Provides access point to the LogManager
		 */
		LogManager* getLogManager() const { return m_logmanager; }

		/** Provides access point to the VFS
		 */
		VFS* getVFS() const { return m_vfs; }

		/** Returns cursor used in the engine
		 */
		Cursor* getCursor() const { return m_cursor; }

		/** Provides access point to the OffRenderer
		 */
		OffRenderer* getOffRenderer() const { return m_offrenderer; }

		/** Provides access point to the TargetRenderer
		 */
		TargetRenderer* getTargetRenderer() const { return m_targetrenderer; }

		/** Adds new change listener
		* @param listener to add
		*/
		void addChangeListener(IEngineChangeListener* listener);

		/** Removes associated change listener
		* @param listener to remove
		*/
		void removeChangeListener(IEngineChangeListener* listener);

	private:
		RenderBackend* m_renderbackend;
		IGUIManager* m_guimanager;
		EventManager* m_eventmanager;
		SoundManager* m_soundmanager;
		TimeManager* m_timemanager;
		ImageManager* m_imagemanager;
		AnimationManager* m_animationmanager;
		SoundClipManager* m_soundclipmanager;

		VFS* m_vfs;
		Model* m_model;
		LogManager* m_logmanager;

		Cursor* m_cursor;
		bool m_destroyed;

		EngineSettings m_settings;
		DeviceCaps m_devcaps;

		ScreenMode m_screenMode;

		OffRenderer* m_offrenderer;
		TargetRenderer* m_targetrenderer;
		std::vector<RendererBase*> m_renderers;

		std::vector<IEngineChangeListener*> m_changelisteners;

#ifdef USE_COCOA
		id m_autoreleasePool;
#endif

	};

}//FIFE

#endif
