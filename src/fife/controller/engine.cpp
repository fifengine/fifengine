// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "engine.h"

// Standard C++ library includes
#include <algorithm>
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

// FIFE includes
#include "audio/soundclipmanager.h"
#include "audio/soundmanager.h"
#include "eventchannel/eventmanager.h"
#include "gui/fifechan/fifechanmanager.h"
#include "gui/guimanager.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/time/timemanager.h"
#include "vfs/directoryprovider.h"
#include "vfs/filesystemassetprovider.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "vfs/vfsassetprovider.h"
#include "vfs/vfsdirectory.h"
#include "vfs/zip/zipprovider.h"
#include "video/animationmanager.h"
#include "video/cursor.h"
#include "video/fonts/fontmanager.h"
#include "video/imagemanager.h"
#include "video/renderbackend.h"
#include "video/window/window.h"
#ifdef HAVE_OPENGL
    #include "video/opengl/fife_opengl.h"
    #include "video/opengl/renderbackendopengl.h"
#endif
#include "loaders/native/audio/ogg_loader.h"
#include "loaders/native/video/imageloader.h"
#include "model/metamodel/grids/hexgrid.h"
#include "model/metamodel/grids/squaregrid.h"
#include "model/model.h"
#include "pathfinder/routepather/routepather.h"
#include "version.hpp"
#include "video/image.h"
#include "video/sdl/renderbackendsdl.h"
#include "view/renderers/blockinginforenderer.h"
#include "view/renderers/cellrenderer.h"
#include "view/renderers/cellselectionrenderer.h"
#include "view/renderers/coordinaterenderer.h"
#include "view/renderers/floatingtextrenderer.h"
#include "view/renderers/genericrenderer.h"
#include "view/renderers/gridrenderer.h"
#include "view/renderers/instancerenderer.h"
#include "view/renderers/lightrenderer.h"
#include "view/renderers/offrenderer.h"
#include "view/renderers/pathrenderer.h"
#include "view/renderers/quadtreerenderer.h"
#include "view/renderers/targetrenderer.h"

#ifdef USE_COCOA
int32_t main(int32_t argc, char** argv)
{
    return 0;
}
#endif

namespace FIFE
{
    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_CONTROLLER);
            return log;
        }
    } // namespace

    Engine::Engine() : m_logmanager(&LogManager::instance()), m_destroyed(false)

    {
#ifdef USE_COCOA
        // The next lines ensure that Cocoa is initialzed correctly.
        // This is needed for SDL to function properly on MAC OS X.
        void* cocoa_lib;
        cocoa_lib = dlopen("/System/Library/Frameworks/Cocoa.framework/Cocoa", RTLD_LAZY);
        void (*nsappload)(void);
        nsappload = reinterpret_cast<void (*)()>(dlsym(cocoa_lib, "NSApplicationLoad"));
        nsappload();

        // Create an autorelease pool, so autoreleased SDL objects don't leak.
    #ifdef OSX_109
        Class NSAutoreleasePool = objc_getClass("NSAutoreleasePool");
        m_autoreleasePool       = class_createInstance(NSAutoreleasePool, 0);
    #else
        objc_object* NSAutoreleasePool = objc_getClass("NSAutoreleasePool");
        m_autoreleasePool              = objc_msgSend(NSAutoreleasePool, sel_registerName("new"));
    #endif
#endif
    }

    EngineSettings& Engine::getSettings()
    {
        return m_settings;
    }

    DeviceCaps const & Engine::getDeviceCaps() const
    {
        return m_devcaps;
    }

    void Engine::setGuiManager(std::unique_ptr<IGUIManager> guimanager)
    {
        if (auto* fifechan = dynamic_cast<FifechanManager*>(guimanager.get())) {
            fifechan->setFontManager(m_fontManager.get());
        }
        m_guimanager = std::move(guimanager);
    }

    void Engine::loadFontManifestFromString(std::string const & xml)
    {
        if (m_fontManager) {
            m_fontManager->loadManifestFromString(xml);
        }
    }

    void Engine::toggleFullscreen()
    {
        if (m_window == nullptr) {
            return;
        }
        WindowSettings settings = m_window->getSettings();
        settings.windowMode =
            (settings.windowMode == WindowMode::Windowed) ? WindowMode::Fullscreen : WindowMode::Windowed;
        changeWindowMode(settings);
    }

    void Engine::toggleVSync()
    {
        if (m_window == nullptr) {
            return;
        }
        bool const current = m_window->getVSync();
        m_window->setVSync(!current);
    }

    void Engine::changeWindowMode(WindowSettings const & settings)
    {
        if (m_window == nullptr) {
            return;
        }
        m_window->create(settings);
        if (m_guimanager != nullptr) {
            m_guimanager->resizeTopContainer(
                0U, 0U, static_cast<uint32_t>(settings.width), static_cast<uint32_t>(settings.height));
        }
        for (auto* listener : m_changelisteners) {
            listener->onWindowModeChanged(settings);
        }
    }

    void Engine::init()
    {
        m_destroyed = false;

        FL_LOG(_log(), std::format("Fifengine v{}", FIFE::app_version::get_version()));
        FL_LOG(_log(), "================== Engine initialize start =================");
        m_timemanager = std::make_unique<TimeManager>();
        FL_LOG(_log(), "Time manager created");

        FL_LOG(_log(), "Creating VFS");
        m_vfs = std::make_unique<VFS>();

        FL_LOG(_log(), "Adding root directory to VFS");
        m_vfs->addSource(std::make_unique<VFSDirectory>(m_vfs.get()));
        m_vfs->addProvider(std::make_unique<DirectoryProvider>());

        FL_LOG(_log(), "Adding zip provider to VFS");
        m_vfs->addProvider(std::make_unique<ZipProvider>());

        // FL_LOG(_log(), "Adding Fallout DAT2 provider to VFS");
        // m_vfs->addProvider(ProviderDAT2());

        // FL_LOG(_log(), "Adding Fallout DAT1 provider to VFS");
        // m_vfs->addProvider(ProviderDAT1());

        TTF_Init();

        FL_LOG(_log(), "Creating FontManager");
        {
            auto resolver = std::make_unique<AssetResolver>();
            resolver->addProvider(std::make_unique<VfsAssetProvider>(m_vfs.get()));
            resolver->addProvider(std::make_unique<FilesystemAssetProvider>(m_settings.getFontPaths()));
            m_fontManager = std::make_unique<FontManager>(std::move(resolver));
        }

        try {
            if (m_vfs->exists("config/fonts.xml")) {
                FL_LOG(_log(), "Loading font manifest from config/fonts.xml");
                auto fontManifest = m_vfs->open("config/fonts.xml");
                if (fontManifest) {
                    auto bytes = fontManifest->getDataInBytes();
                    std::string const xmlContent(bytes.begin(), bytes.end());
                    m_fontManager->loadManifestFromString(xmlContent);
                }
            }
        } catch (std::exception const & e) {
            FL_WARN(_log(), std::format("Failed to load font manifest: {}", e.what()));
        }

        FL_LOG(_log(), "Engine pre-init done");

        FL_LOG(_log(), "Creating event manager");
        m_eventmanager = std::make_unique<EventManager>();
        m_eventmanager->setMouseSensitivity(m_settings.getMouseSensitivity());
        m_eventmanager->setMouseAccelerationEnabled(m_settings.isMouseAccelerationEnabled());
        m_eventmanager->setJoystickSupport(m_settings.isJoystickSupport());

        FL_LOG(_log(), "Creating resource managers");

        m_imagemanager     = std::make_unique<ImageManager>();
        m_animationmanager = std::make_unique<AnimationManager>();
        m_soundclipmanager = std::make_unique<SoundClipManager>();

        FL_LOG(_log(), "Creating render backend");
        std::string const rbackend(m_settings.getRenderBackend());
        if (rbackend == "SDL") {
            m_renderbackend = std::make_unique<RenderBackendSDL>(m_settings.getColorKey());
            FL_LOG(_log(), "SDL Render backend created");
        } else {
#ifdef HAVE_OPENGL
            m_renderbackend = std::make_unique<RenderBackendOpenGL>(m_settings.getColorKey());
            FL_LOG(_log(), "OpenGL Render backend created");
#else
            m_renderbackend = std::make_unique<RenderBackendSDL>(m_settings.getColorKey());
            // Remember  the choice so we pick the right graphics class.
            rbackend = "SDL";
            FL_WARN(
                _log(),
                "Tried to select OpenGL, even though it is not compiled into the engine. Falling back to SDL Render "
                "backend");
#endif
        }
        FL_LOG(_log(), "Initializing render backend");
        m_renderbackend->setColorKeyEnabled(m_settings.isColorKeyEnabled());
        // we always set this to false
        // m_renderbackend->setAlphaOptimizerEnabled(false);
        m_renderbackend->setImageCompressingEnabled(m_settings.isGLCompressImages());
        m_renderbackend->setFramebufferEnabled(m_settings.isGLUseFramebuffer());
        m_renderbackend->setNPOTEnabled(m_settings.isGLUseNPOT());
        m_renderbackend->setTextureFiltering(m_settings.getGLTextureFiltering());
        m_renderbackend->setMipmappingEnabled(m_settings.isGLUseMipmapping());
        m_renderbackend->setMonochromeEnabled(m_settings.isGLUseMonochrome());
        m_renderbackend->setDepthBufferEnabled(m_settings.isGLUseDepthBuffer());
        m_renderbackend->setAlphaTestValue(m_settings.getGLAlphaTestValue());
        m_renderbackend->setVSyncEnabled(m_settings.isVSync());
        if (m_settings.isFrameLimitEnabled()) {
            m_renderbackend->setFrameLimitEnabled(true);
            m_renderbackend->setFrameLimit(m_settings.getFrameLimit());
        }

        std::string driver = m_settings.getVideoDriver();
        if (!driver.empty()) {
            std::vector<std::string> drivers = m_devcaps.getAvailableVideoDrivers();
            if (std::ranges::find(drivers, driver) == drivers.end()) {
                FL_WARN(
                    _log(),
                    "Selected video driver is not supported for your Operating System!  Reverting to default driver.");
                driver = "";
            }
            m_devcaps.setVideoDriverName(driver);
        }
        // init backend with selected video driver or default
        m_renderbackend->init(driver);

        // in case of SDL we use this to create the SDL_Renderer
        driver = m_settings.getSDLDriver();
        if (!driver.empty()) {
            std::vector<std::string> drivers = m_devcaps.getAvailableRenderDrivers();
            if (std::ranges::find(drivers, driver) == drivers.end()) {
                FL_WARN(
                    _log(),
                    "Selected render driver is not supported for your Operating System!  Reverting to default driver.");
                driver = "";
            }
            m_devcaps.setRenderDriverName(driver);
        }

        FL_LOG(_log(), "Querying device capabilities");
        m_devcaps.fillDeviceCaps();

        uint8_t const displayCount = m_devcaps.getDisplayCount();
        if (displayCount == 0) {
            throw NotSupported("Could not find any display!");
        }

        uint8_t const requestedDisplay = m_settings.getDisplay();
        uint8_t selectedDisplay        = requestedDisplay;
        if (requestedDisplay >= displayCount) {
            FL_WARN(
                _log(),
                std::format(
                    "Selected display index {} is out of range [0, {}] and will fall back to display 0.",
                    static_cast<int32_t>(requestedDisplay),
                    static_cast<int32_t>(displayCount - 1)));
            selectedDisplay = 0;
        }

        uint16_t const requestedWidth  = m_settings.getScreenWidth();
        uint16_t const requestedHeight = m_settings.getScreenHeight();
        uint16_t selectedWidth         = requestedWidth;
        uint16_t selectedHeight        = requestedHeight;

        Rect const displayBounds = m_devcaps.getDisplayBounds(selectedDisplay);
        int32_t const maxWidth   = std::max<int32_t>(1, displayBounds.w);
        int32_t const maxHeight  = std::max<int32_t>(1, displayBounds.h);
        selectedWidth            = static_cast<uint16_t>(std::clamp<int32_t>(selectedWidth, 1, maxWidth));
        selectedHeight           = static_cast<uint16_t>(std::clamp<int32_t>(selectedHeight, 1, maxHeight));
        if (selectedWidth != requestedWidth || selectedHeight != requestedHeight) {

            FL_WARN(
                _log(),
                std::format(
                    "Requested resolution {}x{} exceeds selected display bounds {}x{}. Clamping to {}x{}.",
                    requestedWidth,
                    requestedHeight,
                    displayBounds.w,
                    displayBounds.h,
                    selectedWidth,
                    selectedHeight));
        }

        FL_LOG(
            _log(),
            std::format(
                "Using display index {} (display {}) out of {} displays with resolution {}x{}",
                static_cast<int32_t>(selectedDisplay),
                static_cast<int32_t>(selectedDisplay + 1),
                static_cast<int32_t>(displayCount),
                selectedWidth,
                selectedHeight));

        WindowSettings winSettings;
        winSettings.width       = selectedWidth;
        winSettings.height      = selectedHeight;
        winSettings.display     = selectedDisplay;
        winSettings.x           = m_settings.getWindowPositionX();
        winSettings.y           = m_settings.getWindowPositionY();
        winSettings.title       = m_settings.getWindowTitle();
        winSettings.icon        = m_settings.getWindowIcon();
        winSettings.resizable   = m_settings.isWindowResizable();
        winSettings.vsync       = m_settings.isVSync();
        winSettings.opengl      = (dynamic_cast<RenderBackendOpenGL*>(m_renderbackend.get()) != nullptr);
        winSettings.windowMode  = m_settings.isFullScreen() ? WindowMode::Fullscreen : WindowMode::Windowed;
        winSettings.scalingMode = static_cast<ScalingMode>(m_settings.getWindowScalingMode());

        m_window = std::make_unique<Window>();
        m_window->create(winSettings);
        m_window->signalResized = [this](int w, int h) {
            if (m_imagemanager) {
                m_imagemanager->invalidateAll();
            }
            if (m_guimanager) {
                m_guimanager->resizeTopContainer(0U, 0U, static_cast<uint32_t>(w), static_cast<uint32_t>(h));
            }
            for (auto* listener : m_changelisteners) {
                listener->onWindowResized(w, h);
            }
        };
        m_window->signalDPIScaleChanged = [this](float scale) {
            if (m_imagemanager) {
                m_imagemanager->invalidateAll();
            }
            if (m_fontManager) {
                m_fontManager->setDPIScaleForAll(scale);
            }
            if (m_guimanager) {
                int const w = m_window->getWidthInPoints();
                int const h = m_window->getHeightInPoints();
                m_guimanager->resizeTopContainer(0U, 0U, static_cast<uint32_t>(w), static_cast<uint32_t>(h));
            }
            for (auto* listener : m_changelisteners) {
                listener->onDPIScaleChanged(scale);
            }
        };
        m_renderbackend->setWindowObject(m_window.get());

        FL_LOG(_log(), "Creating main screen");
        m_renderbackend->createMainScreen(m_settings.getWindowTitle(), m_settings.getWindowIcon());
        FL_LOG(_log(), "Main screen created");
        m_window->show();

        if (SDL_Window* win = m_window->getSDLWindow()) {
            SDL_StartTextInput(win);
        }

#ifdef HAVE_OPENGL
        if (m_settings.getLightingModel() != 0) {
            m_renderbackend->setLightingModel(m_settings.getLightingModel());
        }

#endif
        FL_LOG(_log(), "Creating sound manager");
        m_soundmanager = std::make_unique<SoundManager>();
        m_soundmanager->setVolume(m_settings.getInitialVolume() / 10.0F);

        FL_LOG(_log(), "Creating renderers");
        m_offrenderer    = std::make_unique<OffRenderer>(m_renderbackend.get());
        m_targetrenderer = std::make_unique<TargetRenderer>(m_renderbackend.get());
        m_renderers.push_back(std::make_unique<InstanceRenderer>(m_renderbackend.get(), 10));
        m_renderers.push_back(std::make_unique<GridRenderer>(m_renderbackend.get(), 20));
        m_renderers.push_back(std::make_unique<CellSelectionRenderer>(m_renderbackend.get(), 30));
        m_renderers.push_back(std::make_unique<BlockingInfoRenderer>(m_renderbackend.get(), 40));
        m_renderers.push_back(std::make_unique<FloatingTextRenderer>(m_renderbackend.get(), 50));
        m_renderers.push_back(std::make_unique<QuadTreeRenderer>(m_renderbackend.get(), 60));
        m_renderers.push_back(std::make_unique<CoordinateRenderer>(m_renderbackend.get(), 70));
        m_renderers.push_back(std::make_unique<GenericRenderer>(m_renderbackend.get(), 80));
        m_renderers.push_back(std::make_unique<LightRenderer>(m_renderbackend.get(), 90));
        m_renderers.push_back(std::make_unique<CellRenderer>(m_renderbackend.get(), 100));
        m_renderers.push_back(std::make_unique<PathRenderer>(m_renderbackend.get(), 105));

        FL_LOG(_log(), "Creating model");
        m_model = std::make_unique<Model>(m_renderbackend.get(), m_renderers);
        FL_LOG(_log(), "Adding pathers to model");
        m_model->adoptPather(std::make_unique<RoutePather>());
        FL_LOG(_log(), "Adding grid prototypes to model");
        m_model->adoptCellGrid(std::make_unique<SquareGrid>());
        m_model->adoptCellGrid(std::make_unique<HexGrid>(false));
        m_model->adoptCellGrid(std::make_unique<HexGrid>(true));

        m_cursor = std::make_unique<Cursor>(m_renderbackend.get());
        m_cursor->setNativeImageCursorEnabled(m_settings.isNativeImageCursorEnabled());
        m_window->onClosed([this]() {
            if (m_cursor != nullptr) {
                m_cursor->invalidate();
            }
        });
        FL_LOG(_log(), "Engine initialized");
    }

    Engine::~Engine()
    {
        if (!m_destroyed) {
            destroy();
        }
    }

    void Engine::destroy()
    {
        FL_LOG(_log(), "Destructing engine");
        // unique_ptr members automatically cleaned up

        TTF_Quit();
        SDL_Quit();

#ifdef USE_COCOA
        objc_msgSend(m_autoreleasePool, sel_registerName("release"));
#endif

        FL_LOG(_log(), "================== Engine destructed ==================");
        m_destroyed = true;
    }
    void Engine::initializePumping()
    {
        m_eventmanager->processEvents();
    }

    void Engine::pump()
    {
        if (m_eventmanager) {
            bool const f11down = m_eventmanager->isKeyPressed(Keys::F11);
            if (f11down && !m_lastF11State) {
                toggleFullscreen();
            }
            m_lastF11State = f11down;

            bool const ctrlPressed =
                m_eventmanager->isKeyPressed(Keys::LCTRL) || m_eventmanager->isKeyPressed(Keys::RCTRL);
            bool const vDown = m_eventmanager->isKeyPressed(Keys::V);
            if (ctrlPressed && vDown && !m_lastCtrlVState) {
                toggleVSync();
            }
            m_lastCtrlVState = vDown;
        }
        m_renderbackend->startFrame();
        m_eventmanager->processEvents();
        if (m_window != nullptr) {
            m_window->updateDPIScaleIfNeeded();
        }
        m_timemanager->update();
        m_soundmanager->update();

        m_targetrenderer->render();
        if (m_model->getActiveCameraCount() == 0) {
            m_renderbackend->clearBackBuffer();
            m_offrenderer->render();
        } else {
            m_model->update();
        }

        if (m_guimanager != nullptr) {
            m_guimanager->turn();
        }

        m_cursor->draw();
        m_renderbackend->endFrame();
    }

    void Engine::finalizePumping()
    {
        // nothing here at the moment..
    }

    void Engine::addChangeListener(IEngineChangeListener* listener)
    {
        m_changelisteners.push_back(listener);
    }

    void Engine::removeChangeListener(IEngineChangeListener const * listener)
    {
        auto i = m_changelisteners.begin();
        while (i != m_changelisteners.end()) {
            if ((*i) == listener) {
                m_changelisteners.erase(i);
                return;
            }
            ++i;
        }
    }
} // namespace FIFE
