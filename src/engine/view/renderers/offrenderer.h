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

    class /*FIFE_API*/ OffRendererElementInfo
    {
    public:
        virtual void render(RenderBackend* renderbackend)
        {
            static_cast<void>(renderbackend);
        }
        virtual ~OffRendererElementInfo() = default;
    };

    class /*FIFE_API*/ OffRendererLineInfo : public OffRendererElementInfo
    {
    public:
        void render(RenderBackend* renderbackend) override;
        OffRendererLineInfo(const Point& n1, const Point& n2, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        ~OffRendererLineInfo() override;

    private:
        Point m_edge1;
        Point m_edge2;
        uint8_t m_red;
        uint8_t m_green;
        uint8_t m_blue;
        uint8_t m_alpha;
    };
    class /*FIFE_API*/ OffRendererPointInfo : public OffRendererElementInfo
    {
    public:
        void render(RenderBackend* renderbackend) override;
        OffRendererPointInfo(const Point& anchor, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        ~OffRendererPointInfo() override;

    private:
        Point m_anchor;
        uint8_t m_red;
        uint8_t m_green;
        uint8_t m_blue;
        uint8_t m_alpha;
    };
    class /*FIFE_API*/ OffRendererTriangleInfo : public OffRendererElementInfo
    {
    public:
        void render(RenderBackend* renderbackend) override;
        OffRendererTriangleInfo(
            const Point& n1, const Point& n2, const Point& n3, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
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
    class /*FIFE_API*/ OffRendererQuadInfo : public OffRendererElementInfo
    {
    public:
        void render(RenderBackend* renderbackend) override;
        OffRendererQuadInfo(
            const Point& n1,
            const Point& n2,
            const Point& n3,
            const Point& n4,
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

    class /*FIFE_API*/ OffRendererVertexInfo : public OffRendererElementInfo
    {
    public:
        void render(RenderBackend* renderbackend) override;
        OffRendererVertexInfo(const Point& center, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        ~OffRendererVertexInfo() override;

    private:
        Point m_center;
        int32_t m_size;
        uint8_t m_red;
        uint8_t m_green;
        uint8_t m_blue;
        uint8_t m_alpha;
    };

    class /*FIFE_API*/ OffRendererImageInfo : public OffRendererElementInfo
    {
    public:
        void render(RenderBackend* renderbackend) override;
        OffRendererImageInfo(const Point& anchor, const ImagePtr& image);
        ~OffRendererImageInfo() override;

    private:
        Point m_anchor;
        ImagePtr m_image;
    };
    class /*FIFE_API*/ OffRendererAnimationInfo : public OffRendererElementInfo
    {
    public:
        void render(RenderBackend* renderbackend) override;
        OffRendererAnimationInfo(const Point& anchor, const AnimationPtr& animation);
        ~OffRendererAnimationInfo() override;

    private:
        Point m_anchor;
        AnimationPtr m_animation;
        uint32_t m_start_time;
        float m_time_scale;
    };
    class /*FIFE_API*/ OffRendererTextInfo : public OffRendererElementInfo
    {
    public:
        void render(RenderBackend* renderbackend) override;
        OffRendererTextInfo(const Point& anchor, IFont* font, std::string text);
        ~OffRendererTextInfo() override;

    private:
        Point m_anchor;
        IFont* m_font;
        std::string m_text;
    };
    class /*FIFE_API*/ OffRendererResizeInfo : public OffRendererElementInfo
    {
    public:
        void render(RenderBackend* renderbackend) override;
        OffRendererResizeInfo(const Point& anchor, const ImagePtr& image, int32_t width, int32_t height);
        ~OffRendererResizeInfo() override;

    private:
        Point m_anchor;
        ImagePtr m_image;
        int32_t m_width;
        int32_t m_height;
    };

    class /*FIFE_API*/ OffRenderer
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
        const Rect& getClipArea() const;
        void render();

        void addLine(
            const std::string& group,
            const Point& n1,
            const Point& n2,
            uint8_t r,
            uint8_t g,
            uint8_t b,
            uint8_t a = 255);
        void addPoint(const std::string& group, const Point& n, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        void addTriangle(
            const std::string& group,
            const Point& n1,
            const Point& n2,
            const Point& n3,
            uint8_t r,
            uint8_t g,
            uint8_t b,
            uint8_t a = 255);
        void addQuad(
            const std::string& group,
            const Point& n1,
            const Point& n2,
            const Point& n3,
            const Point& n4,
            uint8_t r,
            uint8_t g,
            uint8_t b,
            uint8_t a = 255);
        void addVertex(
            const std::string& group, const Point& n, int32_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        void addText(const std::string& group, const Point& n, IFont* font, const std::string& text);
        void addImage(const std::string& group, const Point& n, const ImagePtr& image);
        void addAnimation(const std::string& group, const Point& n, const AnimationPtr& animation);
        void resizeImage(
            const std::string& group, const Point& n, const ImagePtr& image, int32_t width, int32_t height);
        void removeAll(const std::string& group);
        void removeAll();

    private:
        std::map<std::string, std::vector<OffRendererElementInfo*>> m_groups;
        RenderBackend* m_renderbackend;
        bool m_enabled;
        Rect m_area;
    };

} // namespace FIFE

#endif
