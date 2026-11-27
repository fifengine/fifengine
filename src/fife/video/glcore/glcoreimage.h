// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCOREIMAGE_H
#define FIFE_GLCOREIMAGE_H

#include <array>
#include <cstdint>
#include <string>

#include "platform.h"
#include "video/image.h"

namespace FIFE
{
    class IResourceLoader;

    FIFE_API std::array<float, 4> glcoreAtlasTexCoords(
        Rect const & region, uint32_t width, uint32_t height, bool halfPixel);

    class FIFE_API GLCoreImage final : public Image
    {
        public:
            explicit GLCoreImage(IResourceLoader* loader = nullptr);
            explicit GLCoreImage(std::string const & name, IResourceLoader* loader = nullptr);
            explicit GLCoreImage(SDL_Surface* surface);
            GLCoreImage(std::string const & name, SDL_Surface* surface);
            explicit GLCoreImage(uint8_t const * data, uint32_t width, uint32_t height);
            GLCoreImage(std::string const & name, uint8_t const * data, uint32_t width, uint32_t height);
            ~GLCoreImage() override;

            void invalidate() override;
            void setSurface(SDL_Surface* surface) override;
            void render(Rect const & rect, uint8_t alpha = 255, uint8_t const * rgb = nullptr) override;
            void render(Rect const & rect, ImagePtr const & overlay, uint8_t alpha = 255, uint8_t const * rgb = nullptr)
                override;
            void renderZ(Rect const & rect, float, uint8_t alpha = 255, uint8_t const * rgb = nullptr) override;
            void updateTexture(uint8_t const * rgba, uint32_t width, uint32_t height) override;
            void useSharedImage(ImagePtr const & shared, Rect const & region) override;
            void forceLoadInternal() override;
            void load() override;
            void free() override;

            uint32_t texture() const
            {
                return m_texture;
            }

            std::array<float, 4> const & texCoords() const
            {
                return m_texCoords;
            }

        private:
            void upload();

            uint32_t m_texture{0};
            std::array<float, 4> m_texCoords{0.0F, 0.0F, 1.0F, 1.0F};
            ImagePtr m_sharedImage;
            Rect m_region{};
            bool m_isShared{false};
    };
} // namespace FIFE

#endif
