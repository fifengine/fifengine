// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_LIGHTRENDERER_H
#define FIFE_LIGHTRENDERER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "model/structures/renderernode.h"
#include "video/animation.h"
#include "view/rendererbase.h"

namespace FIFE
{
    class RenderBackend;
    class IFont;

    class FIFE_API LightRendererElementInfo
    {
        public:
            LightRendererElementInfo(RendererNode const & n, int32_t src, int32_t dst);
            virtual ~LightRendererElementInfo() = default;

            virtual void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) = 0;
            virtual std::string getName()                                                                       = 0;

            RendererNode* getNode()
            {
                return &m_anchor;
            }
            int32_t getSrcBlend() const
            {
                return m_src;
            }
            int32_t getDstBlend() const
            {
                return m_dst;
            }

            void setStencil(uint8_t stencil_ref);
            int32_t getStencil() const;
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

    class FIFE_API LightRendererImageInfo : public LightRendererElementInfo
    {
        public:
            LightRendererImageInfo(RendererNode const & anchor, ImagePtr const & image, int32_t src, int32_t dst);
            ~LightRendererImageInfo() override;

            void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) override;
            std::string getName() override
            {
                return "image";
            }
            ImagePtr getImage()
            {
                return m_image;
            }
            void setImage(ImagePtr const & image)
            {
                m_image = image;
            }

        private:
            ImagePtr m_image;
    };

    class FIFE_API LightRendererAnimationInfo : public LightRendererElementInfo
    {
        public:
            LightRendererAnimationInfo(
                RendererNode const & anchor, AnimationPtr const & animation, int32_t src, int32_t dst);
            ~LightRendererAnimationInfo() override;

            void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) override;
            std::string getName() override
            {
                return "animation";
            }
            AnimationPtr getAnimation()
            {
                return m_animation;
            }
            void setAnimation(AnimationPtr const & animation)
            {
                m_animation = animation;
            }

        private:
            AnimationPtr m_animation;
            uint64_t m_start_time;
            float m_time_scale;
    };

    class FIFE_API LightRendererSimpleLightInfo : public LightRendererElementInfo
    {
        public:
            LightRendererSimpleLightInfo(
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
                int32_t dst);
            ~LightRendererSimpleLightInfo() override;

            void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) override;
            std::string getName() override
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
            void setIntensity(uint8_t intensity)
            {
                m_intensity = intensity;
            }
            void setRadius(float radius)
            {
                m_radius = radius;
            }
            void setSubdivisions(int32_t subdivisions)
            {
                m_subdivisions = subdivisions;
            }
            void setXStretch(float xstretch)
            {
                m_xstretch = xstretch;
            }
            void setYStretch(float ystretch)
            {
                m_ystretch = ystretch;
            }
            void setColor(uint8_t r, uint8_t g, uint8_t b)
            {
                m_red   = r;
                m_green = g;
                m_blue  = b;
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

    class FIFE_API LightRendererResizeInfo : public LightRendererElementInfo
    {
        public:
            LightRendererResizeInfo(
                RendererNode const & anchor,
                ImagePtr const & image,
                int32_t width,
                int32_t height,
                int32_t src,
                int32_t dst);
            ~LightRendererResizeInfo() override;

            void render(Camera* cam, Layer* layer, RenderList& instances, RenderBackend* renderbackend) override;
            std::string getName() override
            {
                return "resize";
            }

            ImagePtr getImage()
            {
                return m_image;
            }
            void setImage(ImagePtr const & image)
            {
                m_image = image;
            }
            int32_t getWidth() const
            {
                return m_width;
            }
            void setWidth(int32_t width)
            {
                m_width = width;
            }
            int32_t getHeight() const
            {
                return m_height;
            }
            void setHeight(int32_t height)
            {
                m_height = height;
            }

        private:
            ImagePtr m_image;
            int32_t m_width;
            int32_t m_height;
    };

    class FIFE_API LightRenderer : public RendererBase
    {
        public:
            /** constructor.
             * @param renderbackend to use
             * @param position position for this renderer in rendering pipeline
             */
            LightRenderer(RenderBackend* renderbackend, int32_t position);

            LightRenderer(LightRenderer const & old);

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

            LightRendererImageInfo* addImage(
                std::string const & group,
                RendererNode const & n,
                ImagePtr const & image,
                int32_t src = -1,
                int32_t dst = -1);
            LightRendererAnimationInfo* addAnimation(
                std::string const & group,
                RendererNode const & n,
                AnimationPtr const & animation,
                int32_t src = -1,
                int32_t dst = -1);
            LightRendererSimpleLightInfo* addSimpleLight(
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
                int32_t src = -1,
                int32_t dst = -1);
            LightRendererResizeInfo* resizeImage(
                std::string const & group,
                RendererNode const & n,
                ImagePtr const & image,
                int32_t width,
                int32_t height,
                int32_t src = -1,
                int32_t dst = -1);
            void addStencilTest(std::string const & group, uint8_t stencil_ref = 0);
            void removeStencilTest(std::string const & group);
            std::list<std::string> getGroups();
            std::vector<LightRendererElementInfo*> getLightInfo(std::string const & group);
            void removeAll(std::string const & group);
            void removeAll();
            void reset();

        private:
            std::map<std::string, std::vector<std::unique_ptr<LightRendererElementInfo>>> m_groups;
    };

} // namespace FIFE

#endif
