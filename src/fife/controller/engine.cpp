// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "engine.h"

// Standard C++ library includes
#include <algorithm>
#include <array>
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
#include "debug/diagnosticsmanager.h"
#include "debug/diagnosticsphases.h"
#include "debug/renderdoc/renderdocdebugger.h"
#include "debug/tracy/tracygpuprofiler.h"
#include "debug/tracy/tracyprofilermacros.h"
#include "eventchannel/eventmanager.h"
#include "eventchannel/key/key.h"
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
#include "video/dummy/renderbackendnull.h"
#include "video/dummy/renderbackendsurface.h"
#include "video/fonts/fontmanager.h"
#include "video/imagemanager.h"
#include "video/window/guiscale.h"
#ifdef HAVE_MOVIE
    #include "audio/soundclip.h"
    #include "audio/soundemitter.h"
    #include "video/movie/movieplayer.h"
#endif
#include "video/renderbackend.h"
#include "video/window/window.h"
#include "video/window/windowsize.h"
#ifdef HAVE_OPENGL
    #include "video/opengl/fife_opengl.h"
    #include "video/opengl/renderbackendopengl.h"
#endif
#include "loaders/native/audio/ogg_loader.h"
#include "loaders/native/video/imageloader.h"
#include "model/metamodel/grids/hexgrid.h"
#include "model/metamodel/grids/squaregrid.h"
#include "model/model.h"
#include "model/structures/particlemanager.h"
#include "pathfinder/routepather/routepather.h"
#include "version.hpp"
#include "video/glcore/glcore_names.h"
#include "video/glcore/renderbackendgl.h"
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
#include "view/renderers/particlerenderer.h"
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
        [[maybe_unused]] Logger& _log()
        {
            static Logger log(LM_CONTROLLER);
            return log;
        }

        /** Maps a configured hotkey name onto FIFE's SDL-derived key enum. */
        Keys::KeyType renderDocHotkey(std::string const & name)
        {
            static std::array<std::pair<char const *, Keys::KeyType>, 15> const keys{{
                {"F1", Keys::F1},
                {"F2", Keys::F2},
                {"F3", Keys::F3},
                {"F4", Keys::F4},
                {"F5", Keys::F5},
                {"F6", Keys::F6},
                {"F7", Keys::F7},
                {"F8", Keys::F8},
                {"F9", Keys::F9},
                {"F10", Keys::F10},
                {"F11", Keys::F11},
                {"F12", Keys::F12},
                {"P", Keys::P},
                {"C", Keys::C},
                {"V", Keys::V},
            }};
            auto const it = std::ranges::find_if(keys, [&name](auto const & entry) {
                return name == entry.first;
            });
            if (it != keys.end()) {
                return it->second;
            }
            static bool warnedOnce = false;
            if (!warnedOnce) {
                warnedOnce = true;
                FL_WARN(_log(), std::format("Unknown RenderDoc capture hotkey '{}'; using F12", name));
            }
            return Keys::F12;
        }

        std::string joinNames(std::vector<std::string> const & names)
        {
            std::string result;
            for (auto const & name : names) {
                if (!result.empty()) {
                    result += ", ";
                }
                result += name;
            }
            return result.empty() ? "(none)" : result;
        }

        char const * onOff(bool value)
        {
            return value ? "on" : "off";
        }

        char const * filterName(TextureFiltering filter)
        {
            switch (filter) {
            case TEXTURE_FILTER_NONE:
                return "none";
            case TEXTURE_FILTER_BILINEAR:
                return "bilinear";
            case TEXTURE_FILTER_TRILINEAR:
                return "trilinear";
            case TEXTURE_FILTER_ANISOTROPIC:
                return "anisotropic";
            }
            return "unknown";
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
            applyDefaultFont(fifechan);
        }
        m_guimanager = std::move(guimanager);
        // GUI managers such as pychan are installed after Engine.init(), so
        // the initial scale has to be applied here as well.
        applyGUIScale();
    }

    void Engine::updateFontFallback()
    {
        if (!m_fontManager) {
            return;
        }

        std::string fallback = m_settings.getDefaultFontId();
        if (fallback.empty() || !m_fontManager->hasFamily(fallback)) {
            // Fall back to any registered family so font lookups never fail hard.
            fallback = m_fontManager->getFirstFamilyId().value_or("");
        }
        if (!fallback.empty()) {
            m_fontManager->setFallbackFont(fallback);
        }
    }

    void Engine::applyDefaultFont(FifechanManager* fifechan)
    {
        if ((fifechan == nullptr) || !m_fontManager) {
            return;
        }

        std::string family = m_settings.getDefaultFontId();
        if (family.empty() || !m_fontManager->hasFamily(family)) {
            family = m_fontManager->getFirstFamilyId().value_or("");
        }
        if (family.empty()) {
            FL_LOG(_log(), "No font families registered; GUI default font not set");
            return;
        }

        try {
            fifechan->setDefaultFont(family, m_settings.getDefaultFontSize());
            FL_LOG(_log(), std::format("GUI default font set to '{}'", family));
        } catch (std::exception const & e) {
            FL_WARN(_log(), std::format("Failed to set GUI default font '{}': {}", family, e.what()));
        }
    }

    void Engine::applyGUIScale()
    {
        if (!m_guimanager) {
            return;
        }

        float displayScale = 1.0F;
        float dpiScale     = 1.0F;
        if (m_window) {
            displayScale = m_window->getDisplayScale();
            dpiScale     = m_window->getDPIScaleFactor();
        }

        float const guiScale = calculateGUIScale(displayScale, dpiScale, m_settings.isGUIScalingEnabled());
        m_guimanager->setDPIScale(guiScale);
    }

    void Engine::loadFontManifestFromString(std::string const & xml)
    {
        if (m_fontManager) {
            m_fontManager->loadManifestFromString(xml);
            updateFontFallback();
            if (auto* fifechan = dynamic_cast<FifechanManager*>(m_guimanager.get())) {
                applyDefaultFont(fifechan);
            }
        }
    }

    void Engine::toggleFullscreen()
    {
        if (m_window == nullptr) {
            return;
        }
        // Switch the existing window instead of re-creating it: destroying the
        // SDL window also destroys the renderer (SDL3) and the GL context the
        // render backend holds.
        WindowMode const nextMode =
            isFullscreenMode(m_window->getFullscreenMode()) ? WindowMode::Windowed : WindowMode::Fullscreen;
        m_window->setFullscreenMode(nextMode);
        syncSettingsWithWindow();
        for (auto* listener : m_changelisteners) {
            listener->onWindowModeChanged(m_window->getSettings());
        }
    }

    void Engine::toggleVSync()
    {
        if (m_window == nullptr) {
            return;
        }
        bool const current = m_window->getVSync();
        m_window->setVSync(!current);
    }

    bool Engine::windowSettingsRequireRestart(WindowSettings const & settings) const
    {
        if (m_window == nullptr) {
            // Before init the settings are applied when the window is created.
            return false;
        }
        return FIFE::windowSettingsRequireRestart(m_window->getSettings(), settings);
    }

    void Engine::changeWindowMode(WindowSettings const & settings)
    {
        if (m_window == nullptr) {
            return;
        }

        // Apply the settings to the live window. Re-creating the SDL window here
        // would invalidate the render backend (SDL_DestroyWindow also destroys
        // the attached renderer, and the GL context belongs to the old window).
        if (windowSettingsRequireRestart(settings)) {
            WindowSettings const current = m_window->getSettings();
            FL_WARN(
                _log(),
                std::format(
                    "changeWindowMode(): a different monitor or renderer cannot be applied to the live "
                    "window (display {} -> {}, OpenGL {} -> {}); a restart is required. Keeping the current "
                    "display/renderer and applying the remaining settings.",
                    current.display,
                    settings.display,
                    current.opengl,
                    settings.opengl));
        }

        // Switch the mode first: leaving fullscreen restores the remembered
        // windowed geometry, which requested size/position must then override.
        m_window->setResizable(settings.resizable);
        m_window->setScalingMode(settings.scalingMode);
        m_window->setVSync(settings.vsync);
        m_window->setWindowTitle(settings.title);
        m_window->setWindowIcon(settings.icon);
        m_window->setFullscreenMode(settings.windowMode);

        if (!isFullscreenMode(settings.windowMode)) {
            m_window->setSize(settings.width, settings.height);
            if (settings.x != -1 && settings.y != -1) {
                m_window->setPosition(settings.x, settings.y);
            }
        }

        m_settings.setVSync(settings.vsync);
        m_settings.setWindowResizable(settings.resizable);
        m_settings.setWindowScalingMode(static_cast<uint8_t>(settings.scalingMode));
        syncSettingsWithWindow();

        if (m_guimanager != nullptr) {
            m_guimanager->resizeTopContainer(
                0U,
                0U,
                static_cast<uint32_t>(m_window->getWidthInPoints()),
                static_cast<uint32_t>(m_window->getHeightInPoints()));
        }
        for (auto* listener : m_changelisteners) {
            listener->onWindowModeChanged(m_window->getSettings());
        }
    }

    void Engine::syncSettingsWithWindow()
    {
        if (m_window == nullptr) {
            return;
        }
        int const width  = m_window->getWidthInPoints();
        int const height = m_window->getHeightInPoints();
        if (width > 0 && height > 0) {
            m_settings.setEffectiveScreenWidth(static_cast<uint16_t>(std::clamp(width, 1, 65535)));
            m_settings.setEffectiveScreenHeight(static_cast<uint16_t>(std::clamp(height, 1, 65535)));
        }
        m_settings.setFullScreen(isFullscreenMode(m_window->getFullscreenMode()));

        // Publish the remaining fields of the window that actually exists:
        // init() may have fallen back to display 0, and the remembered
        // windowed position changes when leaving fullscreen.
        WindowSettings const & windowSettings = m_window->getSettings();
        m_settings.setDisplay(static_cast<uint8_t>(std::clamp(windowSettings.display, 0, 255)));
        m_settings.setWindowPositionX(windowSettings.x);
        m_settings.setWindowPositionY(windowSettings.y);
    }

    void Engine::init()
    {
        m_destroyed = false;

        FL_LOG(_log(), std::format("Fifengine v{}", FIFE::app_version::get_version()));
        FL_LOG(_log(), "================== Engine initialize start =================");
        m_timemanager = std::make_unique<TimeManager>();
        FL_LOG(_log(), "Time manager created");

        m_particlemanager = std::make_unique<ParticleManager>();
        FL_LOG(_log(), "Particle manager created");

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

        updateFontFallback();

        // If a GUI manager was installed before init(), wire it up now that the
        // FontManager exists.
        if (auto* fifechan = dynamic_cast<FifechanManager*>(m_guimanager.get())) {
            fifechan->setFontManager(m_fontManager.get());
            applyDefaultFont(fifechan);
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
        bool const headless = (rbackend == "DUMMY_NULL") || (rbackend == "DUMMY_SURFACE");
        m_headless          = headless;
        if (headless) {
            uint32_t const width  = static_cast<uint32_t>(m_settings.getScreenWidth());
            uint32_t const height = static_cast<uint32_t>(m_settings.getScreenHeight());
            if (rbackend == "DUMMY_NULL") {
                m_renderbackend = std::make_unique<RenderBackendNull>(m_settings.getColorKey(), width, height);
                FL_LOG(_log(), "Null render backend created");
            } else {
                m_renderbackend = std::make_unique<RenderBackendSurface>(m_settings.getColorKey(), width, height);
                FL_LOG(_log(), "Surface render backend created");
            }
        } else if (rbackend == "SDL") {
            m_renderbackend = std::make_unique<RenderBackendSDL>(m_settings.getColorKey());
            FL_LOG(_log(), "SDL Render backend created");
        } else if (rbackend == "OpenGL_CORE") {
#ifdef HAVE_OPENGL_CORE
            m_renderbackend = std::make_unique<RenderBackendGLCore>(m_settings.getColorKey());
            FL_LOG(_log(), "OpenGL core render backend created");
#else
            m_renderbackend = std::make_unique<RenderBackendSDL>(m_settings.getColorKey());
            FL_WARN(_log(), "OpenGL_CORE is not compiled in; falling back to SDL");
#endif
        } else if (rbackend == "GLES2") {
#ifdef HAVE_OPENGL_ES2
            m_renderbackend = std::make_unique<RenderBackendGLES2>(m_settings.getColorKey());
            FL_LOG(_log(), "OpenGL ES 2.0 render backend created");
#else
            m_renderbackend = std::make_unique<RenderBackendSDL>(m_settings.getColorKey());
            FL_WARN(_log(), "GLES2 is not compiled in; falling back to SDL");
#endif
        } else if (rbackend == "OpenGL_43") {
#ifdef HAVE_OPENGL_43
            m_renderbackend = std::make_unique<RenderBackendGL43>(m_settings.getColorKey());
            FL_LOG(_log(), "OpenGL 4.3 core render backend created");
#else
            m_renderbackend = std::make_unique<RenderBackendSDL>(m_settings.getColorKey());
            FL_WARN(_log(), "OpenGL_43 is not compiled in; falling back to SDL");
#endif
        } else if (rbackend == "GLES_31") {
#ifdef HAVE_OPENGLES_31
            m_renderbackend = std::make_unique<RenderBackendGLES31>(m_settings.getColorKey());
            FL_LOG(_log(), "OpenGL ES 3.1 render backend created");
#else
            m_renderbackend = std::make_unique<RenderBackendSDL>(m_settings.getColorKey());
            FL_WARN(_log(), "GLES_31 is not compiled in; falling back to SDL");
#endif
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
        FL_LOG(_log(), std::format("Requested render backend: {}", m_settings.getRenderBackend()));
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
        FL_LOG(
            _log(),
            std::format(
                "Backend settings: filter={} mipmap={} NPOT={} compression={} depth={} lighting={} alphaTest={:.2f} "
                "colorKey={} vsync={} frameLimit={}",
                filterName(m_renderbackend->getTextureFiltering()),
                onOff(m_renderbackend->isMipmappingEnabled()),
                onOff(m_renderbackend->isNPOTEnabled()),
                onOff(m_renderbackend->isImageCompressingEnabled()),
                onOff(m_renderbackend->isDepthBufferEnabled()),
                m_renderbackend->getLightingModel(),
                m_renderbackend->getAlphaTestValue(),
                onOff(m_renderbackend->isColorKeyEnabled()),
                onOff(m_renderbackend->isVSyncEnabled()),
                m_renderbackend->isFrameLimitEnabled() ? std::format("{} fps", m_renderbackend->getFrameLimit()) :
                                                         std::string("off")));

        // Populate the driver list before validating the setting:
        // fillDeviceCaps() uses SDL3's static driver tables and is safe to
        // call before SDL_Init.
        m_devcaps.fillDeviceCaps();

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
#ifdef FIFE_OS_WINDOWS
        // DPI awareness is process-wide. SDL3 defaults to PerMonitorV2 when
        // the video subsystem initializes, which covers hosts without a
        // DPI-aware manifest (for example the stock python.exe). State the
        // intent explicitly, but let the application override it and let a
        // host .exe manifest win.
        if (SDL_GetHint("SDL_WINDOWS_DPI_AWARENESS") == nullptr) {
            SDL_SetHint("SDL_WINDOWS_DPI_AWARENESS", "permonitorv2");
        }
#endif

        // FIFE translates touch input itself
        // SDL hints must be set before the video subsystem initializes
        EventManager::applyInputHints();

        FL_LOG(_log(), std::format("Requested video driver: '{}'", m_settings.getVideoDriver()));
        FL_LOG(_log(), std::format("Available video drivers: {}", joinNames(m_devcaps.getAvailableVideoDrivers())));
        // init backend with selected video driver or default
        m_renderbackend->init(driver);
        char const * const activeVideoDriver = SDL_GetCurrentVideoDriver();
        FL_LOG(
            _log(),
            std::format(
                "Effective video driver: '{}'", activeVideoDriver != nullptr ? activeVideoDriver : "SDL default"));

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

        FL_LOG(_log(), std::format("Requested SDL render driver: '{}'", m_settings.getSDLDriver()));
        FL_LOG(
            _log(), std::format("Available SDL render drivers: {}", joinNames(m_devcaps.getAvailableRenderDrivers())));

        FL_LOG(_log(), "Querying device capabilities");
        m_devcaps.fillDeviceCaps();
        if (m_devcaps.getDisplayCount() > 0) {
            FL_LOG(
                _log(),
                std::format(
                    "Displays: {}; primary '{}' {}x{} @ {:.1f} Hz, format 0x{:x}",
                    m_devcaps.getDisplayCount(),
                    m_devcaps.getDisplayName(0),
                    m_devcaps.getDesktopWidth(0),
                    m_devcaps.getDesktopHeight(0),
                    static_cast<double>(m_devcaps.getDesktopRefreshRate(0)),
                    m_devcaps.getDesktopFormat(0)));
        }

        if (headless) {
            // Dummy backends never touch SDL video or create a Window. Events
            // are still needed so applications can pump the engine normally.
            if (!m_eventsInitialized) {
                if (!SDL_InitSubSystem(SDL_INIT_EVENTS)) {
                    throw NotSupported(std::format("Could not initialize SDL events subsystem: {}", SDL_GetError()));
                }
                m_eventsInitialized = true;
                FL_LOG(_log(), "SDL events subsystem initialized for headless backend");
            }

            FL_LOG(_log(), "Creating main screen");
            m_renderbackend->createMainScreen(m_settings.getWindowTitle(), m_settings.getWindowIcon());
            FL_LOG(_log(), "Main screen created");
            FL_LOG(_log(), std::format("Active render backend: {}", m_renderbackend->getName()));
            FL_LOG(_log(), std::format("Renderer: {}", m_renderbackend->getRenderDescription()));
            applyGUIScale();
        } else {
            // Keep the existing display/window code untouched.
            // clang-format off
            {
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

        Rect displayBounds = m_devcaps.getDisplayBounds(selectedDisplay);
        if (!m_settings.isFullScreen()) {
            // A windowed window lives inside the desktop work area. Use the
            // usable bounds so it is not clamped underneath a taskbar or dock.
            // Fullscreen keeps the full display bounds.
            SDL_Rect const usableBounds = Display::getUsableBounds(selectedDisplay);
            if (usableBounds.w > 0 && usableBounds.h > 0) {
                displayBounds.w = usableBounds.w;
                displayBounds.h = usableBounds.h;
            }
        }

        // SDL_GetDisplayBounds()/SDL_GetDisplayUsableBounds() and window sizes
        // share the same coordinate space on every platform (pixels on
        // Windows, points on macOS), so no DPI/content-scale conversion is
        // needed here. See unknown-horizons#2923.
        WindowSize const clamped =
            clampWindowSizeToDesktop(requestedWidth, requestedHeight, displayBounds.w, displayBounds.h);
        selectedWidth  = static_cast<uint16_t>(std::clamp(clamped.width, 1, 65535));
        selectedHeight = static_cast<uint16_t>(std::clamp(clamped.height, 1, 65535));
        if (selectedWidth != requestedWidth || selectedHeight != requestedHeight) {

            FL_WARN(
                _log(),
                std::format(
                    "Requested resolution {}x{} does not fit the selected display; clamping to {}x{}.",
                    requestedWidth,
                    requestedHeight,
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

        // EngineSettings only carries a fullscreen flag, so startup uses the
        // legacy desktop-fullscreen mode. Borderless and exclusive fullscreen
        // are selected through WindowSettings/changeWindowMode once the window
        // exists.
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
        winSettings.opengl      = isGlDepthBackend(rbackend);
        winSettings.windowMode  = m_settings.isFullScreen() ? WindowMode::Fullscreen : WindowMode::Windowed;
        winSettings.scalingMode = static_cast<ScalingMode>(m_settings.getWindowScalingMode());

        m_window                        = std::make_unique<Window>();
        m_window->signalWindowDestroyed = [this]() {
            if (m_imagemanager) {
                m_imagemanager->invalidateAll();
            }
        };
        m_window->create(winSettings);
        // SDL may have resized the window (minimum size, display constraints,
        // DPI scaling). Publish the real values through EngineSettings so
        // applications do not keep using the requested ones.
        syncSettingsWithWindow();
        m_window->signalResized = [this](int w, int h) {
            if (w > 0 && h > 0) {
                m_settings.setEffectiveScreenWidth(static_cast<uint16_t>(std::clamp(w, 1, 65535)));
                m_settings.setEffectiveScreenHeight(static_cast<uint16_t>(std::clamp(h, 1, 65535)));
            }
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
            applyGUIScale();
            if (m_guimanager) {
                int const w = m_window->getWidthInPoints();
                int const h = m_window->getHeightInPoints();
                m_guimanager->resizeTopContainer(0U, 0U, static_cast<uint32_t>(w), static_cast<uint32_t>(h));
            }
            for (auto* listener : m_changelisteners) {
                listener->onDPIScaleChanged(scale);
            }
        };
        m_window->signalDisplayScaleChanged = [this](float scale) {
            // The pixel/point ratio can stay at 1.0 while the OS content scale
            // changes (Windows monitors at different DPI percentages). Recompute
            // the GUI scale and let applications adjust their own layouts.
            applyGUIScale();
            if (m_guimanager) {
                int const w = m_window->getWidthInPoints();
                int const h = m_window->getHeightInPoints();
                m_guimanager->resizeTopContainer(0U, 0U, static_cast<uint32_t>(w), static_cast<uint32_t>(h));
            }
            for (auto* listener : m_changelisteners) {
                listener->onDisplayScaleChanged(scale);
            }
        };
        m_renderbackend->setWindowObject(m_window.get());

        FL_LOG(_log(), "Creating main screen");
        m_renderbackend->createMainScreen(m_settings.getWindowTitle(), m_settings.getWindowIcon());
        FL_LOG(_log(), "Main screen created");
        FL_LOG(_log(), std::format("Active render backend: {}", m_renderbackend->getName()));
        FL_LOG(_log(), std::format("Renderer: {}", m_renderbackend->getRenderDescription()));
        m_window->show();
        // Showing a window can apply a pending fullscreen state; publish the
        // final size/state so startup settings match the window.
        syncSettingsWithWindow();

        // At this point the final display scale is known (the window may have
        // moved to another monitor while entering fullscreen).
        applyGUIScale();

        // One line with the values needed in DPI/fullscreen bug reports. The
        // display scale is the OS content scale (1.5 = 150% on Windows); the
        // pixel/point ratio is what the renderer/fonts already apply.
        float const guiScale = calculateGUIScale(
            m_window->getDisplayScale(), m_window->getDPIScaleFactor(), m_settings.isGUIScalingEnabled());
        FL_LOG(
            _log(),
            std::format(
                "Window geometry: {}x{} points ({}x{} pixels), display scale {:.2f}, pixel/point ratio {:.2f}, "
                "GUI scale {:.2f}",
                m_window->getWidthInPoints(),
                m_window->getHeightInPoints(),
                m_window->getWidthInPixels(),
                m_window->getHeightInPixels(),
                m_window->getDisplayScale(),
                m_window->getDPIScaleFactor(),
                guiScale));

        if (SDL_Window* win = m_window->getSDLWindow()) {
            SDL_StartTextInput(win);
        }
            }
        }
        // clang-format on

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
        m_renderers.push_back(std::make_unique<ParticleRenderer>(m_renderbackend.get(), 15));
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
        if (m_window) {
            m_window->onClosed([this]() {
                if (m_cursor != nullptr) {
                    m_cursor->invalidate();
                }
            });
        }
        // Diagnostics are optional: the manager is created after the GL
        // context exists and keeps RenderDoc and Tracy completely independent.
        m_diagnostics = std::make_unique<DiagnosticsManager>();
        DiagnosticsConfig diagnosticsConfig;
        auto& rdocConfig                              = diagnosticsConfig.renderDoc;
        rdocConfig.enableRenderDoc                    = m_settings.isRenderDocEnabled();
        rdocConfig.autoCapture                        = m_settings.isRenderDocAutoCapture();
        rdocConfig.captureOnHotkey                    = m_settings.isRenderDocHotkeyCapture();
        rdocConfig.captureHotkey                      = m_settings.getRenderDocCaptureHotkey();
        rdocConfig.captureDirectory                   = m_settings.getRenderDocCaptureDirectory();
        rdocConfig.enableDebugMarkers                 = m_settings.isRenderDocDebugMarkers();
        rdocConfig.enableResourceNaming               = m_settings.isRenderDocResourceNaming();
        rdocConfig.logCaptureEvents                   = m_settings.isRenderDocLogCaptureEvents();
        diagnosticsConfig.tracy.enableTracy           = m_settings.isTracyEnabled();
        diagnosticsConfig.tracy.enableGPUProfiling    = m_settings.isTracyGPUProfiling();
        diagnosticsConfig.tracy.enableMemoryProfiling = m_settings.isTracyMemoryProfiling();
        bool const hasGLContext                       = isGlDepthBackend(rbackend) && m_renderbackend != nullptr;
        SDL_Window* const window                      = (m_window != nullptr) ? m_window->getSDLWindow() : nullptr;
        m_diagnostics->Initialize(diagnosticsConfig, hasGLContext, window);
        m_diagnostics->OnGLContextCurrent();
        m_lastRenderDocHotkeyState = false;

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
        if (m_destroyed) {
            return;
        }

        FL_LOG(_log(), "Destructing engine");

        // Members (including the image manager) are torn down after this
        // function; do not run the window-destroyed hook against them.
        if (m_window != nullptr) {
            m_window->signalWindowDestroyed = nullptr;
        }

        // Font adapters keep a callback into the FontManager. Detach it before
        // the manager is released so a late scale change (for example from a
        // window signal during SDL_Quit) cannot dereference it.
        if (auto* fifechan = dynamic_cast<FifechanManager*>(m_guimanager.get())) {
            fifechan->setFontManager(nullptr);
        }

#ifdef HAVE_MOVIE
        // Stop playback while the sound manager is still alive.
        if (m_movieplayer != nullptr) {
            m_movieplayer->unload();
        }
        m_movieplayer.reset();
        m_movieemitter = nullptr;
#endif

        // Emitters own ImagePtr frames whose backend textures must be released
        // while the render backend and window still exist. TimeManager is also
        // still alive here, so the particle TimeEvent can unregister safely.
        m_particlemanager.reset();

        // Release owners before the services they borrow from. Models hold
        // raw pointers to renderers, and renderers/gui/cursor hold raw pointers
        // to the render backend, so they must go first.
        m_model.reset();
        m_cursor.reset();
        m_guimanager.reset();

        m_offrenderer.reset();
        m_targetrenderer.reset();
        m_renderers.clear();

        // Release the debugger while the GL context and window still exist.
        m_diagnostics.reset();

        // These managers derive from DynamicSingleton. Releasing them here (and
        // not only in ~Engine) is what makes destroy()/init() cycles and two
        // Engine lifetimes in one process safe.
        m_animationmanager.reset();
        m_imagemanager.reset();
        m_soundclipmanager.reset();
        m_soundmanager.reset();
        m_eventmanager.reset();

        // Fonts must be released before TTF_Quit, otherwise the FreeType faces
        // they own are destroyed against an already shut down library. The
        // FontManager's resolver borrows the VFS, so release it before m_vfs.
        m_fontManager.reset();

        m_window.reset();
        m_renderbackend.reset();

        m_vfs.reset();
        m_timemanager.reset();

        // SDL subsystem initialization is ref-counted; balance only the
        // SDL_InitSubSystem() call made for a headless Engine::init().
        if (m_eventsInitialized) {
            SDL_QuitSubSystem(SDL_INIT_EVENTS);
            m_eventsInitialized = false;
        }

        TTF_Quit();
        // A headless Engine owns no SDL video subsystem; SDL_Quit() would shut
        // down SDL for the rest of the process (other engines, tests, hosts).
        if (!m_headless) {
            SDL_Quit();
        }
        // Clear the flag after the teardown decision above: a stale value must
        // not affect a later init/destroy cycle, but this destroy still has to
        // honour whether SDL was actually owned here.
        m_headless = false;

#ifdef USE_COCOA
        objc_msgSend(m_autoreleasePool, sel_registerName("release"));
#endif

        FL_LOG(_log(), "================== Engine destructed ==================");
        m_destroyed = true;
    }
    RenderDocDebugger* Engine::getRenderDocDebugger() const
    {
        return (m_diagnostics != nullptr) ? m_diagnostics->GetGraphicsDebugger() : nullptr;
    }

    void Engine::initializePumping()
    {
        m_eventmanager->processEvents();
    }

    void Engine::pump()
    {
        FIFE_PROFILE_ZONE_N(FIFE::kPhaseFrame);
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

        bool rdocHotkeyDown               = false;
        RenderDocDebugger* const debugger = m_diagnostics->GetGraphicsDebugger();
        if (debugger != nullptr && debugger->isEnabled() && debugger->isHotkeyCaptureEnabled() &&
            m_eventmanager != nullptr) {
            rdocHotkeyDown = m_eventmanager->isKeyPressed(renderDocHotkey(debugger->getCaptureHotkey()));
            if (RenderDocDebugger::isHotkeyEdge(rdocHotkeyDown, m_lastRenderDocHotkeyState)) {
                debugger->triggerCapture();
            }
        }
        m_lastRenderDocHotkeyState = rdocHotkeyDown;

        m_diagnostics->OnFrameBegin();
        {
            FIFE_PROFILE_ZONE_N(FIFE::kPhaseUpdate);
            m_renderbackend->startFrame();
            {
                FIFE_PROFILE_ZONE_N(FIFE::kPhaseInput);
                m_eventmanager->processEvents();
            }
            if (m_window != nullptr) {
                m_window->updateDPIScaleIfNeeded();
            }
            m_timemanager->update();
            m_soundmanager->update();
        }

        {
            FIFE_PROFILE_ZONE_N(FIFE::kPhaseTarget);
            FIFE::TracyGPUPhase const gpuPhase(*m_diagnostics->GetGPUProfiler(), FIFE::kPhaseTarget);
            m_diagnostics->BeginPhase(FIFE::kPhaseTarget);
            m_targetrenderer->render();
            m_diagnostics->EndPhase();
        }
#ifdef HAVE_MOVIE
        if (m_movieplayer != nullptr && m_movieplayer->isLoaded()) {
            if (m_moviestart == 0) {
                // TimeManager::now64() only becomes meaningful after its first
                // update(), which runs above, so the origin is captured here rather
                // than in playMovie() where the clock may still be zero.
                m_moviestart = m_timemanager->now64();
            }
            // Follow the audio when it is playing, so picture and sound stay locked.
            double movieTime = static_cast<double>(m_timemanager->now64() - m_moviestart) / 1000.0;
            if (m_movieemitter != nullptr) {
                if (m_movieemitter->isActive()) {
                    movieTime = static_cast<double>(m_movieemitter->getCursor(SD_TIME_POS));
                } else if (m_movieemitter->isFinished()) {
                    // Audio ran out; let the picture finish its last frame.
                    movieTime = m_movieplayer->getDuration();
                }
            }
            m_movieplayer->update(movieTime);
            {
                FIFE_PROFILE_ZONE_N(FIFE::kPhaseMovie);
                FIFE::TracyGPUPhase const gpuPhase(*m_diagnostics->GetGPUProfiler(), FIFE::kPhaseMovie);
                m_diagnostics->BeginPhase(FIFE::kPhaseMovie);
                renderMovie();
                m_diagnostics->EndPhase();
            }
            if (m_movieplayer->isFinished()) {
                stopMovie();
            }
        } else {
#endif
            if (m_model->getActiveCameraCount() == 0) {
                m_renderbackend->clearBackBuffer();
                {
                    FIFE_PROFILE_ZONE_N(FIFE::kPhaseOffscreen);
                    FIFE::TracyGPUPhase const gpuPhase(*m_diagnostics->GetGPUProfiler(), FIFE::kPhaseOffscreen);
                    m_diagnostics->BeginPhase(FIFE::kPhaseOffscreen);
                    m_offrenderer->render();
                    m_diagnostics->EndPhase();
                }
            } else {
                {
                    FIFE_PROFILE_ZONE_N(FIFE::kPhaseWorld);
                    FIFE::TracyGPUPhase const gpuPhase(*m_diagnostics->GetGPUProfiler(), FIFE::kPhaseWorld);
                    m_diagnostics->BeginPhase(FIFE::kPhaseWorld);
                    m_model->update();
                    m_diagnostics->EndPhase();
                }
            }
#ifdef HAVE_MOVIE
        }
#endif

        if (m_guimanager != nullptr) {
            FIFE_PROFILE_ZONE_N(FIFE::kPhaseGui);
            FIFE::TracyGPUPhase const gpuPhase(*m_diagnostics->GetGPUProfiler(), FIFE::kPhaseGui);
            m_diagnostics->BeginPhase(FIFE::kPhaseGui);
            m_guimanager->turn();
            m_diagnostics->EndPhase();
        }

        {
            FIFE_PROFILE_ZONE_N(FIFE::kPhaseCursor);
            FIFE::TracyGPUPhase const gpuPhase(*m_diagnostics->GetGPUProfiler(), FIFE::kPhaseCursor);
            m_diagnostics->BeginPhase(FIFE::kPhaseCursor);
            m_cursor->draw();
            m_diagnostics->EndPhase();
        }
        {
            FIFE_PROFILE_ZONE_N(FIFE::kPhasePresent);
            m_renderbackend->endFrame();
        }
        m_diagnostics->OnFrameEnd();
    }

#ifdef HAVE_MOVIE
    void Engine::playMovie(std::string const & path, bool looping)
    {
        if (m_movieplayer == nullptr) {
            m_movieplayer = std::make_unique<MoviePlayer>();
        }
        stopMovieSound();

        m_movieplayer->load(path);
        m_movieplayer->setLooping(looping);
        m_moviestart = 0;

        startMovieSound();
    }

    bool Engine::isMoviePlaying() const
    {
        return m_movieplayer != nullptr && m_movieplayer->isLoaded() && !m_movieplayer->isFinished();
    }

    double Engine::getMovieTime() const
    {
        if (m_movieplayer == nullptr || !m_movieplayer->isLoaded()) {
            return -1.0;
        }
        return m_movieplayer->getFrameTimestamp();
    }

    void Engine::stopMovie()
    {
        if (m_movieplayer != nullptr) {
            m_movieplayer->unload();
        }
        stopMovieSound();
    }

    void Engine::startMovieSound()
    {
        if (m_movieplayer == nullptr || !m_movieplayer->hasAudio() || m_soundmanager == nullptr) {
            return;
        }

        SoundClipPtr clip = m_movieplayer->getSoundClip();
        if (!clip) {
            return;
        }

        m_movieemitter = m_soundmanager->createEmitter();
        if (m_movieemitter == nullptr) {
            return;
        }
        // Own group so a global stopAll does not silently cut the movie short.
        m_movieemitter->setGroup("movie");
        m_movieemitter->setSoundClip(clip);
        m_movieemitter->setLooping(false);
        m_movieemitter->play();
    }

    void Engine::stopMovieSound()
    {
        if (m_movieemitter == nullptr || m_soundmanager == nullptr) {
            return;
        }
        uint32_t const id = m_movieemitter->getId();
        m_movieemitter->stop();
        m_soundmanager->removeFromGroup(m_movieemitter);
        m_soundmanager->releaseEmitter(id);
        m_movieemitter = nullptr;
    }

    void Engine::renderMovie()
    {
        ImagePtr const frame = m_movieplayer->getFrameImage();
        if (!frame) {
            return;
        }

        m_renderbackend->clearBackBuffer();

        uint32_t const width  = m_renderbackend->getWidth();
        uint32_t const height = m_renderbackend->getHeight();
        uint32_t const fw     = m_movieplayer->getWidth();
        uint32_t const fh     = m_movieplayer->getHeight();
        if (fw == 0U || fh == 0U) {
            return;
        }

        // Letterbox so the aspect ratio is kept.
        double const scale = std::min(
            static_cast<double>(width) / static_cast<double>(fw),
            static_cast<double>(height) / static_cast<double>(fh));
        int32_t const dw = static_cast<int32_t>(static_cast<double>(fw) * scale);
        int32_t const dh = static_cast<int32_t>(static_cast<double>(fh) * scale);
        int32_t const dx = (static_cast<int32_t>(width) - dw) / 2;
        int32_t const dy = (static_cast<int32_t>(height) - dh) / 2;

        frame->render(Rect(dx, dy, dw, dh));
        m_renderbackend->renderVertexArrays();
    }
#endif

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
