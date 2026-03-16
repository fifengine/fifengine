// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <map>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/timeprovider.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"
#include "util/time/timemanager.h"
#include "video/fonts/ifont.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/renderbackend.h"

#include "offrenderer.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_VIEWVIEW);

    OffRendererLineInfo::OffRendererLineInfo(Point n1, Point n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
        OffRendererElementInfo(), m_edge1(n1), m_edge2(n2), m_red(r), m_green(g), m_blue(b), m_alpha(a)
    {
    }

    OffRendererLineInfo::~OffRendererLineInfo() = default;

    void OffRendererLineInfo::render(RenderBackend* renderbackend)
    {
        renderbackend->drawLine(m_edge1, m_edge2, m_red, m_green, m_blue, m_alpha);
    }

    OffRendererPointInfo::OffRendererPointInfo(Point anchor, uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
        OffRendererElementInfo(), m_anchor(anchor), m_red(r), m_green(g), m_blue(b), m_alpha(a)
    {
    }

    OffRendererPointInfo::~OffRendererPointInfo() = default;

    void OffRendererPointInfo::render(RenderBackend* renderbackend)
    {
        renderbackend->putPixel(m_anchor.x, m_anchor.y, m_red, m_green, m_blue, m_alpha);
    }

    OffRendererTriangleInfo::OffRendererTriangleInfo(
        Point n1, Point n2, Point n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
        OffRendererElementInfo(), m_edge1(n1), m_edge2(n2), m_edge3(n3), m_red(r), m_green(g), m_blue(b), m_alpha(a)
    {
    }

    OffRendererTriangleInfo::~OffRendererTriangleInfo() = default;

    void OffRendererTriangleInfo::render(RenderBackend* renderbackend)
    {
        renderbackend->drawTriangle(m_edge1, m_edge2, m_edge3, m_red, m_green, m_blue, m_alpha);
    }

    OffRendererQuadInfo::OffRendererQuadInfo(
        Point n1, Point n2, Point n3, Point n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
        OffRendererElementInfo(),
        m_edge1(n1),
        m_edge2(n2),
        m_edge3(n3),
        m_edge4(n4),
        m_red(r),
        m_green(g),
        m_blue(b),
        m_alpha(a)
    {
    }

    OffRendererQuadInfo::~OffRendererQuadInfo() = default;

    void OffRendererQuadInfo::render(RenderBackend* renderbackend)
    {
        renderbackend->drawQuad(m_edge1, m_edge2, m_edge3, m_edge4, m_red, m_green, m_blue, m_alpha);
    }

    OffRendererVertexInfo::OffRendererVertexInfo(
        Point center, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
        OffRendererElementInfo(), m_center(center), m_size(size), m_red(r), m_green(g), m_blue(b), m_alpha(a)
    {
    }

    OffRendererVertexInfo::~OffRendererVertexInfo() = default;

    void OffRendererVertexInfo::render(RenderBackend* renderbackend)
    {
        renderbackend->drawVertex(m_center, m_size, m_red, m_green, m_blue, m_alpha);
    }

    OffRendererImageInfo::OffRendererImageInfo(Point anchor, ImagePtr image) :
        OffRendererElementInfo(), m_anchor(anchor), m_image(image)
    {
    }

    OffRendererImageInfo::~OffRendererImageInfo() = default;

    void OffRendererImageInfo::render(RenderBackend* renderbackend)
    {
        Rect r;
        uint16_t width  = m_image->getWidth();
        uint16_t height = m_image->getHeight();
        r.x             = m_anchor.x - width / 2;
        r.y             = m_anchor.y - height / 2;
        r.w             = width;
        r.h             = height;

        m_image->render(r);
    }

    OffRendererAnimationInfo::OffRendererAnimationInfo(Point anchor, AnimationPtr animation) :
        OffRendererElementInfo(),
        m_anchor(anchor),
        m_animation(animation),
        m_start_time(TimeManager::instance()->getTime()),
        m_time_scale(1.0)
    {
    }

    OffRendererAnimationInfo::~OffRendererAnimationInfo() = default;

    void OffRendererAnimationInfo::render(RenderBackend* renderbackend)
    {
        int32_t animtime =
            scaleTime(m_time_scale, TimeManager::instance()->getTime() - m_start_time) % m_animation->getDuration();
        ImagePtr img = m_animation->getFrameByTimestamp(animtime);

        Rect r;
        uint16_t width  = img->getWidth();
        uint16_t height = img->getHeight();
        r.x             = m_anchor.x - width / 2;
        r.y             = m_anchor.y - height / 2;
        r.w             = width;
        r.h             = height;

        img->render(r);
    }

    OffRendererTextInfo::OffRendererTextInfo(Point anchor, IFont* font, std::string text) :
        OffRendererElementInfo(), m_anchor(anchor), m_font(font), m_text(std::move(text))
    {
    }

    OffRendererTextInfo::~OffRendererTextInfo() = default;

    void OffRendererTextInfo::render(RenderBackend* renderbackend)
    {
        Image* img = m_font->getAsImageMultiline(m_text);

        Rect r;
        uint16_t width  = img->getWidth();
        uint16_t height = img->getHeight();
        r.x             = m_anchor.x - width / 2;
        r.y             = m_anchor.y - height / 2;
        r.w             = width;
        r.h             = height;

        img->render(r);
    }

    OffRendererResizeInfo::OffRendererResizeInfo(Point anchor, ImagePtr image, int32_t width, int32_t height) :
        OffRendererElementInfo(), m_anchor(anchor), m_image(image), m_width(width), m_height(height)
    {
    }

    OffRendererResizeInfo::~OffRendererResizeInfo() = default;

    void OffRendererResizeInfo::render(RenderBackend* renderbackend)
    {
        Rect r;
        uint16_t width  = m_width;
        uint16_t height = m_height;
        r.x             = m_anchor.x - width / 2;
        r.y             = m_anchor.y - height / 2;
        r.w             = width;
        r.h             = height;

        m_image->render(r);
    }

    OffRenderer::OffRenderer(RenderBackend* renderbackend) :
        m_renderbackend(renderbackend), m_enabled(false), m_area(renderbackend->getArea())
    {
    }

    OffRenderer::~OffRenderer()
    {
        removeAll();
    }

    void OffRenderer::setEnabled(bool enabled)
    {
        m_enabled = enabled;
    }

    bool OffRenderer::isEnabled()
    {
        return m_enabled;
    }

    void OffRenderer::setClipArea(Rect area)
    {
        m_area = area;
    }

    const Rect& OffRenderer::getClipArea() const
    {
        return m_area;
    }

    void OffRenderer::addLine(const std::string& group, Point n1, Point n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        OffRendererElementInfo* info = new OffRendererLineInfo(n1, n2, r, g, b, a);
        m_groups[group].push_back(info);
    }
    void OffRenderer::addPoint(const std::string& group, Point n, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        OffRendererElementInfo* info = new OffRendererPointInfo(n, r, g, b, a);
        m_groups[group].push_back(info);
    }
    void OffRenderer::addTriangle(
        const std::string& group, Point n1, Point n2, Point n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        OffRendererElementInfo* info = new OffRendererTriangleInfo(n1, n2, n3, r, g, b, a);
        m_groups[group].push_back(info);
    }
    void OffRenderer::addQuad(
        const std::string& group, Point n1, Point n2, Point n3, Point n4, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        OffRendererElementInfo* info = new OffRendererQuadInfo(n1, n2, n3, n4, r, g, b, a);
        m_groups[group].push_back(info);
    }
    void OffRenderer::addVertex(
        const std::string& group, Point n, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        OffRendererElementInfo* info = new OffRendererVertexInfo(n, size, r, g, b, a);
        m_groups[group].push_back(info);
    }
    void OffRenderer::addText(const std::string& group, Point n, IFont* font, const std::string& text)
    {
        OffRendererElementInfo* info = new OffRendererTextInfo(n, font, text);
        m_groups[group].push_back(info);
    }
    void OffRenderer::addImage(const std::string& group, Point n, ImagePtr image)
    {
        OffRendererElementInfo* info = new OffRendererImageInfo(n, image);
        m_groups[group].push_back(info);
    }
    void OffRenderer::addAnimation(const std::string& group, Point n, AnimationPtr animation)
    {
        OffRendererElementInfo* info = new OffRendererAnimationInfo(n, animation);
        m_groups[group].push_back(info);
    }
    void OffRenderer::resizeImage(const std::string& group, Point n, ImagePtr image, int32_t width, int32_t height)
    {
        OffRendererElementInfo* info = new OffRendererResizeInfo(n, image, width, height);
        m_groups[group].push_back(info);
    }
    void OffRenderer::removeAll(const std::string& group)
    {
        auto info_it = m_groups[group].begin();
        for (; info_it != m_groups[group].end(); ++info_it) {
            delete *info_it;
        }
        m_groups[group].clear();
        m_groups.erase(group);
    }
    void OffRenderer::removeAll()
    {
        m_groups.clear();
    }

    void OffRenderer::render()
    {
        if (!m_enabled) {
            return;
        }
        m_renderbackend->pushClipArea(m_area);
        auto group_it = m_groups.begin();
        for (; group_it != m_groups.end(); ++group_it) {
            auto info_it = group_it->second.begin();
            for (; info_it != group_it->second.end(); ++info_it) {
                (*info_it)->render(m_renderbackend);
            }
        }
        m_renderbackend->renderVertexArrays();
        m_renderbackend->popClipArea();
    }
} // namespace FIFE
