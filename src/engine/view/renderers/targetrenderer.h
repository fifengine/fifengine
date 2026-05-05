// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_TARGETRENDERER_H
#define FIFE_TARGETRENDERER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <map>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "view/renderers/offrenderer.h"

namespace FIFE
{

    class /*FIFE_API*/ RenderTarget
    {
            friend class TargetRenderer;

        public:
            /** Destructor.
             */
            ~RenderTarget();

            void addLine(
                std::string const & group,
                Point const & n1,
                Point const & n2,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255);
            void addPoint(std::string const & group, Point const & n, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
            void addTriangle(
                std::string const & group,
                Point const & n1,
                Point const & n2,
                Point const & n3,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255);
            void addQuad(
                std::string const & group,
                Point const & n1,
                Point const & n2,
                Point const & n3,
                Point const & n4,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255);
            void addVertex(
                std::string const & group,
                Point const & n,
                int32_t size,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a = 255);
            void addText(std::string const & group, Point const & n, IFont* font, std::string const & text);
            void addImage(std::string const & group, Point const & n, ImagePtr const & image);
            void addAnimation(std::string const & group, Point const & n, AnimationPtr const & animation);
            void resizeImage(
                std::string const & group, Point const & n, ImagePtr const & image, int32_t width, int32_t height);
            void removeAll(std::string const & group);
            void removeAll();
            void render();

            ImagePtr getTarget()
            {
                return m_target;
            }

        private:
            RenderTarget(RenderBackend* rb, std::string const & name, uint32_t width, uint32_t height);
            RenderTarget(RenderBackend* rb, ImagePtr& image);

            // Non copyable
            RenderTarget(RenderTarget const & rhs);            /* = delete */
            RenderTarget& operator=(RenderTarget const & rhs); /* = delete */

            std::map<std::string, std::vector<OffRendererElementInfo*>> m_groups;
            RenderBackend* m_renderbackend;
            ImagePtr m_target;
    };
    using RenderTargetPtr = SharedPtr<RenderTarget>;
    class /*FIFE_API*/ TargetRenderer
    {
        public:
            /** Constructor.
             * @param renderbackend to use
             */
            explicit TargetRenderer(RenderBackend* renderbackend);

            /** Destructor.
             */
            virtual ~TargetRenderer();

            /** Creates render target
             */
            RenderTargetPtr createRenderTarget(std::string const & name, uint32_t width, uint32_t height);
            RenderTargetPtr createRenderTarget(ImagePtr& image);

            // -1 - dont render
            // 0 - just for the next frame
            // 1 - every frame
            // 2 - every two frames, etc...
            void setRenderTarget(std::string const & targetname, bool discard, int32_t ndraws = 0);
            void render();

        private:
            struct /*FIFE_API*/ RenderJob
            {
                    int32_t ndraws;
                    int32_t lasttime_draw;
                    RenderTargetPtr target;
                    bool discard;
            };
            using RenderJobMap = std::map<std::string, RenderJob>;
            RenderJobMap m_targets;
            RenderBackend* m_renderbackend;
    };

} // namespace FIFE

#endif
