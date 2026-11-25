// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "glcoreimage.h"

#include <string>

#include "video/glcore/glad/glcore_glad.h"
#include "video/glcore/glcore_bridge.h"

namespace FIFE
{
    std::array<float, 4> glcoreAtlasTexCoords(Rect const & region, uint32_t width, uint32_t height, bool halfPixel)
    {
        float const inset = halfPixel ? 0.5F : 0.0F;
        float const x0    = static_cast<float>(region.x) + inset;
        float const y0    = static_cast<float>(region.y) + inset;
        float const x1    = static_cast<float>(region.x + region.w) - inset;
        float const y1    = static_cast<float>(region.y + region.h) - inset;
        float const w     = static_cast<float>(width);
        float const h     = static_cast<float>(height);
        return {x0 / w, y0 / h, x1 / w, y1 / h};
    }

    GLCoreImage::GLCoreImage(IResourceLoader* loader) : Image(loader)
    {
    }

    GLCoreImage::GLCoreImage(std::string const & name, IResourceLoader* loader) : Image(name, loader)
    {
    }

    GLCoreImage::GLCoreImage(SDL_Surface* surface) : Image(surface)
    {
        upload();
    }

    GLCoreImage::GLCoreImage(std::string const & name, SDL_Surface* surface) : Image(name, surface)
    {
        upload();
    }

    GLCoreImage::GLCoreImage(uint8_t const * data, uint32_t width, uint32_t height) : Image(data, width, height)
    {
        upload();
    }

    GLCoreImage::GLCoreImage(std::string const & name, uint8_t const * data, uint32_t width, uint32_t height) :
        Image(name, data, width, height)
    {
        upload();
    }

    GLCoreImage::~GLCoreImage()
    {
        if (m_texture != 0) {
            glDeleteTextures(1, &m_texture);
        }
    }

    void GLCoreImage::upload()
    {
        if (m_isShared || m_texture != 0) {
            return;
        }
        SDL_Surface* surface = m_surface;
        if (surface == nullptr || surface->pixels == nullptr) {
            return;
        }
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    }

    void GLCoreImage::invalidate()
    {
        if (m_texture != 0 && !m_isShared) {
            glDeleteTextures(1, &m_texture);
            m_texture = 0;
        }
        upload();
    }

    void GLCoreImage::setSurface(SDL_Surface* surface)
    {
        if (m_texture != 0 && !m_isShared) {
            glDeleteTextures(1, &m_texture);
            m_texture = 0;
        }
        reset(surface);
        upload();
    }

    void GLCoreImage::render(Rect const & rect, uint8_t alpha, uint8_t const * rgb)
    {
        forceLoadInternal();
        if (m_texture == 0) {
            return;
        }
        glcoreAddImageToArray(m_texture, rect, m_texCoords.data(), alpha, rgb);
    }

    void GLCoreImage::render(Rect const & rect, ImagePtr const & overlay, uint8_t alpha, uint8_t const * rgb)
    {
        forceLoadInternal();
        if (m_texture == 0 || !overlay) {
            render(rect, alpha, rgb);
            return;
        }
        auto const * other = dynamic_cast<GLCoreImage const *>(overlay.get());
        if (other == nullptr || other->texture() == 0) {
            render(rect, alpha, rgb);
            return;
        }
        glcoreAddImageToArrayOverlay(
            rect, m_texture, m_texCoords.data(), other->texture(), other->m_texCoords.data(), alpha, rgb);
    }

    void GLCoreImage::renderZ(Rect const & rect, float, uint8_t alpha, uint8_t const * rgb)
    {
        // Z is carried by changeRenderInfos' RenderDataType; the batching
        // contract is otherwise identical to the non-Z path.
        render(rect, alpha, rgb);
    }

    void GLCoreImage::updateTexture(uint8_t const * rgba, uint32_t width, uint32_t height)
    {
        if (m_isShared) {
            return;
        }
        if (m_texture == 0) {
            glGenTextures(1, &m_texture);
        }
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            static_cast<GLsizei>(width),
            static_cast<GLsizei>(height),
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            rgba);
    }

    void GLCoreImage::useSharedImage(ImagePtr const & shared, Rect const & region)
    {
        m_shared            = shared;
        m_region            = region;
        m_isShared          = true;
        auto const * source = dynamic_cast<GLCoreImage const *>(shared.get());
        if (source == nullptr) {
            return;
        }
        m_texture = source->texture();
        // Divide by the full atlas dimensions, not the sub-image size.
        m_texCoords = glcoreAtlasTexCoords(
            region, static_cast<uint32_t>(source->getWidth()), static_cast<uint32_t>(source->getHeight()), true);
    }

    void GLCoreImage::forceLoadInternal()
    {
        if (!m_isShared && m_texture == 0) {
            upload();
        }
    }

    void GLCoreImage::load()
    {
        Image::load();
        forceLoadInternal();
    }

    void GLCoreImage::free()
    {
        if (m_texture != 0 && !m_isShared) {
            glDeleteTextures(1, &m_texture);
            m_texture = 0;
        }
        Image::free();
    }
} // namespace FIFE
