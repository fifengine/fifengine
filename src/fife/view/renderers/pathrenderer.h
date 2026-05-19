// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_PATHRENDERER_H
#define FIFE_PATHRENDERER_H

#include <cstdint>
#include <map>
#include <vector>

#include "model/structures/instance.h"
#include "platform.h"
#include "util/structures/point.h"
#include "view/rendererbase.h"
#include "view/renderitem.h"

namespace FIFE
{
    class Camera;
    class RenderBackend;

    enum PathLineStyle : uint8_t
    {
        PLS_SOLID = 0,
        PLS_DASHED,
        PLS_ARROWED,
        PLS_GRADIENT,
        PLS_HEATMAP
    };

    struct FIFE_API PathStyleConfig
    {
            SDL_Color color{0, 100, 255, 255};
            float width{2.0f};
            PathLineStyle style{PLS_SOLID};
            float dashLength{12.0f};
            float dashGap{6.0f};
            bool drawArrows{false};
            float arrowSize{10.0f};
    };

    class FIFE_API PathRenderer : public RendererBase
    {
        public:
            PathRenderer(RenderBackend* renderbackend, int32_t position = 105);

            PathRenderer(PathRenderer const & old);

            RendererBase* clone() override;

            ~PathRenderer() override;

            std::string getName() override
            {
                return "PathRenderer";
            }

            static PathRenderer* getInstance(IRendererContainer* cnt);

            void render(Camera* cam, Layer* layer, RenderList& instances) override;

            void addPath(Instance* inst, PathStyleConfig const & config);
            void removePath(Instance* inst);
            void clearPaths();
            void setPathStyle(Instance* inst, PathStyleConfig const & config);

        private:
            struct CachedPath
            {
                    std::vector<Point3D> screenPts;
                    int32_t routeSessionId{0};
                    bool dirty{true};
            };

            std::map<Instance*, PathStyleConfig> m_configs;
            std::map<Instance*, CachedPath> m_cache;

            void updateCache(Instance* inst, Camera* cam, Layer* layer);

            bool isPointInView(Point3D const & pt, Camera* cam);

            void drawSolidLine(Camera* cam, CachedPath const & c, PathStyleConfig const & cfg);
            void drawDashedLine(Camera* cam, CachedPath const & c, PathStyleConfig const & cfg);
            void drawArrows(Camera* cam, CachedPath const & c, PathStyleConfig const & cfg);
    };

} // namespace FIFE

#endif
