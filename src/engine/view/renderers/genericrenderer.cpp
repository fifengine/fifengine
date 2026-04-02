// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cassert>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/timeprovider.h"
#include "model/structures/instance.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"
#include "util/time/timemanager.h"
#include "video/animation.h"
#include "video/fonts/ifont.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/renderbackend.h"

#include "genericrenderer.h"
#include "view/camera.h"

namespace FIFE
{
    namespace
    {
        [[nodiscard]] int32_t toScreenSize(const uint32_t value)
        {
            assert(value <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
            return static_cast<int32_t>(value);
        }

        [[nodiscard]] uint32_t toAnimationTimestamp(const int32_t value)
        {
            assert(value >= 0);
            return static_cast<uint32_t>(value);
        }

        [[nodiscard]] uint8_t toVertexSize(const int32_t value)
        {
            assert(value >= 0);
            assert(value <= std::numeric_limits<uint8_t>::max());
            return static_cast<uint8_t>(value);
        }
    } // namespace

    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_VIEWVIEW);

    GenericRendererLineInfo::GenericRendererLineInfo(
        const RendererNode& n1, const RendererNode& n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
        GenericRendererElementInfo(), m_edge1(n1), m_edge2(n2), m_red(r), m_green(g), m_blue(b), m_alpha(a)
    {
    }

    GenericRendererLineInfo::~GenericRendererLineInfo() = default;

    void GenericRendererLineInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend)
    {
        static_cast<void>(instances);
        Point const p1 = m_edge1.getCalculatedPoint(cam, layer);
        Point const p2 = m_edge2.getCalculatedPoint(cam, layer);
        if (m_edge1.getLayer() == layer) {
            renderbackend->drawLine(p1, p2, m_red, m_green, m_blue, m_alpha);
            if (renderbackend->getLightingModel() > 0) {
                renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, false, false, 0, KEEP, ALWAYS);
            }
        }
    }

    GenericRendererPointInfo::GenericRendererPointInfo(
        const RendererNode& anchor, uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
        GenericRendererElementInfo(), m_anchor(anchor), m_red(r), m_green(g), m_blue(b), m_alpha(a)
    {
    }

    GenericRendererPointInfo::~GenericRendererPointInfo() = default;

    void GenericRendererPointInfo::render(
        Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend)
    {
        static_cast<void>(instances);
        Point const p = m_anchor.getCalculatedPoint(cam, layer);
        if (m_anchor.getLayer() == layer) {
            renderbackend->putPixel(p.x, p.y, m_red, m_green, m_blue, m_alpha);
            if (renderbackend->getLightingModel() > 0) {
                renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, false, false, 0, KEEP, ALWAYS);
            }
        }
    }

    GenericRendererTriangleInfo::GenericRendererTriangleInfo(
        const RendererNode& n1,
        const RendererNode& n2,
        const RendererNode& n3,
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t a) :
        GenericRendererElementInfo(), m_edge1(n1), m_edge2(n2), m_edge3(n3), m_red(r), m_green(g), m_blue(b), m_alpha(a)
    {
    }

    GenericRendererTriangleInfo::~GenericRendererTriangleInfo() = default;

    void GenericRendererTriangleInfo::render(
        Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend)
    {
        static_cast<void>(instances);
        Point const p1 = m_edge1.getCalculatedPoint(cam, layer);
        Point const p2 = m_edge2.getCalculatedPoint(cam, layer);
        Point const p3 = m_edge3.getCalculatedPoint(cam, layer);
        if (m_edge1.getLayer() == layer) {
            renderbackend->drawTriangle(p1, p2, p3, m_red, m_green, m_blue, m_alpha);
            if (renderbackend->getLightingModel() > 0) {
                renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, false, false, 0, KEEP, ALWAYS);
            }
        }
    }

    GenericRendererQuadInfo::GenericRendererQuadInfo(
        const RendererNode& n1,
        const RendererNode& n2,
        const RendererNode& n3,
        const RendererNode& n4,
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t a) :
        GenericRendererElementInfo(),
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

    GenericRendererQuadInfo::~GenericRendererQuadInfo() = default;

    void GenericRendererQuadInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend)
    {
        static_cast<void>(instances);
        Point const p1 = m_edge1.getCalculatedPoint(cam, layer);
        Point const p2 = m_edge2.getCalculatedPoint(cam, layer);
        Point const p3 = m_edge3.getCalculatedPoint(cam, layer);
        Point const p4 = m_edge4.getCalculatedPoint(cam, layer);
        if (m_edge1.getLayer() == layer) {
            renderbackend->drawQuad(p1, p2, p3, p4, m_red, m_green, m_blue, m_alpha);
            if (renderbackend->getLightingModel() > 0) {
                renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, false, false, 0, KEEP, ALWAYS);
            }
        }
    }

    GenericRendererVertexInfo::GenericRendererVertexInfo(
        const RendererNode& center, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
        GenericRendererElementInfo(), m_center(center), m_size(size), m_red(r), m_green(g), m_blue(b), m_alpha(a)
    {
    }

    GenericRendererVertexInfo::~GenericRendererVertexInfo() = default;

    void GenericRendererVertexInfo::render(
        Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend)
    {
        static_cast<void>(instances);
        Point const p = m_center.getCalculatedPoint(cam, layer);
        if (m_center.getLayer() == layer) {
            renderbackend->drawVertex(p, toVertexSize(m_size), m_red, m_green, m_blue, m_alpha);
            if (renderbackend->getLightingModel() > 0) {
                renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, false, false, 0, KEEP, ALWAYS);
            }
        }
    }

    GenericRendererImageInfo::GenericRendererImageInfo(const RendererNode& anchor, const ImagePtr& image, bool zoomed) :
        GenericRendererElementInfo(), m_anchor(anchor), m_image(image), m_zoomed(zoomed)
    {
    }

    GenericRendererImageInfo::~GenericRendererImageInfo() = default;

    void GenericRendererImageInfo::render(
        Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend)
    {
        static_cast<void>(instances);
        static_cast<void>(renderbackend);
        Point const p = m_anchor.getCalculatedPoint(cam, layer, m_zoomed);
        if (m_anchor.getLayer() == layer) {
            Rect r;
            Rect const viewport = cam->getViewPort();
            int32_t width       = 0;
            int32_t height      = 0;
            if (m_zoomed) {
                width  = static_cast<int32_t>(round(m_image->getWidth() * cam->getZoom()));
                height = static_cast<int32_t>(round(m_image->getHeight() * cam->getZoom()));
            } else {
                width  = toScreenSize(m_image->getWidth());
                height = toScreenSize(m_image->getHeight());
            }
            r.x = p.x - width / 2;
            r.y = p.y - height / 2;
            r.w = width;
            r.h = height;
            if (r.intersects(viewport)) {
                m_image->render(r);
            }
        }
    }

    GenericRendererAnimationInfo::GenericRendererAnimationInfo(
        const RendererNode& anchor, const AnimationPtr& animation, bool zoomed) :
        GenericRendererElementInfo(),
        m_anchor(anchor),
        m_animation(animation),
        m_start_time(TimeManager::instance()->getTime()),
        m_time_scale(1.0),
        m_zoomed(zoomed)
    {
    }

    GenericRendererAnimationInfo::~GenericRendererAnimationInfo() = default;

    void GenericRendererAnimationInfo::render(
        Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend)
    {
        static_cast<void>(instances);
        static_cast<void>(renderbackend);
        Point const p = m_anchor.getCalculatedPoint(cam, layer, m_zoomed);
        if (m_anchor.getLayer() == layer) {
            const uint32_t duration = toAnimationTimestamp(m_animation->getDuration());
            const uint32_t animtime =
                scaleTime(m_time_scale, TimeManager::instance()->getTime() - m_start_time) % duration;
            ImagePtr const img = m_animation->getFrameByTimestamp(animtime);
            Rect r;
            Rect const viewport = cam->getViewPort();
            int32_t width       = 0;
            int32_t height      = 0;
            if (m_zoomed) {
                width  = static_cast<int32_t>(round(img->getWidth() * cam->getZoom()));
                height = static_cast<int32_t>(round(img->getHeight() * cam->getZoom()));
            } else {
                width  = toScreenSize(img->getWidth());
                height = toScreenSize(img->getHeight());
            }
            r.x = p.x - width / 2;
            r.y = p.y - height / 2;
            r.w = width;
            r.h = height;
            if (r.intersects(viewport)) {
                img->render(r);
            }
        }
    }

    GenericRendererTextInfo::GenericRendererTextInfo(
        const RendererNode& anchor, IFont* font, std::string text, bool zoomed) :
        GenericRendererElementInfo(), m_anchor(anchor), m_font(font), m_text(std::move(text)), m_zoomed(zoomed)
    {
    }

    GenericRendererTextInfo::~GenericRendererTextInfo() = default;

    void GenericRendererTextInfo::render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend)
    {
        static_cast<void>(instances);
        Point const p = m_anchor.getCalculatedPoint(cam, layer, m_zoomed);
        if (m_anchor.getLayer() == layer) {
            Image* img = m_font->getAsImageMultiline(m_text);
            Rect r;
            Rect const viewport = cam->getViewPort();
            int32_t width       = 0;
            int32_t height      = 0;
            if (m_zoomed) {
                width  = static_cast<int32_t>(round(img->getWidth() * cam->getZoom()));
                height = static_cast<int32_t>(round(img->getHeight() * cam->getZoom()));
            } else {
                width  = toScreenSize(img->getWidth());
                height = toScreenSize(img->getHeight());
            }
            r.x = p.x - width / 2;
            r.y = p.y - height / 2;
            r.w = width;
            r.h = height;
            if (r.intersects(viewport)) {
                img->render(r);
                if (renderbackend->getLightingModel() > 0) {
                    renderbackend->changeRenderInfos(RENDER_DATA_WITHOUT_Z, 1, 4, 5, false, false, 0, KEEP, ALWAYS);
                }
            }
        }
    }

    GenericRendererResizeInfo::GenericRendererResizeInfo(
        const RendererNode& anchor, const ImagePtr& image, int32_t width, int32_t height, bool zoomed) :
        GenericRendererElementInfo(),
        m_anchor(anchor),
        m_image(image),
        m_width(width),
        m_height(height),
        m_zoomed(zoomed)
    {
    }

    GenericRendererResizeInfo::~GenericRendererResizeInfo() = default;

    void GenericRendererResizeInfo::render(
        Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend)
    {
        static_cast<void>(instances);
        static_cast<void>(renderbackend);
        Point const p = m_anchor.getCalculatedPoint(cam, layer, m_zoomed);
        if (m_anchor.getLayer() == layer) {
            Rect r;
            Rect const viewport = cam->getViewPort();
            int32_t width       = 0;
            int32_t height      = 0;
            if (m_zoomed) {
                width  = static_cast<int32_t>(round(m_width * cam->getZoom()));
                height = static_cast<int32_t>(round(m_height * cam->getZoom()));
            } else {
                width  = m_width;
                height = m_height;
            }
            r.x = p.x - width / 2;
            r.y = p.y - height / 2;
            r.w = width;
            r.h = height;
            if (r.intersects(viewport)) {
                m_image->render(r);
            }
        }
    }

    GenericRenderer* GenericRenderer::getInstance(IRendererContainer* cnt)
    {
        return dynamic_cast<GenericRenderer*>(cnt->getRenderer("GenericRenderer"));
    }

    GenericRenderer::GenericRenderer(RenderBackend* renderbackend, int32_t position) :
        RendererBase(renderbackend, position)
    {
        setEnabled(false);
    }

    GenericRenderer::GenericRenderer(const GenericRenderer& old) : RendererBase(old)
    {
        setEnabled(false);
    }

    RendererBase* GenericRenderer::clone()
    {
        return new GenericRenderer(*this);
    }

    GenericRenderer::~GenericRenderer() = default;
    void GenericRenderer::addLine(
        const std::string& group,
        const RendererNode& n1,
        const RendererNode& n2,
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t a)
    {
        GenericRendererElementInfo* info = new GenericRendererLineInfo(n1, n2, r, g, b, a);
        m_groups[group].push_back(info);
    }
    void GenericRenderer::addPoint(
        const std::string& group, const RendererNode& n, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        GenericRendererElementInfo* info = new GenericRendererPointInfo(n, r, g, b, a);
        m_groups[group].push_back(info);
    }
    void GenericRenderer::addTriangle(
        const std::string& group,
        const RendererNode& n1,
        const RendererNode& n2,
        const RendererNode& n3,
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t a)
    {
        GenericRendererElementInfo* info = new GenericRendererTriangleInfo(n1, n2, n3, r, g, b, a);
        m_groups[group].push_back(info);
    }
    void GenericRenderer::addQuad(
        const std::string& group,
        const RendererNode& n1,
        const RendererNode& n2,
        const RendererNode& n3,
        const RendererNode& n4,
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t a)
    {
        GenericRendererElementInfo* info = new GenericRendererQuadInfo(n1, n2, n3, n4, r, g, b, a);
        m_groups[group].push_back(info);
    }
    void GenericRenderer::addVertex(
        const std::string& group, const RendererNode& n, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        GenericRendererElementInfo* info = new GenericRendererVertexInfo(n, size, r, g, b, a);
        m_groups[group].push_back(info);
    }
    void GenericRenderer::addText(
        const std::string& group, const RendererNode& n, IFont* font, const std::string& text, bool zoomed)
    {
        GenericRendererElementInfo* info = new GenericRendererTextInfo(n, font, text, zoomed);
        m_groups[group].push_back(info);
    }
    void GenericRenderer::addImage(const std::string& group, const RendererNode& n, const ImagePtr& image, bool zoomed)
    {
        GenericRendererElementInfo* info = new GenericRendererImageInfo(n, image, zoomed);
        m_groups[group].push_back(info);
    }
    void GenericRenderer::addAnimation(
        const std::string& group, const RendererNode& n, const AnimationPtr& animation, bool zoomed)
    {
        GenericRendererElementInfo* info = new GenericRendererAnimationInfo(n, animation, zoomed);
        m_groups[group].push_back(info);
    }
    void GenericRenderer::resizeImage(
        const std::string& group,
        const RendererNode& n,
        const ImagePtr& image,
        int32_t width,
        int32_t height,
        bool zoomed)
    {
        GenericRendererElementInfo* info = new GenericRendererResizeInfo(n, image, width, height, zoomed);
        m_groups[group].push_back(info);
    }
    void GenericRenderer::removeAll(const std::string& group)
    {
        auto info_it = m_groups[group].begin();
        for (; info_it != m_groups[group].end(); ++info_it) {
            delete *info_it;
        }
        m_groups[group].clear();
        m_groups.erase(group);
    }
    // Remove all groups
    void GenericRenderer::removeAll()
    {
        auto it = m_groups.begin();
        for (; it != m_groups.end(); ++it) {
            auto info_it = it->second.begin();
            for (; info_it != it->second.end(); ++info_it) {
                delete *info_it;
            }
        }
        m_groups.clear();
    }
    // Clear all groups
    void GenericRenderer::reset()
    {
        removeAll();
    }

    void GenericRenderer::render(Camera* cam, Layer* layer, RenderList& instances)
    {
        auto group_it = m_groups.begin();
        for (; group_it != m_groups.end(); ++group_it) {
            auto info_it = group_it->second.begin();
            for (; info_it != group_it->second.end(); ++info_it) {
                (*info_it)->render(cam, layer, instances, m_renderbackend);
            }
        }
    }
} // namespace FIFE
