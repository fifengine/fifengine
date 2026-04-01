// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <string>
#include <vector>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/log/logger.h"

#include "enginesettings.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_CONTROLLER);

    constexpr float MAXIMUM_VOLUME = 10.0F;

    EngineSettings::EngineSettings() :
        m_bitsperpixel(0),
        m_fullscreen(false),
        m_refreshRate(60),
        m_displayIndex(0),
        m_windowX(-1),
        m_windowY(-1),
        m_vSync(false),

        m_initialvolume(MAXIMUM_VOLUME / 2.0F),
        m_renderbackend("SDL"),
        m_sdlremovefakealpha(false),
        m_oglcompressimages(false),
        m_ogluseframebuffer(true),
        m_oglusenpot(true),
        m_oglMipmapping(false),
        m_oglMonochrome(false),
        m_oglTextureFilter(TEXTURE_FILTER_NONE),
        m_oglDepthBuffer(false),
        m_alphaTestValue(0.3F),
        m_screenwidth(800),
        m_screenheight(600),
        m_windowtitle("FIFE"),

        m_defaultfontpath("fonts/FreeSans.ttf"),
        m_defaultfontsize(8),
        m_defaultfontglyphs("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/():;%&`'*#=[]\\\""),
        m_iscolorkeyenabled(false),
        m_colorkey{255, 0, 255},
        m_videodriver(
#if defined(__unix__)
            "x11"
#elif defined(WIN32)
            "windib"
#elif defined(__APPLE_CC__)
            "x11"
#else
            ""
#endif
            ),
        m_lighting(0),
        m_isframelimit(false),
        m_framelimit(60),
        m_mousesensitivity(0.0F),
        m_mouseacceleration(false),
        m_nativeimagecursor(false),
        m_joystickSupport(false)
    {
    }

    EngineSettings::~EngineSettings() = default;

    void EngineSettings::setBitsPerPixel(uint8_t bitsperpixel)
    {
        std::vector<uint8_t> pv = getPossibleBitsPerPixel();
        auto i                  = std::ranges::find(pv, bitsperpixel);
        if (i != pv.end()) {
            m_bitsperpixel = bitsperpixel;
            return;
        }

        FL_WARN(
            _log,
            LMsg("EngineSettings::setBitsPerPixel() - ")
                << " Tried to set screen bpp to an unsupporded value of " << bitsperpixel
                << ".  Setting bpp to use the default value of 0 (the current screen bpp)");

        m_bitsperpixel = 0; // default value
    }

    std::vector<uint8_t> EngineSettings::getPossibleBitsPerPixel() const
    {
        std::vector<uint8_t> tmp;
        tmp.push_back(0);
        tmp.push_back(16);
        tmp.push_back(24);
        tmp.push_back(32);
        return tmp;
    }

    void EngineSettings::setInitialVolume(float volume)
    {
        if (volume > getMaxVolume() || volume < 0) {
            FL_WARN(
                _log,
                LMsg("EngineSettings::setInitialVolume() - ")
                    << " Tried to set initial volume to an unsupporded value of " << volume
                    << ".  Setting volume to the default value of 5 (minumum is 0, maximum is 10)");

            m_initialvolume = 5.0F;
            return;
        }

        m_initialvolume = volume;
    }

    float EngineSettings::getMaxVolume() const
    {
        return MAXIMUM_VOLUME;
    }

    void EngineSettings::setRenderBackend(const std::string& renderbackend)
    {
        std::vector<std::string> pv = getPossibleRenderBackends();
        auto i                      = std::ranges::find(pv, renderbackend);
        if (i != pv.end()) {
            m_renderbackend = renderbackend;
            return;
        }
        FL_WARN(
            _log,
            LMsg("EngineSettings::setRenderBackend() - ")
                << renderbackend << " is not a valid render backend "
                << ".  Setting the render backend to the default value of \"SDL\".");

        m_renderbackend = "SDL";
    }

    std::vector<std::string> EngineSettings::getPossibleRenderBackends()
    {
        std::vector<std::string> tmp;
        tmp.emplace_back("SDL");
        tmp.emplace_back("OpenGL");
        return tmp;
    }

    void EngineSettings::setSDLRemoveFakeAlpha(bool sdlremovefakealpha)
    {
        m_sdlremovefakealpha = sdlremovefakealpha;
    }

    void EngineSettings::setGLCompressImages(bool oglcompressimages)
    {
        m_oglcompressimages = oglcompressimages;
    }

    void EngineSettings::setGLUseFramebuffer(bool ogluseframebuffer)
    {
        m_ogluseframebuffer = ogluseframebuffer;
    }

    void EngineSettings::setGLUseNPOT(bool oglusenpot)
    {
        m_oglusenpot = oglusenpot;
    }

    void EngineSettings::setGLTextureFiltering(TextureFiltering filter)
    {
        m_oglTextureFilter = filter;
    }

    TextureFiltering EngineSettings::getGLTextureFiltering() const
    {
        return m_oglTextureFilter;
    }

    void EngineSettings::setGLUseMipmapping(bool mipmapping)
    {
        m_oglMipmapping = mipmapping;
    }

    bool EngineSettings::isGLUseMipmapping() const
    {
        return m_oglMipmapping;
    }

    void EngineSettings::setGLUseMonochrome(bool monochrome)
    {
        m_oglMonochrome = monochrome;
    }

    bool EngineSettings::isGLUseMonochrome() const
    {
        return m_oglMonochrome;
    }

    void EngineSettings::setGLUseDepthBuffer(bool buffer)
    {
        m_oglDepthBuffer = buffer;
    }

    bool EngineSettings::isGLUseDepthBuffer() const
    {
        return m_oglDepthBuffer;
    }

    void EngineSettings::setGLAlphaTestValue(float alpha)
    {
        m_alphaTestValue = alpha;
    }

    float EngineSettings::getGLAlphaTestValue() const
    {
        return m_alphaTestValue;
    }

    void EngineSettings::setScreenWidth(uint16_t screenwidth)
    {
        m_screenwidth = screenwidth;
    }

    void EngineSettings::setScreenHeight(uint16_t screenheight)
    {
        m_screenheight = screenheight;
    }

    void EngineSettings::setDefaultFontPath(const std::string& defaultfontpath)
    {
        m_defaultfontpath = defaultfontpath;
    }

    void EngineSettings::setDefaultFontSize(uint16_t defaultfontsize)
    {
        m_defaultfontsize = defaultfontsize;
    }

    void EngineSettings::setDefaultFontGlyphs(const std::string& defaultfontglyphs)
    {
        m_defaultfontglyphs = defaultfontglyphs;
    }

    void EngineSettings::setWindowTitle(const std::string& title)
    {
        m_windowtitle = title;
    }

    void EngineSettings::setWindowIcon(const std::string& icon)
    {
        m_windowicon = icon;
    }

    void EngineSettings::setColorKeyEnabled(bool colorkeyenable)
    {
        m_iscolorkeyenabled = colorkeyenable;
    }

    bool EngineSettings::isColorKeyEnabled() const
    {
        return m_iscolorkeyenabled;
    }

    void EngineSettings::setColorKey(uint8_t r, uint8_t g, uint8_t b)
    {
        m_colorkey.r = r;
        m_colorkey.g = g;
        m_colorkey.b = b;
    }

    const SDL_Color& EngineSettings::getColorKey() const
    {
        return m_colorkey;
    }

    void EngineSettings::setVideoDriver(const std::string& driver)
    {
        // TODO: validate the video driver
        m_videodriver = driver;
    }

    const std::string& EngineSettings::getVideoDriver() const
    {
        return m_videodriver;
    }
    void EngineSettings::setLightingModel(uint32_t lighting)
    {
        if (lighting <= 2) {
            m_lighting = lighting;
            return;
        }

        FL_WARN(
            _log,
            LMsg("EngineSettings::setLightingModel() - ")
                << lighting << " is not a valid lighting model."
                << ".  Setting the lighting model to the default value of 0 (off)");

        m_lighting = 0;
    }

    void EngineSettings::setFrameLimitEnabled(bool limited)
    {
        m_isframelimit = limited;
    }

    bool EngineSettings::isFrameLimitEnabled() const
    {
        return m_isframelimit;
    }

    void EngineSettings::setFrameLimit(uint16_t framelimit)
    {
        m_framelimit = framelimit;
    }

    uint16_t EngineSettings::getFrameLimit() const
    {
        return m_framelimit;
    }

    void EngineSettings::setMouseSensitivity(float sens)
    {
        m_mousesensitivity = sens;
    }

    float EngineSettings::getMouseSensitivity() const
    {
        return m_mousesensitivity;
    }

    void EngineSettings::setMouseAccelerationEnabled(bool acceleration)
    {
        m_mouseacceleration = acceleration;
    }

    bool EngineSettings::isMouseAccelerationEnabled() const
    {
        return m_mouseacceleration;
    }

    void EngineSettings::setNativeImageCursorEnabled(bool nativeimagecursor)
    {
        m_nativeimagecursor = nativeimagecursor;
    }

    bool EngineSettings::isNativeImageCursorEnabled() const
    {
        return m_nativeimagecursor;
    }

    void EngineSettings::setJoystickSupport(bool support)
    {
        m_joystickSupport = support;
    }

    bool EngineSettings::isJoystickSupport() const
    {
        return m_joystickSupport;
    }
} // namespace FIFE
