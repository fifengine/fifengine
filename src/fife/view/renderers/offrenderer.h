// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_OFFRENDERER_H
#define FIFE_OFFRENDERER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <map>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "video/animation.h"

namespace FIFE
{
    class RenderBackend;
    class IFont;

    class FIFE_API OffRendererElementInfo
    {
        public:
            virtual void render(RenderBackend* renderbackend)
            {
                static_cast<void>(renderbackend);
            }
            virtual ~OffRendererElementInfo() = default;
    };

    class FIFE_API OffRendererLineInfo : public OffRendererElementInfo
    {
        public:
            void render(RenderBackend* renderbackend) override;
            OffRendererLineInfo(Point const & n1, Point const & n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
            ~OffRendererLineInfo() override;

        private:
            Point m_edge1;
            Point m_edge2;
            uint8_t m_red;
            uint8_t m_green;
            uint8_t m_blue;
            uint8_t m_alpha;
    };
    class FIFE_API OffRendererPointInfo : public OffRendererElementInfo
    {
        public:
            void render(RenderBackend* renderbackend) override;
            OffRendererPointInfo(Point const & anchor, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
            ~OffRendererPointInfo() override;

        private:
            Point m_anchor;
            uint8_t m_red;
            uint8_t m_green;
            uint8_t m_blue;
            uint8_t m_alpha;
    };
    class FIFE_API OffRendererTriangleInfo : public OffRendererElementInfo
    {
        public:
            void render(RenderBackend* renderbackend) override;
            OffRendererTriangleInfo(
                Point const & n1, Point const & n2, Point const & n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
            ~OffRendererTriangleInfo() override;

        private:
            Point m_edge1;
            Point m_edge2;
            Point m_edge3;
            uint8_t m_red;
            uint8_t m_green;
            uint8_t m_blue;
            uint8_t m_alpha;
    };
    class FIFE_API OffRendererQuadInfo : public OffRendererElementInfo
    {
        public:
            void render(RenderBackend* renderbackend) override;
            OffRendererQuadInfo(
                Point const & n1,
                Point const & n2,
                Point const & n3,
                Point const & n4,
                uint8_t r,
                uint8_t g,
                uint8_t b,
                uint8_t a);
            ~OffRendererQuadInfo() override;

        private:
            Point m_edge1;
            Point m_edge2;
            Point m_edge3;
            Point m_edge4;
            uint8_t m_red;
            uint8_t m_green;
            uint8_t m_blue;
            uint8_t m_alpha;
    };

    class FIFE_API OffRendererVertexInfo : public OffRendererElementInfo
    {
        public:
            void render(RenderBackend* renderbackend) override;
            OffRendererVertexInfo(Point const & center, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
            ~OffRendererVertexInfo() override;

        private:
            Point m_center;
            int32_t m_size;
            uint8_t m_red;
            uint8_t m_green;
            uint8_t m_blue;
            uint8_t m_alpha;
    };

    class FIFE_API OffRendererImageInfo : public OffRendererElementInfo
    {
        public:
            void render(RenderBackend* renderbackend) override;
            OffRendererImageInfo(Point const & anchor, ImagePtr const & image);
            ~OffRendererImageInfo() override;

        private:
            Point m_anchor;
            ImagePtr m_image;
    };
    class FIFE_API OffRendererAnimationInfo : public OffRendererElementInfo
    {
        public:
            void render(RenderBackend* renderbackend) override;
            OffRendererAnimationInfo(Point const & anchor, AnimationPtr const & animation);
            ~OffRendererAnimationInfo() override;

        private:
            Point m_anchor;
            AnimationPtr m_animation;
            uint32_t m_start_time;
            float m_time_scale;
    };
    class FIFE_API OffRendererTextInfo : public OffRendererElementInfo
    {
        public:
            void render(RenderBackend* renderbackend) override;
            OffRendererTextInfo(Point const & anchor, IFont* font, std::string text);
            ~OffRendererTextInfo() override;

        private:
            Point m_anchor;
            IFont* m_font;
            std::string m_text;
    };
    class FIFE_API OffRendererResizeInfo : public OffRendererElementInfo
    {
        public:
            void render(RenderBackend* renderbackend) override;
            OffRendererResizeInfo(Point const & anchor, ImagePtr const & image, int32_t width, int32_t height);
            ~OffRendererResizeInfo() override;

        private:
            Point m_anchor;
            ImagePtr m_image;
            int32_t m_width;
            int32_t m_height;
    };

    class FIFE_API OffRenderer
    {
        public:
            /** constructor.
             * @param renderbackend to use
             */
            explicit OffRenderer(RenderBackend* renderbackend);

            /** Destructor.
             */
            virtual ~OffRenderer();

            void setEnabled(bool enabled);
            bool isEnabled() const;
            void setClipArea(Rect area);
            Rect const & getClipArea() const;
            void render();

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

        private:
            std::map<std::string, std::vector<OffRendererElementInfo*>> m_groups;
            RenderBackend* m_renderbackend;
            bool m_enabled;
            Rect m_area;
    };

} // namespace FIFE

#endif
