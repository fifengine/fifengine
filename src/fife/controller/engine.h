// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_ENGINE_H
#define FIFE_ENGINE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <map>
#include <memory>
#include <string>
#include <vector>
#ifdef USE_COCOA
    #include <dlfcn.h>
    #include <objc/message.h>
    #include <objc/runtime.h>
#endif

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes
#include "enginesettings.h"
#include "video/devicecaps.h"

namespace FIFE
{

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
    class FontManager;
    class OffRenderer;
    class TargetRenderer;

    class FIFE_API IEngineChangeListener
    {
        public:
            virtual ~IEngineChangeListener() = default;

            /** Screen mode has been changed
             */
            virtual void onScreenModeChanged(ScreenMode const & newmode) = 0;
    };

    /** Engine acts as a controller to the whole system
     * Responsibilities of the engine are:
     *  - Construct and initialize engine internals
     *  - Clean-up when the program ends
     *  - Act as an entry point to the engine subsystems
     */
    class FIFE_API Engine
    {
        public:
            /** Constructor
             */
            Engine();
            Engine(Engine const &)            = delete;
            Engine& operator=(Engine const &) = delete;

            /** Destructor
             */
            virtual ~Engine();

            /** Gets settings class for engine
             */
            EngineSettings& getSettings();

            /** Gets device capabilities
             */
            DeviceCaps const & getDeviceCaps() const;

            /** Changes the screen mode.
             * This should be called instead of the renderer's setScreenMode() function.
             * It takes care of any objects that need to be re-created after switching
             * screen modes.
             *
             * @param mode A valid ScreenMode retrieved from FIFE::DeviceCaps::getNearestScreenMode()
             */
            void changeScreenMode(ScreenMode const & mode);

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
            SoundManager* getSoundManager() const
            {
                return m_soundmanager.get();
            }

            /** Provides access point to the EventManager
             */
            EventManager* getEventManager() const
            {
                return m_eventmanager.get();
            }

            /** Provides access point to the TimeManager
             */
            TimeManager* getTimeManager() const
            {
                return m_timemanager.get();
            }

            /** Sets the GUI Manager to use.  Engine takes
             * ownership of the manager so DONT DELETE IT!
             */
            void setGuiManager(std::unique_ptr<IGUIManager> guimanager);

            /** Provides access point to the GUI Manager
             */
            IGUIManager* getGuiManager() const
            {
                return m_guimanager.get();
            }

            /** Provides access point to the ImageManager
             */
            ImageManager* getImageManager() const
            {
                return m_imagemanager.get();
            }

            /** Provides access point to the ImageManager
             */
            AnimationManager* getAnimationManager() const
            {
                return m_animationmanager.get();
            }

            /** Provides access point to the SoundClipManager
             */
            SoundClipManager* getSoundClipManager() const
            {
                return m_soundclipmanager.get();
            }

            /** Provides access point to the RenderBackend
             */
            RenderBackend* getRenderBackend() const
            {
                return m_renderbackend.get();
            }

            /** Provides access point to the Model
             */
            Model* getModel() const
            {
                return m_model.get();
            }

            /** Provides access point to the LogManager
             */
            LogManager* getLogManager() const
            {
                return m_logmanager;
            }

            /** Provides access point to the VFS
             */
            VFS* getVFS() const
            {
                return m_vfs.get();
            }

            /** Provides access point to the FontManager
             */
            FontManager* getFontManager() const
            {
                return m_fontManager.get();
            }

            /** Loads a font manifest XML string into the FontManager.
             *  Useful for Python applications that need to register fonts dynamically.
             */
            void loadFontManifestFromString(std::string const & xml);

            /** Returns cursor used in the engine
             */
            Cursor* getCursor() const
            {
                return m_cursor.get();
            }

            /** Provides access point to the OffRenderer
             */
            OffRenderer* getOffRenderer() const
            {
                return m_offrenderer.get();
            }

            /** Provides access point to the TargetRenderer
             */
            TargetRenderer* getTargetRenderer() const
            {
                return m_targetrenderer.get();
            }

            /** Adds new change listener
             * @param listener to add
             */
            void addChangeListener(IEngineChangeListener* listener);

            /** Removes associated change listener
             * @param listener to remove
             */
            void removeChangeListener(IEngineChangeListener const * listener);

        private:
            std::unique_ptr<RenderBackend> m_renderbackend;
            std::unique_ptr<IGUIManager> m_guimanager;
            std::unique_ptr<EventManager> m_eventmanager;
            std::unique_ptr<SoundManager> m_soundmanager;
            std::unique_ptr<TimeManager> m_timemanager;
            std::unique_ptr<ImageManager> m_imagemanager;
            std::unique_ptr<AnimationManager> m_animationmanager;
            std::unique_ptr<SoundClipManager> m_soundclipmanager;

            std::unique_ptr<VFS> m_vfs;
            std::unique_ptr<Model> m_model;
            LogManager* m_logmanager;

            std::unique_ptr<Cursor> m_cursor;
            bool m_destroyed;

            EngineSettings m_settings;
            std::unique_ptr<FontManager> m_fontManager;
            DeviceCaps m_devcaps;

            ScreenMode m_screenMode;

            std::unique_ptr<OffRenderer> m_offrenderer;
            std::unique_ptr<TargetRenderer> m_targetrenderer;
            std::vector<std::unique_ptr<RendererBase>> m_renderers;

            std::vector<IEngineChangeListener*> m_changelisteners;

#ifdef USE_COCOA
            id m_autoreleasePool;
#endif
    };

} // namespace FIFE

#endif
