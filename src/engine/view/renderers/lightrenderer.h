// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_LIGHTRENDERER_H
#define FIFE_LIGHTRENDERER_H

// Standard C++ library includes
#include <list>
#include <map>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/renderernode.h"
#include "video/animation.h"
#include "view/rendererbase.h"

namespace FIFE
{
    class RenderBackend;
    class IFont;

    class LightRendererElementInfo
    {
    public:
        LightRendererElementInfo(RendererNode n, int32_t src, int32_t dst);
        virtual ~LightRendererElementInfo() { }

        virtual void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) = 0;
        virtual std::string getName()                                                                       = 0;

        RendererNode* getNode()
        {
            return &m_anchor;
        }
        int32_t getSrcBlend()
        {
            return m_src;
        }
        int32_t getDstBlend()
        {
            return m_dst;
        }

        void setStencil(uint8_t stencil_ref);
        int32_t getStencil();
        void removeStencil();

        virtual std::vector<uint8_t> getColor()
        {
            return std::vector<uint8_t>();
        }
        virtual float getRadius()
        {
            return 0;
        }
        virtual int32_t getSubdivisions()
        {
            return 0;
        }
        virtual float getXStretch()
        {
            return 0;
        }
        virtual float getYStretch()
        {
            return 0;
        }

    protected:
        RendererNode m_anchor;
        int32_t m_src;
        int32_t m_dst;
        bool m_stencil;
        uint8_t m_stencil_ref;
    };

    class LightRendererImageInfo : public LightRendererElementInfo
    {
    public:
        LightRendererImageInfo(RendererNode n, ImagePtr image, int32_t src, int32_t dst);
        virtual ~LightRendererImageInfo() override;

        virtual void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) override;
        virtual std::string getName() override
        {
            return "image";
        }
        ImagePtr getImage()
        {
            return m_image;
        }

    private:
        ImagePtr m_image;
    };

    class LightRendererAnimationInfo : public LightRendererElementInfo
    {
    public:
        LightRendererAnimationInfo(RendererNode n, AnimationPtr animation, int32_t src, int32_t dst);
        virtual ~LightRendererAnimationInfo() override;

        virtual void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) override;
        virtual std::string getName() override
        {
            return "animation";
        }
        AnimationPtr getAnimation()
        {
            return m_animation;
        }

    private:
        AnimationPtr m_animation;
        uint32_t m_start_time;
        float m_time_scale;
    };

    class LightRendererSimpleLightInfo : public LightRendererElementInfo
    {
    public:
        LightRendererSimpleLightInfo(
            RendererNode n,
            uint8_t intensity,
            float radius,
            int32_t subdivisions,
            float xstretch,
            float ystretch,
            uint8_t r,
            uint8_t g,
            uint8_t b,
            int32_t src,
            int32_t dst);
        virtual ~LightRendererSimpleLightInfo() override;

        virtual void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) override;
        virtual std::string getName() override
        {
            return "simple";
        }

        std::vector<uint8_t> getColor() override;
        float getRadius() override
        {
            return m_radius;
        }
        int32_t getSubdivisions() override
        {
            return m_subdivisions;
        }
        float getXStretch() override
        {
            return m_xstretch;
        }
        float getYStretch() override
        {
            return m_ystretch;
        }

    private:
        uint8_t m_intensity;
        float m_radius;
        int32_t m_subdivisions;
        float m_xstretch;
        float m_ystretch;
        uint8_t m_red;
        uint8_t m_green;
        uint8_t m_blue;
    };

    class LightRendererResizeInfo : public LightRendererElementInfo
    {
    public:
        LightRendererResizeInfo(
            RendererNode n, ImagePtr image, int32_t width, int32_t height, int32_t src, int32_t dst);
        virtual ~LightRendererResizeInfo() override;

        virtual void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) override;
        virtual std::string getName() override
        {
            return "resize";
        }

        ImagePtr getImage()
        {
            return m_image;
        }

    private:
        ImagePtr m_image;
        int32_t m_width;
        int32_t m_height;
    };

    class LightRenderer : public RendererBase
    {
    public:
        /** constructor.
         * @param renderbackend to use
         * @param position position for this renderer in rendering pipeline
         */
        LightRenderer(RenderBackend* renderbackend, int32_t position);

        LightRenderer(const LightRenderer& old);

        RendererBase* clone();

        /** Destructor.
         */
        virtual ~LightRenderer();
        void render(Camera* cam, Layer* layer, RenderList& instances);
        std::string getName()
        {
            return "LightRenderer";
        }

        /** Gets instance for interface access
         */
        static LightRenderer* getInstance(IRendererContainer* cnt);

        void addImage(const std::string& group, RendererNode n, ImagePtr image, int32_t src = -1, int32_t dst = -1);
        void addAnimation(
            const std::string& group, RendererNode n, AnimationPtr animation, int32_t src = -1, int32_t dst = -1);
        void addSimpleLight(
            const std::string& group,
            RendererNode n,
            uint8_t intensity,
            float radius,
            int32_t subdivisions,
            float xstretch,
            float ystretch,
            uint8_t r,
            uint8_t g,
            uint8_t b,
            int32_t src = -1,
            int32_t dst = -1);
        void resizeImage(
            const std::string& group,
            RendererNode n,
            ImagePtr image,
            int32_t width,
            int32_t height,
            int32_t src = -1,
            int32_t dst = -1);
        void addStencilTest(const std::string& group, uint8_t stencil_ref = 0);
        void removeStencilTest(const std::string& group);
        std::list<std::string> getGroups();
        std::vector<LightRendererElementInfo*> getLightInfo(const std::string& group);
        void removeAll(const std::string& group);
        void removeAll();
        void reset();

    private:
        std::map<std::string, std::vector<LightRendererElementInfo*>> m_groups;
    };

} // namespace FIFE

#endif
