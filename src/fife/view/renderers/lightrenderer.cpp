// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "lightrenderer.h"

// Standard C++ library includes
#include <list>
#include <map>
#include <string>
#include <vector>

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes
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
#include "video/opengl/glimage.h"
#include "video/opengl/renderbackendopengl.h"
#include "video/renderbackend.h"
#include "view/camera.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_VIEWVIEW);

    LightRendererElementInfo::LightRendererElementInfo(RendererNode const & n, int32_t src, int32_t dst) :
        m_anchor(n), m_src(src), m_dst(dst), m_stencil(false), m_stencil_ref(0)
    {
    }

    void LightRendererElementInfo::setStencil(uint8_t stencil_ref)
    {
        m_stencil     = true;
        m_stencil_ref = stencil_ref;
    }

    int32_t LightRendererElementInfo::getStencil() const
    {
        if (!m_stencil) {
            return -1;
        }
        return m_stencil_ref;
    }

    void LightRendererElementInfo::removeStencil()
    {
        m_stencil     = false;
        m_stencil_ref = 0;
    }

    LightRendererImageInfo::LightRendererImageInfo(
        RendererNode const & anchor, ImagePtr const & image, int32_t src, int32_t dst) :
        LightRendererElementInfo(anchor, src, dst), m_image(image)
    {
    }

    LightRendererImageInfo::~LightRendererImageInfo() = default;

    void LightRendererImageInfo::render(
        Camera* cam, Layer* layer, [[maybe_unused]] RenderList& instances, RenderBackend* renderbackend)
    {
        Point const p = m_anchor.getCalculatedPoint(cam, layer, true);
        if (m_anchor.getLayer() == layer) {
            Rect r;
            Rect const viewport = cam->getViewPort();
            auto width          = static_cast<uint32_t>(round(m_image->getWidth() * cam->getZoom()));
            auto height         = static_cast<uint32_t>(round(m_image->getHeight() * cam->getZoom()));
            r.x                 = p.x - width / 2;
            r.y                 = p.y - height / 2;
            r.w                 = width;
            r.h                 = height;

            if (r.intersects(viewport)) {
                auto const lm = renderbackend->getLightingModel();
                m_image->render(r);
                if (m_stencil) {
                    renderbackend->changeRenderInfos(
                        RENDER_DATA_WITHOUT_Z, 1, m_src, m_dst, false, true, m_stencil_ref, INCR, GEQUAL);
                } else if (lm == 1) {
                    renderbackend->changeRenderInfos(
                        RENDER_DATA_WITHOUT_Z, 1, m_src, m_dst, false, true, 255, KEEP, NOTEQUAL);
                }
            }
        }
    }

    LightRendererAnimationInfo::LightRendererAnimationInfo(
        RendererNode const & anchor, AnimationPtr const & animation, int32_t src, int32_t dst) :
        LightRendererElementInfo(anchor, src, dst),
        m_animation(animation),
        m_start_time(TimeManager::instance()->getTime()),
        m_time_scale(1.0)
    {
    }

    LightRendererAnimationInfo::~LightRendererAnimationInfo() = default;

    void LightRendererAnimationInfo::render(
        Camera* cam, Layer* layer, [[maybe_unused]] RenderList& instances, RenderBackend* renderbackend)
    {
        Point const p = m_anchor.getCalculatedPoint(cam, layer, true);
        if (m_anchor.getLayer() == layer) {
            int32_t const animtime =
                scaleTime(m_time_scale, TimeManager::instance()->getTime() - m_start_time) % m_animation->getDuration();
            ImagePtr const img = m_animation->getFrameByTimestamp(animtime);
            Rect r;
            Rect const viewport = cam->getViewPort();
            auto width          = static_cast<uint32_t>(round(img->getWidth() * cam->getZoom()));
            auto height         = static_cast<uint32_t>(round(img->getHeight() * cam->getZoom()));
            r.x                 = p.x - width / 2;
            r.y                 = p.y - height / 2;
            r.w                 = width;
            r.h                 = height;

            if (r.intersects(viewport)) {
                auto const lm = renderbackend->getLightingModel();
                img->render(r);
                if (m_stencil) {
                    renderbackend->changeRenderInfos(
                        RENDER_DATA_WITHOUT_Z, 1, m_src, m_dst, false, true, m_stencil_ref, INCR, GEQUAL);
                } else if (lm == 1) {
                    renderbackend->changeRenderInfos(
                        RENDER_DATA_WITHOUT_Z, 1, m_src, m_dst, false, true, 255, KEEP, NOTEQUAL);
                }
            }
        }
    }

    LightRendererResizeInfo::LightRendererResizeInfo(
        RendererNode const & anchor, ImagePtr const & image, int32_t width, int32_t height, int32_t src, int32_t dst) :
        LightRendererElementInfo(anchor, src, dst), m_image(image), m_width(width), m_height(height)
    {
    }

    LightRendererResizeInfo::~LightRendererResizeInfo() = default;

    void LightRendererResizeInfo::render(
        Camera* cam, Layer* layer, [[maybe_unused]] RenderList& instances, RenderBackend* renderbackend)
    {
        Point const p = m_anchor.getCalculatedPoint(cam, layer, true);
        if (m_anchor.getLayer() == layer) {
            Rect r;
            Rect const viewport = cam->getViewPort();
            auto width          = static_cast<uint32_t>(round(m_width * cam->getZoom()));
            auto height         = static_cast<uint32_t>(round(m_height * cam->getZoom()));
            r.x                 = p.x - width / 2;
            r.y                 = p.y - height / 2;
            r.w                 = width;
            r.h                 = height;

            if (r.intersects(viewport)) {
                auto const lm = renderbackend->getLightingModel();
                m_image->render(r);
                if (m_stencil) {
                    renderbackend->changeRenderInfos(
                        RENDER_DATA_WITHOUT_Z, 1, m_src, m_dst, false, true, m_stencil_ref, INCR, GEQUAL);
                } else if (lm == 1) {
                    renderbackend->changeRenderInfos(
                        RENDER_DATA_WITHOUT_Z, 1, m_src, m_dst, false, true, 255, KEEP, NOTEQUAL);
                }
            }
        }
    }

    LightRendererSimpleLightInfo::LightRendererSimpleLightInfo(
        RendererNode const & anchor,
        uint8_t intensity,
        float radius,
        int32_t subdivisions,
        float xstretch,
        float ystretch,
        uint8_t r,
        uint8_t g,
        uint8_t b,
        int32_t src,
        int32_t dst) :
        LightRendererElementInfo(anchor, src, dst),
        m_intensity(intensity),
        m_radius(radius),
        m_subdivisions(subdivisions),
        m_xstretch(xstretch),
        m_ystretch(ystretch),
        m_red(r),
        m_green(g),
        m_blue(b)
    {
    }

    LightRendererSimpleLightInfo::~LightRendererSimpleLightInfo() = default;

    void LightRendererSimpleLightInfo::render(
        Camera* cam, Layer* layer, [[maybe_unused]] RenderList& instances, RenderBackend* renderbackend)
    {
        Point const p = m_anchor.getCalculatedPoint(cam, layer, true);
        if (m_anchor.getLayer() == layer) {
            double const zoom = cam->getZoom();

            auto const lm = renderbackend->getLightingModel();
            renderbackend->drawLightPrimitive(
                p,
                m_intensity,
                m_radius,
                m_subdivisions,
                static_cast<float>(m_xstretch * zoom),
                static_cast<float>(m_ystretch * zoom),
                m_red,
                m_green,
                m_blue);

            if (m_stencil) {
                renderbackend->changeRenderInfos(
                    RENDER_DATA_WITHOUT_Z, 1, m_src, m_dst, false, true, m_stencil_ref, INCR, GEQUAL);
            } else if (lm == 1) {
                renderbackend->changeRenderInfos(
                    RENDER_DATA_WITHOUT_Z, 1, m_src, m_dst, false, true, 255, KEEP, NOTEQUAL);
            }
        }
    }
    std::vector<uint8_t> LightRendererSimpleLightInfo::getColor()
    {
        std::vector<uint8_t> colors;
        colors.push_back(m_red);
        colors.push_back(m_green);
        colors.push_back(m_blue);
        colors.push_back(m_intensity);
        return colors;
    }

    LightRenderer* LightRenderer::getInstance(IRendererContainer* cnt)
    {
        return dynamic_cast<LightRenderer*>(cnt->getRenderer("LightRenderer"));
    }

    LightRenderer::LightRenderer(RenderBackend* renderbackend, int32_t position) : RendererBase(renderbackend, position)
    {
        setEnabled(false);
    }

    LightRenderer::LightRenderer(LightRenderer const & old) : RendererBase(old)
    {
        setEnabled(false);
    }

    RendererBase* LightRenderer::clone()
    {
        return new LightRenderer(*this);
    }

    LightRenderer::~LightRenderer() = default;

    // Add a static lightmap
    void LightRenderer::addImage(
        std::string const & group, RendererNode const & n, ImagePtr const & image, int32_t src, int32_t dst)
    {
        LightRendererElementInfo* info = new LightRendererImageInfo(n, image, src, dst);
        m_groups[group].push_back(info);
    }

    // Add a animation lightmap
    void LightRenderer::addAnimation(
        std::string const & group, RendererNode const & n, AnimationPtr const & animation, int32_t src, int32_t dst)
    {
        LightRendererElementInfo* info = new LightRendererAnimationInfo(n, animation, src, dst);
        m_groups[group].push_back(info);
    }

    // Add a simple light
    void LightRenderer::addSimpleLight(
        std::string const & group,
        RendererNode const & n,
        uint8_t intensity,
        float radius,
        int32_t subdivisions,
        float xstretch,
        float ystretch,
        uint8_t r,
        uint8_t g,
        uint8_t b,
        int32_t src,
        int32_t dst)
    {
        LightRendererElementInfo* info =
            new LightRendererSimpleLightInfo(n, intensity, radius, subdivisions, xstretch, ystretch, r, g, b, src, dst);
        m_groups[group].push_back(info);
    }

    // Resize an Image
    void LightRenderer::resizeImage(
        std::string const & group,
        RendererNode const & n,
        ImagePtr const & image,
        int32_t width,
        int32_t height,
        int32_t src,
        int32_t dst)
    {
        LightRendererElementInfo* info = new LightRendererResizeInfo(n, image, width, height, src, dst);
        m_groups[group].push_back(info);
    }

    // Enable stencil test for the group
    void LightRenderer::addStencilTest(std::string const & group, uint8_t stencil_ref)
    {
        auto info_it = m_groups[group].begin();
        for (; info_it != m_groups[group].end(); ++info_it) {
            (*info_it)->setStencil(stencil_ref);
        }
    }

    // Disable stencil test for the group
    void LightRenderer::removeStencilTest(std::string const & group)
    {
        auto info_it = m_groups[group].begin();
        for (; info_it != m_groups[group].end(); ++info_it) {
            (*info_it)->removeStencil();
        }
    }

    // Return a list of all groups
    std::list<std::string> LightRenderer::getGroups()
    {
        std::list<std::string> groups;
        auto group_it = m_groups.begin();
        for (; group_it != m_groups.end(); ++group_it) {
            groups.push_back(group_it->first);
        }
        groups.sort();
        groups.unique();
        return groups;
    }

    // Return a vector of all LightElementInfos
    std::vector<LightRendererElementInfo*> LightRenderer::getLightInfo(std::string const & group)
    {
        std::vector<LightRendererElementInfo*> info;
        auto info_it = m_groups[group].begin();
        for (; info_it != m_groups[group].end(); ++info_it) {
            info.push_back(*info_it);
        }
        return info;
    }

    // Remove the group
    void LightRenderer::removeAll(std::string const & group)
    {
        auto info_it = m_groups[group].begin();
        for (; info_it != m_groups[group].end(); ++info_it) {
            delete *info_it;
        }
        m_groups[group].clear();
        m_groups.erase(group);
    }

    // Remove all groups
    void LightRenderer::removeAll()
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
    void LightRenderer::reset()
    {
        removeAll();
    }

    // Render
    void LightRenderer::render(Camera* cam, Layer* layer, RenderList& instances)
    {
        auto const lm = m_renderbackend->getLightingModel();

        if (!layer->areInstancesVisible()) {
            return;
        }

        auto group_it = m_groups.begin();
        for (; group_it != m_groups.end(); ++group_it) {
            auto info_it = group_it->second.begin();
            for (; info_it != group_it->second.end(); ++info_it) {
                if (lm != 0) {
                    if ((*info_it)->getStencil() != -1 && (*info_it)->getStencil() < 255) {
                        if (info_it != group_it->second.begin()) {
                            (*info_it)->setStencil((*info_it)->getStencil() + 1);
                        }
                    }
                }
                (*info_it)->render(cam, layer, instances, m_renderbackend);
            }
        }
    }

} // namespace FIFE
