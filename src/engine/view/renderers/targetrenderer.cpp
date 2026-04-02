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
#include "util/log/logger.h"
#include "video/imagemanager.h"
#include "video/renderbackend.h"
#include "view/camera.h"

#include "targetrenderer.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_VIEWVIEW);

    RenderTarget::RenderTarget(RenderBackend* rb, const std::string& name, uint32_t width, uint32_t height) :
        m_renderbackend(rb), m_target(ImageManager::instance()->loadBlank(name, width, height))
    {
    }

    RenderTarget::RenderTarget(RenderBackend* rb, ImagePtr& image) : m_renderbackend(rb), m_target(image) { }

    RenderTarget::~RenderTarget() = default;

    void RenderTarget::addLine(
        const std::string& group, const Point& n1, const Point& n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        OffRendererElementInfo* info = new OffRendererLineInfo(n1, n2, r, g, b, a);
        m_groups[group].push_back(info);
    }

    void RenderTarget::addPoint(const std::string& group, const Point& n, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        OffRendererElementInfo* info = new OffRendererPointInfo(n, r, g, b, a);
        m_groups[group].push_back(info);
    }

    void RenderTarget::addTriangle(
        const std::string& group,
        const Point& n1,
        const Point& n2,
        const Point& n3,
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t a)
    {
        OffRendererElementInfo* info = new OffRendererTriangleInfo(n1, n2, n3, r, g, b, a);
        m_groups[group].push_back(info);
    }

    void RenderTarget::addQuad(
        const std::string& group,
        const Point& n1,
        const Point& n2,
        const Point& n3,
        const Point& n4,
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t a)
    {
        OffRendererElementInfo* info = new OffRendererQuadInfo(n1, n2, n3, n4, r, g, b, a);
        m_groups[group].push_back(info);
    }

    void RenderTarget::addVertex(
        const std::string& group, const Point& n, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        OffRendererElementInfo* info = new OffRendererVertexInfo(n, size, r, g, b, a);
        m_groups[group].push_back(info);
    }

    void RenderTarget::addText(const std::string& group, const Point& n, IFont* font, const std::string& text)
    {
        OffRendererElementInfo* info = new OffRendererTextInfo(n, font, text);
        m_groups[group].push_back(info);
    }

    void RenderTarget::addImage(const std::string& group, const Point& n, const ImagePtr& image)
    {
        OffRendererElementInfo* info = new OffRendererImageInfo(n, image);
        m_groups[group].push_back(info);
    }

    void RenderTarget::addAnimation(const std::string& group, const Point& n, const AnimationPtr& animation)
    {
        OffRendererElementInfo* info = new OffRendererAnimationInfo(n, animation);
        m_groups[group].push_back(info);
    }

    void RenderTarget::resizeImage(
        const std::string& group, const Point& n, const ImagePtr& image, int32_t width, int32_t height)
    {
        OffRendererElementInfo* info = new OffRendererResizeInfo(n, image, width, height);
        m_groups[group].push_back(info);
    }

    void RenderTarget::removeAll(const std::string& group)
    {
        auto info_it = m_groups[group].begin();
        for (; info_it != m_groups[group].end(); ++info_it) {
            delete *info_it;
        }
        m_groups[group].clear();
        m_groups.erase(group);
    }

    void RenderTarget::removeAll()
    {
        m_groups.clear();
    }

    void RenderTarget::render()
    {
        auto group_it = m_groups.begin();
        for (; group_it != m_groups.end(); ++group_it) {
            auto info_it = group_it->second.begin();
            for (; info_it != group_it->second.end(); ++info_it) {
                (*info_it)->render(m_renderbackend);
            }
        }
    }

    TargetRenderer::TargetRenderer(RenderBackend* renderbackend) : m_renderbackend(renderbackend) { }

    TargetRenderer::~TargetRenderer() = default;

    RenderTargetPtr TargetRenderer::createRenderTarget(const std::string& name, uint32_t width, uint32_t height)
    {
        RenderJob rj;
        rj.ndraws        = -1;
        rj.lasttime_draw = 1;
        rj.target        = RenderTargetPtr(new RenderTarget(m_renderbackend, name, width, height));
        rj.discard       = false;

        const std::pair<RenderJobMap::iterator, bool> ret = m_targets.insert(std::make_pair(name, rj));

        return ret.first->second.target;
    }

    RenderTargetPtr TargetRenderer::createRenderTarget(ImagePtr& image)
    {
        RenderJob rj;
        rj.ndraws        = -1;
        rj.lasttime_draw = 1;
        rj.target        = RenderTargetPtr(new RenderTarget(m_renderbackend, image));
        rj.discard       = false;

        const std::pair<RenderJobMap::iterator, bool> ret = m_targets.insert(std::make_pair(image->getName(), rj));

        return ret.first->second.target;
    }

    void TargetRenderer::setRenderTarget(const std::string& targetname, bool discard, int32_t ndraws)
    {
        auto it = m_targets.find(targetname);
        if (it != m_targets.end()) {
            it->second.ndraws  = ndraws;
            it->second.discard = discard;
        }
    }

    void TargetRenderer::render()
    {
        if (!m_targets.empty()) {
            for (auto& m_target : m_targets) {
                if (m_target.second.ndraws != -1) {
                    if (m_target.second.ndraws <= m_target.second.lasttime_draw) {
                        const RenderTargetPtr rt = m_target.second.target;
                        m_renderbackend->attachRenderTarget(rt->m_target, m_target.second.discard);
                        rt->render();
                        m_renderbackend->detachRenderTarget();

                        if (m_target.second.ndraws == 0) {
                            m_target.second.ndraws = -1;
                        } else {
                            m_target.second.lasttime_draw = 1;
                        }
                    } else {
                        ++m_target.second.lasttime_draw;
                    }
                }
            }
        }
    }
} // namespace FIFE
