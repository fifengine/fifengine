// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "renderbackend.h"

// Standard C++ library includes
#include <cassert>
#include <limits>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "util/time/timemanager.h"
#include "video/devicecaps.h"
#include "video/window/window.h"

namespace FIFE
{
    RenderBackend::RenderBackend(SDL_Color const & colorkey) :
        m_window(nullptr),
        m_screen(nullptr),
        m_target(nullptr),
        m_compressimages(false),
        m_useframebuffer(false),
        m_usenpot(false),
        m_isalphaoptimized(false),
        m_iscolorkeyenabled(false),
        m_colorkey(colorkey),
        m_rgba_format{},
        m_isbackgroundcolor(false),
        m_backgroundcolor{.r = 0, .g = 0, .b = 0, .a = 0},
        m_isMipmapping(false),
        m_textureFilter(TEXTURE_FILTER_NONE),
        m_maxAnisotropy(0),
        m_monochrome(false),
        m_isDepthBuffer(false),
        m_alphaValue(0.3F),
        m_vSync(false),
        m_guiClip(),
        m_windowObject(nullptr),
        m_baseWidth(1280),
        m_baseHeight(720),
        m_scalingMode(ScalingMode::Linear),
        m_isframelimit(false),
        m_frame_start(0),
        m_framelimit(60)
    {
    }

    RenderBackend::~RenderBackend() = default;

    void RenderBackend::deinit()
    {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        SDL_Quit();
    }

    void RenderBackend::startFrame()
    {
        if (m_isframelimit) {
            m_frame_start = TimeManager::instance()->getTicks64();
        }
    }

    void RenderBackend::endFrame()
    {
        if (m_isframelimit && m_framelimit > 0) {
            Uint64 const frame_time = TimeManager::instance()->getTicks64() - m_frame_start;
            float const frame_limit = 1000.0F / static_cast<float>(m_framelimit);
            if (frame_time < static_cast<Uint64>(frame_limit)) {
                Uint64 const remaining = static_cast<Uint64>(frame_limit) - frame_time;
                SDL_Delay(static_cast<Uint32>(remaining));
            }
        }
    }

    uint32_t RenderBackend::getWidth() const
    {
        if (m_windowObject != nullptr) {
            return static_cast<uint32_t>(m_windowObject->getWidthInPoints());
        }
        assert(m_screen->w >= 0);
        return static_cast<uint32_t>(m_screen->w);
    }

    uint32_t RenderBackend::getHeight() const
    {
        if (m_windowObject != nullptr) {
            return static_cast<uint32_t>(m_windowObject->getHeightInPoints());
        }
        assert(m_screen->h >= 0);
        return static_cast<uint32_t>(m_screen->h);
    }

    Rect const & RenderBackend::getArea() const
    {
        if (m_windowObject != nullptr) {
            static Rect const r(0, 0, m_windowObject->getWidthInPoints(), m_windowObject->getHeightInPoints());
            return r;
        }
        assert(m_screen->w >= 0);
        assert(m_screen->h >= 0);
        static Rect const r(0, 0, m_screen->w, m_screen->h);
        return r;
    }

    void RenderBackend::pushClipArea(Rect const & cliparea, bool clear)
    {
        ClipInfo ci;
        ci.r        = cliparea;
        ci.clearing = clear;
        m_clipstack.push(ci);
        setClipArea(cliparea, clear);
    }

    void RenderBackend::popClipArea()
    {
        assert(!m_clipstack.empty());
        m_clipstack.pop();
        if (m_clipstack.empty()) {
            setClipArea(getArea(), false);
        } else {
            ClipInfo const ci = m_clipstack.top();
            // instead of using ci.clearing, we set it to false
            // to avoid double clearing
            setClipArea(ci.r, false);
        }
    }

    Rect const & RenderBackend::getClipArea() const
    {
        if (!m_clipstack.empty()) {
            return m_clipstack.top().r;
        }
        return getArea();
    }

    void RenderBackend::clearClipArea()
    {
        setClipArea(getArea(), true);
    }

    void RenderBackend::setTextureFiltering(TextureFiltering filter)
    {
        m_textureFilter = filter;
    }

    TextureFiltering RenderBackend::getTextureFiltering() const
    {
        return m_textureFilter;
    }

    void RenderBackend::setMipmappingEnabled(bool enabled)
    {
        m_isMipmapping = enabled;
    }

    bool RenderBackend::isMipmappingEnabled() const
    {
        return m_isMipmapping;
    }

    int32_t RenderBackend::getMaxAnisotropy() const
    {
        return m_maxAnisotropy;
    }

    void RenderBackend::setMonochromeEnabled(bool enabled)
    {
        m_monochrome = enabled;
    }

    bool RenderBackend::isMonochromeEnabled() const
    {
        return m_monochrome;
    }

    void RenderBackend::setDepthBufferEnabled(bool enabled)
    {
        m_isDepthBuffer = enabled;
    }

    bool RenderBackend::isDepthBufferEnabled() const
    {
        return m_isDepthBuffer;
    }

    void RenderBackend::setAlphaTestValue(float alpha)
    {
        m_alphaValue = alpha;
    }

    float RenderBackend::getAlphaTestValue() const
    {
        return m_alphaValue;
    }

    void RenderBackend::setColorKeyEnabled(bool colorkeyenable)
    {
        m_iscolorkeyenabled = colorkeyenable;
    }

    bool RenderBackend::isColorKeyEnabled() const
    {
        return m_iscolorkeyenabled;
    }

    void RenderBackend::setColorKey(SDL_Color const & colorkey)
    {
        m_colorkey = colorkey;
    }

    SDL_Color const & RenderBackend::getColorKey() const
    {
        return m_colorkey;
    }

    void RenderBackend::setBackgroundColor(uint8_t r, uint8_t g, uint8_t b)
    {
        if (r != m_backgroundcolor.r || g != m_backgroundcolor.g || b != m_backgroundcolor.b) {
            m_isbackgroundcolor = true;
            m_backgroundcolor.r = r;
            m_backgroundcolor.g = g;
            m_backgroundcolor.b = b;
        }
    }

    void RenderBackend::resetBackgroundColor()
    {
        setBackgroundColor(0, 0, 0);
    }

    SDL_PixelFormat RenderBackend::getPixelFormat() const
    {
        return m_rgba_format.format;
    }

    void RenderBackend::setVSyncEnabled(bool vsync)
    {
        m_vSync = vsync;
    }

    bool RenderBackend::isVSyncEnabled() const
    {
        return m_vSync;
    }

    void RenderBackend::setFrameLimitEnabled(bool limited)
    {
        m_isframelimit = limited;
    }

    bool RenderBackend::isFrameLimitEnabled() const
    {
        return m_isframelimit;
    }

    void RenderBackend::setFrameLimit(uint16_t framelimit)
    {
        m_framelimit = framelimit;
    }

    uint16_t RenderBackend::getFrameLimit() const
    {
        return m_framelimit;
    }

    SDL_Surface* RenderBackend::getScreenSurface()
    {
        return m_screen;
    }

    SDL_Surface* RenderBackend::getRenderTargetSurface()
    {
        return m_target;
    }

    Point RenderBackend::getBezierPoint(std::vector<Point> const & points, int32_t elements, float t)
    {
        if (t < 0.0) {
            return points.at(0);
        }
        if (t >= static_cast<double>(elements)) {
            return points.back();
        }

        // Interpolate
        double px       = 0.0;
        double py       = 0.0;
        int32_t const n = elements - 1;
        double muk      = 1.0;
        double const mu = static_cast<double>(t) / static_cast<double>(elements);
        double munk     = Mathd::Pow(1.0 - mu, static_cast<double>(n));
        for (size_t pointIndex = 0; std::cmp_less_equal(pointIndex, n); ++pointIndex) {
            int32_t const i = static_cast<int32_t>(pointIndex);
            int32_t tmpn    = n;
            int32_t tmpi    = i;
            int32_t diffn   = n - i;
            double blend    = muk * munk;
            muk *= mu;
            munk /= 1.0 - mu;
            while (tmpn != 0) {
                blend *= static_cast<double>(tmpn);
                tmpn--;
                if (tmpi > 1) {
                    blend /= static_cast<double>(tmpi);
                    tmpi--;
                }
                if (diffn > 1) {
                    blend /= static_cast<double>(diffn);
                    diffn--;
                }
            }
            px += static_cast<double>(points.at(pointIndex).x) * blend;
            py += static_cast<double>(points.at(pointIndex).y) * blend;
        }

        return Point(static_cast<int32_t>(px), static_cast<int32_t>(py));
    }

    void RenderBackend::addControlPoints(std::vector<Point> const & points, std::vector<Point>& newPoints)
    {
        if (points.empty()) {
            return;
        }

        size_t const segmentCount = points.size() - 1;
        assert(segmentCount <= static_cast<size_t>(std::numeric_limits<int32_t>::max()));
        int32_t const n = static_cast<int32_t>(segmentCount);
        // min 2 points
        if (n < 1) {
            return;
        }

        Point p;
        // straight line
        if (n == 1) {
            newPoints.push_back(points.at(0));
            p.x = (((2 * points.at(0).x) + points.at(1).x) / 3);
            p.y = (((2 * points.at(0).y) + points.at(1).y) / 3);
            newPoints.push_back(p);
            p.x = (2 * p.x) - points.at(0).x;
            p.y = (2 * p.y) - points.at(0).y;
            newPoints.push_back(p);
            newPoints.push_back(points.at(1));
            return;
        }

        // calculate x and y values
        std::vector<float> xrhs(segmentCount);
        std::vector<float> yrhs(segmentCount);
        // first
        xrhs.at(0) = static_cast<float>(points.at(0).x) + static_cast<float>(2 * points.at(1).x);
        yrhs.at(0) = static_cast<float>(points.at(0).y) + static_cast<float>(2 * points.at(1).y);
        // last
        xrhs.at(segmentCount - 1) =
            static_cast<float>((8 * points.at(segmentCount - 1).x) + points.at(segmentCount).x) / 2.0F;
        yrhs.at(segmentCount - 1) =
            static_cast<float>((8 * points.at(segmentCount - 1).y) + points.at(segmentCount).y) / 2.0F;
        // rest
        for (size_t i = 1; i + 1 < segmentCount; ++i) {
            xrhs.at(i) = static_cast<float>((4 * points.at(i).x) + (2 * points.at(i + 1).x));
            yrhs.at(i) = static_cast<float>((4 * points.at(i).y) + (2 * points.at(i + 1).y));
        }

        std::vector<float> x(segmentCount);
        std::vector<float> y(segmentCount);
        std::vector<float> xtmp(segmentCount);
        std::vector<float> ytmp(segmentCount);
        float xb = 2.0;
        float yb = 2.0;
        x.at(0)  = xrhs.at(0) / xb;
        y.at(0)  = yrhs.at(0) / yb;
        // Decomposition and forward substitution.
        for (size_t i = 1; i < segmentCount; ++i) {
            xtmp.at(i) = 1.0F / xb;
            ytmp.at(i) = 1.0F / yb;
            xb         = (i + 1 < segmentCount ? 4.0F : 3.5F) - xtmp.at(i);
            yb         = (i + 1 < segmentCount ? 4.0F : 3.5F) - ytmp.at(i);
            x.at(i)    = (xrhs.at(i) - x.at(i - 1)) / xb;
            y.at(i)    = (yrhs.at(i) - y.at(i - 1)) / yb;
        }
        // Backward substitution
        for (size_t i = 1; i < segmentCount; ++i) {
            size_t const backIndex = segmentCount - i;
            x.at(backIndex - 1) -= xtmp.at(backIndex) * x.at(backIndex);
            y.at(backIndex - 1) -= ytmp.at(backIndex) * y.at(backIndex);
        }

        // start point
        newPoints.push_back(points.at(0));
        for (size_t i = 0; i + 1 < segmentCount; ++i) {
            p.x = static_cast<int32_t>(x.at(i));
            p.y = static_cast<int32_t>(y.at(i));
            newPoints.push_back(p);
            p.x = static_cast<int32_t>(static_cast<float>(2 * points.at(i + 1).x) - x.at(i + 1));
            p.y = static_cast<int32_t>(static_cast<float>(2 * points.at(i + 1).y) - y.at(i + 1));
            newPoints.push_back(p);

            newPoints.push_back(points.at(i + 1));
        }
        p.x = static_cast<int32_t>(x.at(segmentCount - 1));
        p.y = static_cast<int32_t>(y.at(segmentCount - 1));
        newPoints.push_back(p);
        p.x = static_cast<int32_t>(
            (static_cast<float>(points.at(segmentCount).x) + x.at(segmentCount - 1)) / static_cast<float>(2));
        p.y = static_cast<int32_t>(
            (static_cast<float>(points.at(segmentCount).y) + y.at(segmentCount - 1)) / static_cast<float>(2));
        newPoints.push_back(p);
        // end point
        newPoints.push_back(points.at(segmentCount));
    }

    int RenderBackend::getWidthInPoints() const
    {
        if (m_windowObject != nullptr) {
            return m_windowObject->getWidthInPoints();
        }
        return static_cast<int>(getWidth());
    }

    int RenderBackend::getWidthInPixels() const
    {
        if (m_windowObject != nullptr) {
            return m_windowObject->getWidthInPixels();
        }
        return static_cast<int>(getWidth());
    }

    int RenderBackend::getHeightInPoints() const
    {
        if (m_windowObject != nullptr) {
            return m_windowObject->getHeightInPoints();
        }
        return static_cast<int>(getHeight());
    }

    int RenderBackend::getHeightInPixels() const
    {
        if (m_windowObject != nullptr) {
            return m_windowObject->getHeightInPixels();
        }
        return static_cast<int>(getHeight());
    }

    float RenderBackend::getDPIScaleFactor() const
    {
        if (m_windowObject != nullptr) {
            return m_windowObject->getDPIScaleFactor();
        }
        return 1.0F;
    }

    void RenderBackend::setWindowObject(Window* window)
    {
        m_windowObject = window;
        if (m_windowObject != nullptr) {
            m_window = m_windowObject->getSDLWindow();
        }
    }

    void RenderBackend::setScalingMode(ScalingMode mode)
    {
        m_scalingMode = mode;
    }

    ScalingMode RenderBackend::getScalingMode() const
    {
        return m_scalingMode;
    }
} // namespace FIFE
