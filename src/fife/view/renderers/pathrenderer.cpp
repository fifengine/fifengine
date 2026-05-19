// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "pathrenderer.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"
#include "pathfinder/route.h"
#include "util/log/logger.h"
#include "video/renderbackend.h"
#include "view/camera.h"

namespace FIFE
{
    namespace
    {
        Logger& _log = []() -> Logger& {
            static Logger log(LM_VIEWVIEW);
            return log;
        }();
    } // namespace

    PathRenderer::PathRenderer(RenderBackend* renderbackend, int32_t position) : RendererBase(renderbackend, position)
    {
        setEnabled(false);
    }

    PathRenderer::PathRenderer(PathRenderer const & old) : RendererBase(old), m_configs(old.m_configs)
    {
        setEnabled(false);
    }

    RendererBase* PathRenderer::clone()
    {
        return new PathRenderer(*this);
    }

    PathRenderer::~PathRenderer() = default;

    PathRenderer* PathRenderer::getInstance(IRendererContainer* cnt)
    {
        return dynamic_cast<PathRenderer*>(cnt->getRenderer("PathRenderer"));
    }

    void PathRenderer::addPath(Instance* inst, PathStyleConfig const & config)
    {
        assert("inst must not be null" && inst);
        if (!inst)
            return;
        m_configs[inst] = config;
        m_cache[inst]   = CachedPath{};
    }

    void PathRenderer::removePath(Instance* inst)
    {
        assert("inst must not be null" && inst);
        m_configs.erase(inst);
        m_cache.erase(inst);
    }

    void PathRenderer::clearPaths()
    {
        m_configs.clear();
        m_cache.clear();
    }

    void PathRenderer::setPathStyle(Instance* inst, PathStyleConfig const & config)
    {
        assert("inst must not be null" && inst);
        auto it = m_configs.find(inst);
        if (it != m_configs.end()) {
            it->second = config;
        }
    }

    bool PathRenderer::isPointInView(Point3D const & pt, Camera* cam)
    {
        assert("cam must not be null" && cam);
        Rect const & vp       = cam->getViewPort();
        int32_t const padding = 20;
        return pt.x >= vp.x - padding && pt.x <= vp.x + vp.w + padding && pt.y >= vp.y - padding &&
               pt.y <= vp.y + vp.h + padding;
    }

    void PathRenderer::updateCache(Instance* inst, Camera* cam, Layer* layer)
    {
        assert("inst must not be null" && inst);
        assert("cam must not be null" && cam);
        assert("layer must not be null" && layer);
        Route* route = inst->getRoute();
        if (!route) {
            m_cache[inst].dirty = false;
            return;
        }

        auto& cache = m_cache[inst];
        if (!cache.dirty && cache.routeSessionId == route->getSessionId())
            return;

        cache.screenPts.clear();
        Path const & path = route->getPath();
        CellGrid* cg      = layer->getCellGrid();
        if (!cg) {
            FL_WARN(_log, "No cellgrid assigned to layer, cannot draw path");
            cache.dirty = false;
            return;
        }
        cache.screenPts.reserve(path.size());

        for (auto const & loc : path) {
            if (loc.getLayer() != layer)
                continue;

            ExactModelCoordinate const mapCenter = cg->toMapCoordinates(loc.getLayerCoordinates());
            Point3D const screenPt               = cam->toScreenCoordinates(mapCenter);

            if (!isPointInView(screenPt, cam))
                continue;

            cache.screenPts.push_back(screenPt);
        }

        cache.routeSessionId = route->getSessionId();
        cache.dirty          = false;
    }

    void PathRenderer::render(Camera* cam, Layer* layer, [[maybe_unused]] RenderList& instances)
    {
        assert("cam must not be null" && cam);
        assert("layer must not be null" && layer);
        for (auto& [inst, cfg] : m_configs) {
            updateCache(inst, cam, layer);
            auto const & cache = m_cache[inst];
            if (cache.screenPts.size() < 2)
                continue;

            switch (cfg.style) {
            case PLS_SOLID:
                drawSolidLine(cam, cache, cfg);
                break;
            case PLS_DASHED:
                drawDashedLine(cam, cache, cfg);
                break;
            case PLS_ARROWED:
                drawSolidLine(cam, cache, cfg);
                drawArrows(cam, cache, cfg);
                break;
            case PLS_GRADIENT:
            case PLS_HEATMAP:
                drawSolidLine(cam, cache, cfg);
                break;
            }
        }
    }

    void PathRenderer::drawSolidLine([[maybe_unused]] Camera* cam, CachedPath const & c, PathStyleConfig const & cfg)
    {
        assert("screen points must not be empty" && !c.screenPts.empty());
        std::vector<Point> pts;
        pts.reserve(c.screenPts.size());
        for (auto const & sp : c.screenPts) {
            pts.emplace_back(sp.x, sp.y);
        }
        m_renderbackend->drawPolyLine(
            pts, static_cast<uint8_t>(cfg.width), cfg.color.r, cfg.color.g, cfg.color.b, cfg.color.a);
    }

    void PathRenderer::drawDashedLine([[maybe_unused]] Camera* cam, CachedPath const & c, PathStyleConfig const & cfg)
    {
        assert("screen points must have at least 2 points" && c.screenPts.size() >= 2);
        float drawLen = 0.0F;
        bool drawing  = true;
        float lastX   = static_cast<float>(c.screenPts.at(0).x);
        float lastY   = static_cast<float>(c.screenPts.at(0).y);

        for (size_t i = 1; i < c.screenPts.size(); ++i) {
            float const nextX = static_cast<float>(c.screenPts[i].x);
            float const nextY = static_cast<float>(c.screenPts[i].y);
            float const segLen =
                static_cast<float>(std::hypot(static_cast<double>(nextX - lastX), static_cast<double>(nextY - lastY)));
            if (segLen < 0.1F) {
                lastX = nextX;
                lastY = nextY;
                continue;
            }

            float remaining = segLen;
            while (remaining > 1.0F) {
                float const step  = drawing ? cfg.dashLength : cfg.dashGap;
                float const draw  = std::min(remaining, step - drawLen);
                float const ratio = draw / segLen;
                float const endX  = lastX + ((nextX - lastX) * ratio);
                float const endY  = lastY + ((nextY - lastY) * ratio);

                if (drawing) {
                    m_renderbackend->drawLine(
                        Point(static_cast<int32_t>(lastX), static_cast<int32_t>(lastY)),
                        Point(static_cast<int32_t>(endX), static_cast<int32_t>(endY)),
                        cfg.color.r,
                        cfg.color.g,
                        cfg.color.b,
                        cfg.color.a);
                }

                lastX += (nextX - lastX) * (draw / segLen);
                lastY += (nextY - lastY) * (draw / segLen);
                drawLen += draw;
                remaining -= draw;
                if (drawLen >= step - 0.01F) {
                    drawLen = 0.0F;
                    drawing = !drawing;
                }
            }
            lastX = nextX;
            lastY = nextY;
        }
    }

    void PathRenderer::drawArrows([[maybe_unused]] Camera* cam, CachedPath const & c, PathStyleConfig const & cfg)
    {
        assert("screen points must have at least 2 points" && c.screenPts.size() >= 2);
        if (c.screenPts.size() < 2)
            return;

        auto drawArrowAt = [&](Point3D const & from, Point3D const & to) {
            float const angle = std::atan2(static_cast<float>(to.y - from.y), static_cast<float>(to.x - from.x));
            float const a     = cfg.arrowSize;
            Point const tip(to.x, to.y);
            Point const left(
                static_cast<int32_t>(static_cast<float>(tip.x) - (a * std::cos(angle - 0.4F))),
                static_cast<int32_t>(static_cast<float>(tip.y) - (a * std::sin(angle - 0.4F))));
            Point const right(
                static_cast<int32_t>(static_cast<float>(tip.x) - (a * std::cos(angle + 0.4F))),
                static_cast<int32_t>(static_cast<float>(tip.y) - (a * std::sin(angle + 0.4F))));
            m_renderbackend->drawTriangle(tip, left, right, cfg.color.r, cfg.color.g, cfg.color.b, cfg.color.a);
        };

        drawArrowAt(c.screenPts.at(c.screenPts.size() - 2), c.screenPts.back());
    }

} // namespace FIFE
