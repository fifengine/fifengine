// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "glimage.h"

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <span>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "util/log/logger.h"
#include "util/structures/rect.h"
#include "video/imagemanager.h"
#include "video/opengl/renderbackendopengl.h"
#include "video/renderbackend.h"
#include "video/sdl/sdlimage.h"

namespace FIFE
{
    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_VIDEO);
            return log;
        }

        Logger& _guiLog()
        {
            static Logger log(LM_GUI);
            return log;
        }

        bool shouldLogGuiLikeSurface(SDL_Surface const * surface)
        {
            if (surface == nullptr) {
                return false;
            }
            return surface->w >= 20 && surface->w <= 500 && surface->h >= 8 && surface->h <= 80;
        }

        bool shouldLogTargetRect(Rect const & rect)
        {
            return (rect.w == 33 && rect.h == 16) || (rect.w == 39 && rect.h == 16);
        }

        std::string summarizeSurfaceSamples(SDL_Surface* surface)
        {
            if (surface == nullptr || surface->pixels == nullptr || surface->w <= 0 || surface->h <= 0) {
                return "samples=unavailable";
            }

            SDL_PixelFormatDetails const * fmt = SDL_GetPixelFormatDetails(surface->format);
            auto const sampleAt                = [&](int32_t x, int32_t y) {
                int32_t const clampedX = std::clamp(x, 0, surface->w - 1);
                int32_t const clampedY = std::clamp(y, 0, surface->h - 1);
                auto const * pixels    = static_cast<uint32_t const *>(surface->pixels);
                int32_t const pitchPx  = surface->pitch / 4;
                uint32_t const pixel =
                    pixels[(clampedY * pitchPx) + clampedX]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                uint8_t r = 0;
                uint8_t g = 0;
                uint8_t b = 0;
                uint8_t a = 0;
                SDL_GetRGBA(pixel, fmt, SDL_GetSurfacePalette(surface), &r, &g, &b, &a);
                return std::format("({},{})={:#010x}/rgba({},{},{},{})", clampedX, clampedY, pixel, r, g, b, a);
            };

            return std::format(
                "{} {} {}",
                sampleAt(0, 0),
                sampleAt(surface->w / 4, surface->h / 2),
                sampleAt(surface->w / 2, surface->h / 2));
        }

        std::string summarizeSurfaceAlphaCoverage(SDL_Surface* surface)
        {
            if (surface == nullptr || surface->pixels == nullptr || surface->w <= 0 || surface->h <= 0) {
                return "alpha=unavailable";
            }

            SDL_PixelFormatDetails const * fmt = SDL_GetPixelFormatDetails(surface->format);
            auto const * pixels                = static_cast<uint32_t const *>(surface->pixels);
            int32_t const pitchPx              = surface->pitch / 4;

            int32_t nonZeroAlpha = 0;
            int32_t fullAlpha    = 0;
            int32_t minX         = surface->w;
            int32_t minY         = surface->h;
            int32_t maxX         = -1;
            int32_t maxY         = -1;

            for (int32_t y = 0; y < surface->h; ++y) {
                for (int32_t x = 0; x < surface->w; ++x) {
                    uint8_t r = 0;
                    uint8_t g = 0;
                    uint8_t b = 0;
                    uint8_t a = 0;
                    SDL_GetRGBA(
                        pixels[(y * pitchPx) + x],
                        fmt,
                        SDL_GetSurfacePalette(surface),
                        &r,
                        &g,
                        &b,
                        &a); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

                    if (a == 0) {
                        continue;
                    }

                    ++nonZeroAlpha;
                    if (a == 255) {
                        ++fullAlpha;
                    }
                    minX = std::min(minX, x);
                    minY = std::min(minY, y);
                    maxX = std::max(maxX, x);
                    maxY = std::max(maxY, y);
                }
            }

            if (nonZeroAlpha == 0) {
                return "alpha=all-zero";
            }

            return std::format(
                "alpha=nonZero:{}/{} full:{} bounds=({},{})->({},{})",
                nonZeroAlpha,
                surface->w * surface->h,
                fullAlpha,
                minX,
                minY,
                maxX,
                maxY);
        }
    } // namespace

    GLImage::GLImage(IResourceLoader* loader) :
        Image(loader),
        m_tex_coords{0, 0, 0, 0},
        m_shared_img(nullptr),
        m_texId(0),
        m_chunk_size_w(0),
        m_chunk_size_h(0),
        m_colorkey{.r = 0, .g = 0, .b = 0, .a = 0},
        m_compressed(false)
    {
        resetGlimage();
    }

    GLImage::GLImage(std::string const & name, IResourceLoader* loader) :
        Image(name, loader),
        m_tex_coords{0, 0, 0, 0},
        m_shared_img(nullptr),
        m_texId(0),
        m_chunk_size_w(0),
        m_chunk_size_h(0),
        m_colorkey{.r = 0, .g = 0, .b = 0, .a = 0},
        m_compressed(false)
    {
        resetGlimage();
    }

    GLImage::GLImage(SDL_Surface* surface) :
        Image(surface),
        m_tex_coords{0, 0, 0, 0},
        m_shared_img(nullptr),
        m_texId(0),
        m_chunk_size_w(0),
        m_chunk_size_h(0),
        m_colorkey{.r = 0, .g = 0, .b = 0, .a = 0},
        m_compressed(false)
    {
        resetGlimage();
    }

    GLImage::GLImage(std::string const & name, SDL_Surface* surface) :
        Image(name, surface),
        m_tex_coords{0, 0, 0, 0},
        m_shared_img(nullptr),
        m_texId(0),
        m_chunk_size_w(0),
        m_chunk_size_h(0),
        m_colorkey{.r = 0, .g = 0, .b = 0, .a = 0},
        m_compressed(false)
    {
        resetGlimage();
    }

    GLImage::GLImage(uint8_t const * data, uint32_t width, uint32_t height) :
        Image(data, width, height),
        m_tex_coords{0, 0, 0, 0},
        m_shared_img(nullptr),
        m_texId(0),
        m_chunk_size_w(0),
        m_chunk_size_h(0),
        m_colorkey{.r = 0, .g = 0, .b = 0, .a = 0},
        m_compressed(false)
    {

        assert(m_surface);
        resetGlimage();
    }

    GLImage::GLImage(std::string const & name, uint8_t const * data, uint32_t width, uint32_t height) :
        Image(name, data, width, height),
        m_tex_coords{0, 0, 0, 0},
        m_shared_img(nullptr),
        m_texId(0),
        m_chunk_size_w(0),
        m_chunk_size_h(0),
        m_colorkey{.r = 0, .g = 0, .b = 0, .a = 0},
        m_compressed(false)
    {

        assert(m_surface);
        resetGlimage();
    }

    GLImage::~GLImage()
    {
        cleanup();
    }

    void GLImage::invalidate()
    {
        resetGlimage();
    }

    void GLImage::setSurface(SDL_Surface* surface)
    {
        reset(surface);
        resetGlimage();
    }

    void GLImage::resetGlimage()
    {
        cleanup();

        m_chunk_size_w = 0;
        m_chunk_size_h = 0;

        m_colorkey = RenderBackend::instance()->getColorKey();
    }

    void GLImage::cleanup()
    {
        if (m_texId != 0U) {
            if (!m_shared) {
                glDeleteTextures(1, &m_texId);
            }
            m_texId      = 0;
            m_compressed = false;
        }

        m_tex_coords[0] = m_tex_coords[1] = m_tex_coords[2] = m_tex_coords[3] = 0.0F;
    }

    void GLImage::render(Rect const & rect, uint8_t alpha, uint8_t const * rgb)
    {
        // completely transparent so dont bother rendering
        if (0 == alpha) {
            return;
        }
        RenderBackend* rb          = RenderBackend::instance();
        SDL_Surface const * target = rb->getRenderTargetSurface();
        assert(target != m_surface); // can't draw on the source surface

        // not on the screen.  dont render
        if (rect.right() < 0 || rect.x > static_cast<int32_t>(target->w) || rect.bottom() < 0 ||
            rect.y > static_cast<int32_t>(target->h)) {
            return;
        }
        if (m_texId == 0U) {
            generateGLTexture();
        } else if (m_shared) {
            validateShared();
        }

        /*if (shouldLogTargetRect(rect)) {
            FL_LOG(
                _guiLog(),
                std::format(
                    "GLImage::render tex={} rect=({},{} {}x{}) alpha={} rgb={} texCoords=({:.3f},{:.3f},{:.3f},{:.3f}) "
                    "surf={}x{} fmt={:#x}",
                    m_texId,
                    rect.x,
                    rect.y,
                    rect.w,
                    rect.h,
                    alpha,
                    rgb != nullptr ? std::format("{},{},{},{}", rgb[0], rgb[1], rgb[2], rgb[3]) :
                                     std::string("null"), // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                    m_tex_coords[0],
                    m_tex_coords[1],
                    m_tex_coords[2],
                    m_tex_coords[3],
                    m_surface != nullptr ? m_surface->w : 0,
                    m_surface != nullptr ? m_surface->h : 0,
                    m_surface != nullptr ? static_cast<unsigned>(m_surface->format) : 0U));
        } else if (shouldLogGuiLikeSurface(m_surface)) {
            FL_WARN(
                _log(),
                std::format(
                    "GLImage::render tex={} rect=({},{} {}x{}) alpha={} rgb={} texCoords=({:.3f},{:.3f},{:.3f},{:.3f}) "
                    "surf={}x{} fmt={:#x}",
                    m_texId,
                    rect.x,
                    rect.y,
                    rect.w,
                    rect.h,
                    alpha,
                    rgb != nullptr ? std::format("{},{},{},{}", rgb[0], rgb[1], rgb[2], rgb[3]) :
                                     std::string("null"), // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                    m_tex_coords[0],
                    m_tex_coords[1],
                    m_tex_coords[2],
                    m_tex_coords[3],
                    m_surface != nullptr ? m_surface->w : 0,
                    m_surface != nullptr ? m_surface->h : 0,
                    m_surface != nullptr ? static_cast<unsigned>(m_surface->format) : 0U));
        }*/

        rb->addImageToArray(m_texId, rect, &m_tex_coords[0], alpha, rgb);
    }

    void GLImage::render(Rect const & rect, ImagePtr const & overlay, uint8_t alpha, uint8_t const * rgb)
    {
        // completely transparent so dont bother rendering
        if (0 == alpha) {
            return;
        }
        RenderBackend* rb          = RenderBackend::instance();
        SDL_Surface const * target = rb->getRenderTargetSurface();
        assert(target != m_surface); // can't draw on the source surface

        // not on the screen.  dont render
        if (rect.right() < 0 || rect.x > static_cast<int32_t>(target->w) || rect.bottom() < 0 ||
            rect.y > static_cast<int32_t>(target->h)) {
            return;
        }
        if (m_texId == 0U) {
            generateGLTexture();
        } else if (m_shared) {
            validateShared();
        }

        auto* img = dynamic_cast<GLImage*>(overlay.get());
        img->forceLoadInternal();

        dynamic_cast<RenderBackendOpenGL*>(rb)->addImageToArray(
            rect, m_texId, &m_tex_coords[0], img->getTexId(), img->getTexCoords(), alpha, rgb);
        // rb->addImageToArray(rect, m_texId, m_tex_coords, img->getTexId(), img->getTexCoords(), alpha, rgb);
    }

    void GLImage::renderZ(Rect const & rect, float vertexZ, uint8_t alpha, uint8_t const * rgb)
    {
        // completely transparent so dont bother rendering
        if (0 == alpha) {
            return;
        }
        RenderBackend* rb          = RenderBackend::instance();
        SDL_Surface const * target = rb->getRenderTargetSurface();
        assert(target != m_surface); // can't draw on the source surface

        // not on the screen.  dont render
        if (rect.right() < 0 || rect.x > static_cast<int32_t>(target->w) || rect.bottom() < 0 ||
            rect.y > static_cast<int32_t>(target->h)) {
            return;
        }
        if (m_texId == 0U) {
            generateGLTexture();
        } else if (m_shared) {
            validateShared();
        }
        dynamic_cast<RenderBackendOpenGL*>(rb)->addImageToArrayZ(m_texId, rect, vertexZ, &m_tex_coords[0], alpha, rgb);
        // rb->addImageToArray(m_texId, rect, m_tex_coords, alpha, rgb);
    }

    void GLImage::renderZ(
        Rect const & rect, float vertexZ, ImagePtr const & overlay, uint8_t alpha, uint8_t const * rgb)
    {
        // completely transparent so dont bother rendering
        if (0 == alpha) {
            return;
        }
        RenderBackend* rb          = RenderBackend::instance();
        SDL_Surface const * target = rb->getRenderTargetSurface();
        assert(target != m_surface); // can't draw on the source surface

        // not on the screen.  dont render
        if (rect.right() < 0 || rect.x > static_cast<int32_t>(target->w) || rect.bottom() < 0 ||
            rect.y > static_cast<int32_t>(target->h)) {
            return;
        }

        if (m_texId == 0U) {
            generateGLTexture();
        } else if (m_shared) {
            validateShared();
        }

        auto* img = dynamic_cast<GLImage*>(overlay.get());
        img->forceLoadInternal();

        dynamic_cast<RenderBackendOpenGL*>(rb)->addImageToArrayZ(
            rect, vertexZ, m_texId, &m_tex_coords[0], img->getTexId(), img->getTexCoords(), alpha, rgb);
        // rb->addImageToArray(rect, m_texId, m_tex_coords, img->getTexId(), img->getTexCoords(), alpha, rgb);
    }

    void GLImage::generateGLTexture()
    {
        if (m_shared) {
            // First make sure we loaded big image to opengl
            validateShared();
            return;
        }
        // ultimate possibility to load the image
        // is used e.g. in case a cursor or gui image is freed even if there is a reference
        if (m_surface == nullptr) {
            if (m_state == IResource::RES_NOT_LOADED) {
                load();
            }
        }
        uint32_t const width  = static_cast<uint32_t>(m_surface->w);
        uint32_t const height = static_cast<uint32_t>(m_surface->h);

        // With OpenGL 2.0 or GL_ARB_texture_non_power_of_two we don't really need to care
        // about non power of 2 textures
        if (GLEW_ARB_texture_non_power_of_two && RenderBackend::instance()->isNPOTEnabled()) {
            m_chunk_size_w = width;
            m_chunk_size_h = height;
        } else {
            // calculate the nearest larger power of 2
            m_chunk_size_w = nextPow2(width);
            m_chunk_size_h = nextPow2(height);
        }

        // used to calculate the fill ratio for given chunk
        m_tex_coords[0] = m_tex_coords[1] = 0.0F;

        m_tex_coords[2] = static_cast<float>(static_cast<uint32_t>(m_surface->w) % m_chunk_size_w) /
                          static_cast<float>(m_chunk_size_w);

        m_tex_coords[3] = static_cast<float>(static_cast<uint32_t>(m_surface->h) % m_chunk_size_h) /
                          static_cast<float>(m_chunk_size_h);

        if (m_tex_coords[2] == 0.0F) {
            m_tex_coords[2] = 1.0F;
        }

        if (m_tex_coords[3] == 0.0F) {
            m_tex_coords[3] = 1.0F;
        }

        std::span<uint8_t> const data_span{
            static_cast<uint8_t*>(m_surface->pixels),
            static_cast<size_t>(height) * static_cast<size_t>(m_surface->pitch)};
        int32_t const pitch = m_surface->pitch;

        /*if ((m_surface->w == 33 && m_surface->h == 16) || (m_surface->w == 39 && m_surface->h == 16)) {
            FL_LOG(
                _guiLog(),
                std::format(
                    "GLImage::generateGLTexture surface={}x{} pitch={} fmt={:#x} targetFmt={:#x} texChunk={}x{} "
                    "texCoords=({:.3f},{:.3f},{:.3f},{:.3f}) {} {}",
                    m_surface->w,
                    m_surface->h,
                    m_surface->pitch,
                    static_cast<unsigned>(m_surface->format),
                    static_cast<unsigned>(RenderBackend::instance()->getPixelFormat()),
                    m_chunk_size_w,
                    m_chunk_size_h,
                    m_tex_coords[0],
                    m_tex_coords[1],
                    m_tex_coords[2],
                    m_tex_coords[3],
                    summarizeSurfaceSamples(m_surface),
                    summarizeSurfaceAlphaCoverage(m_surface)));
        } else if (shouldLogGuiLikeSurface(m_surface)) {
            FL_WARN(
                _log(),
                std::format(
                    "GLImage::generateGLTexture surface={}x{} pitch={} fmt={:#x} targetFmt={:#x} texChunk={}x{} "
                    "texCoords=({:.3f},{:.3f},{:.3f},{:.3f}) {}",
                    m_surface->w,
                    m_surface->h,
                    m_surface->pitch,
                    static_cast<unsigned>(m_surface->format),
                    static_cast<unsigned>(RenderBackend::instance()->getPixelFormat()),
                    m_chunk_size_w,
                    m_chunk_size_h,
                    m_tex_coords[0],
                    m_tex_coords[1],
                    m_tex_coords[2],
                    m_tex_coords[3],
                    summarizeSurfaceSamples(m_surface)));
        }*/

        assert(!m_texId);

        // get texture id from opengl
        glGenTextures(1, &m_texId);
        // set focus on that texture
        dynamic_cast<RenderBackendOpenGL*>(RenderBackend::instance())->bindTexture(m_texId);
        // set filters for texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        bool const mipmapping = RenderBackend::instance()->isMipmappingEnabled();
        if (mipmapping) {
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        }
        // without mipmapping, trilinear and bilinear filters are the same
        switch (RenderBackend::instance()->getTextureFiltering()) {
        case TEXTURE_FILTER_NONE:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapping ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
            break;
        case TEXTURE_FILTER_BILINEAR:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapping ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
            break;
        case TEXTURE_FILTER_TRILINEAR:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapping ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
            break;
        case TEXTURE_FILTER_ANISOTROPIC:
            // currently trilinear anisotropic
            glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MAX_ANISOTROPY_EXT,
                static_cast<GLint>(RenderBackend::instance()->getMaxAnisotropy()));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapping ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
            break;
        default:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapping ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
            break;
        }

        GLint internalFormat = GL_RGBA8;
        if (GLEW_ARB_texture_compression && RenderBackend::instance()->isImageCompressingEnabled()) {
            internalFormat = GL_COMPRESSED_RGBA;
            m_compressed   = true;
        } else {
            m_compressed = false;
        }

        bool const monochrome                  = RenderBackend::instance()->isMonochromeEnabled();
        SDL_PixelFormatDetails const * details = SDL_GetPixelFormatDetails(RenderBackend::instance()->getPixelFormat());
        int32_t const bpp_target               = details->bits_per_pixel;
        int32_t const bpp_source               = SDL_BYTESPERPIXEL(m_surface->format);

        // create 16 bit texture, RGBA_4444
        if (bpp_target == 16 && bpp_source == 32) {
            std::vector<uint16_t> oglbuffer(
                static_cast<size_t>(m_chunk_size_w) * static_cast<size_t>(m_chunk_size_h), 0x00);

            for (uint32_t y = 0; y < height; ++y) {
                for (uint32_t x = 0; x < width; ++x) {
                    size_t const pos =
                        (static_cast<size_t>(y) * static_cast<size_t>(pitch)) + (static_cast<size_t>(x) * 4U);

                    uint8_t r = *(data_span.data() + (pos + 0)); // NOLINT(cppcoreguidelines-init-variables)
                    uint8_t g = *(data_span.data() + (pos + 1)); // NOLINT(cppcoreguidelines-init-variables)
                    uint8_t b = *(data_span.data() + (pos + 2)); // NOLINT(cppcoreguidelines-init-variables)
                    uint8_t a = *(data_span.data() + (pos + 3)); // NOLINT(cppcoreguidelines-init-variables)

                    if (RenderBackend::instance()->isColorKeyEnabled()) {
                        // only set alpha to zero if colorkey feature is enabled
                        if (r == m_colorkey.r && g == m_colorkey.g && b == m_colorkey.b) {
                            a = 0;
                        }
                    }
                    // if monochrome rendering is enabled, then the colors are converted to grayscale
                    if (monochrome) {
                        auto lum = static_cast<uint8_t>((r * 0.3) + (g * 0.59) + (b * 0.11));
                        r        = lum;
                        g        = lum;
                        b        = lum;
                    }
                    oglbuffer.at((static_cast<size_t>(y) * static_cast<size_t>(m_chunk_size_w)) + x) =
                        static_cast<uint16_t>(((r >> 4) << 12) | ((g >> 4) << 8) | ((b >> 4) << 4) | ((a >> 4) << 0));
                }
            }
            // in case of compression we let OpenGL handle it
            if (!m_compressed) {
                internalFormat = GL_RGBA4;
            }

            // transfer data from sdl buffer
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                internalFormat,
                static_cast<GLsizei>(m_chunk_size_w),
                static_cast<GLsizei>(m_chunk_size_h),
                0,
                GL_RGBA,
                GL_UNSIGNED_SHORT_4_4_4_4,
                oglbuffer.data());
            return;
        }

        if (GLEW_ARB_texture_non_power_of_two && RenderBackend::instance()->isNPOTEnabled()) {
            if (RenderBackend::instance()->isColorKeyEnabled()) {
                std::vector<uint8_t> oglbuffer(static_cast<size_t>(width) * static_cast<size_t>(height) * 4U);
                std::memcpy(
                    oglbuffer.data(), data_span.data(), static_cast<size_t>(width) * static_cast<size_t>(height) * 4U);

                for (uint32_t y = 0; y < height; ++y) {
                    for (uint32_t x = 0; x < width * 4; x += 4) {
                        size_t const gid =
                            static_cast<size_t>(x) + (static_cast<size_t>(y) * static_cast<size_t>(pitch));

                        uint8_t const r = oglbuffer.at(gid + 0);
                        uint8_t const g = oglbuffer.at(gid + 1);
                        uint8_t const b = oglbuffer.at(gid + 2);

                        // set alpha to zero
                        if (r == m_colorkey.r && g == m_colorkey.g && b == m_colorkey.b) {
                            oglbuffer.at(gid + 3) = 0;
                        }
                        // if monochrome rendering is enabled, then the colors are converted to grayscale
                        if (monochrome) {
                            auto lum              = static_cast<uint8_t>((r * 0.3) + (g * 0.59) + (b * 0.11));
                            oglbuffer.at(gid + 0) = lum;
                            oglbuffer.at(gid + 1) = lum;
                            oglbuffer.at(gid + 2) = lum;
                        }
                    }
                }

                // transfer data from sdl buffer
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    internalFormat,
                    static_cast<GLsizei>(m_chunk_size_w),
                    static_cast<GLsizei>(m_chunk_size_h),
                    0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    oglbuffer.data());
            } else if (monochrome) {
                std::vector<uint8_t> oglbuffer(static_cast<size_t>(width) * static_cast<size_t>(height) * 4U);
                std::memcpy(
                    oglbuffer.data(), data_span.data(), static_cast<size_t>(width) * static_cast<size_t>(height) * 4U);

                for (uint32_t y = 0; y < height; ++y) {
                    for (uint32_t x = 0; x < width * 4; x += 4) {
                        size_t const gid =
                            static_cast<size_t>(x) + (static_cast<size_t>(y) * static_cast<size_t>(pitch));
                        // if monochrome rendering is enabled, then the colors are converted to grayscale
                        auto lum = static_cast<uint8_t>(
                            (oglbuffer.at(gid + 0) * 0.3) + (oglbuffer.at(gid + 1) * 0.59) +
                            (oglbuffer.at(gid + 2) * 0.11));
                        oglbuffer.at(gid + 0) = lum;
                        oglbuffer.at(gid + 1) = lum;
                        oglbuffer.at(gid + 2) = lum;
                    }
                }

                // transfer data from sdl buffer
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    internalFormat,
                    static_cast<GLsizei>(m_chunk_size_w),
                    static_cast<GLsizei>(m_chunk_size_h),
                    0,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    oglbuffer.data());
            } else {
                // SDL3 surfaces may have padded row pitch; OpenGL expects contiguous rows when
                // uploading with GL_RGBA/GL_UNSIGNED_BYTE in this path, so repack when needed.
                uint32_t const tightPitch = width * 4U;
                if (std::cmp_equal(pitch, tightPitch)) {
                    // Fast path for tightly packed RGBA rows.
                    glTexImage2D(
                        GL_TEXTURE_2D,
                        0,
                        internalFormat,
                        static_cast<GLsizei>(m_chunk_size_w),
                        static_cast<GLsizei>(m_chunk_size_h),
                        0,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        data_span.data());
                } else {
                    // SDL surfaces can have padded row pitch; repack to a tightly packed buffer.
                    std::vector<uint8_t> packed(static_cast<size_t>(tightPitch) * static_cast<size_t>(height));
                    for (uint32_t y = 0; y < height; ++y) {
                        uint8_t const * srcRow = // NOLINT(cppcoreguidelines-init-variables)
                            data_span.data() + (static_cast<size_t>(y) * static_cast<size_t>(pitch));
                        uint8_t* dstRow = &packed.at(static_cast<size_t>(y) * static_cast<size_t>(tightPitch));
                        std::memcpy(dstRow, srcRow, static_cast<size_t>(tightPitch));
                    }

                    glTexImage2D(
                        GL_TEXTURE_2D,
                        0,
                        internalFormat,
                        static_cast<GLsizei>(m_chunk_size_w),
                        static_cast<GLsizei>(m_chunk_size_h),
                        0,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        packed.data());
                }
            }
            // Non power of 2 textures are not supported, we need to pad the size of texture to nearest power of 2
        } else {
            std::vector<uint32_t> oglbuffer(
                static_cast<size_t>(m_chunk_size_w) * static_cast<size_t>(m_chunk_size_h), 0x00);

            for (uint32_t y = 0; y < height; ++y) {
                for (uint32_t x = 0; x < width; ++x) {
                    size_t const pos =
                        (static_cast<size_t>(y) * static_cast<size_t>(pitch)) + (static_cast<size_t>(x) * 4U);

                    uint8_t a = *(data_span.data() + (pos + 3)); // NOLINT(cppcoreguidelines-init-variables)
                    uint8_t b = *(data_span.data() + (pos + 2)); // NOLINT(cppcoreguidelines-init-variables)
                    uint8_t g = *(data_span.data() + (pos + 1)); // NOLINT(cppcoreguidelines-init-variables)
                    uint8_t r = *(data_span.data() + (pos + 0)); // NOLINT(cppcoreguidelines-init-variables)

                    if (RenderBackend::instance()->isColorKeyEnabled()) {
                        // only set alpha to zero if colorkey feature is enabled
                        if (r == m_colorkey.r && g == m_colorkey.g && b == m_colorkey.b) {
                            a = 0;
                        }
                    }
                    // if monochrome rendering is enabled, then the colors are converted to grayscale
                    if (monochrome) {
                        auto lum = static_cast<uint8_t>((r * 0.3) + (g * 0.59) + (b * 0.11));
                        r        = lum;
                        g        = lum;
                        b        = lum;
                    }

                    oglbuffer.at((static_cast<size_t>(y) * static_cast<size_t>(m_chunk_size_w)) + x) =
                        static_cast<uint32_t>(r | (g << 8) | (b << 16) | (a << 24));
                }
            }

            // transfer data from sdl buffer
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                internalFormat,
                static_cast<GLsizei>(m_chunk_size_w),
                static_cast<GLsizei>(m_chunk_size_h),
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                oglbuffer.data());
        }
    }

    void GLImage::generateGLSharedTexture(GLImage const * shared, Rect const & region)
    {
        uint32_t width  = shared->getWidth();
        uint32_t height = shared->getHeight();

        if (!GLEW_ARB_texture_non_power_of_two || !RenderBackend::instance()->isNPOTEnabled()) {
            width  = nextPow2(width);
            height = nextPow2(height);
        }

        if (RenderBackend::instance()->getTextureFiltering() != TEXTURE_FILTER_NONE ||
            RenderBackend::instance()->isMipmappingEnabled()) {
            // half pixel correction
            m_tex_coords[0] = (static_cast<GLfloat>(region.x) + 0.5F) / static_cast<GLfloat>(width);
            m_tex_coords[1] = (static_cast<GLfloat>(region.y) + 0.5F) / static_cast<GLfloat>(height);
            m_tex_coords[2] = (static_cast<GLfloat>(region.x + region.w) - 0.5F) / static_cast<GLfloat>(width);
            m_tex_coords[3] = (static_cast<GLfloat>(region.y + region.h) - 0.5F) / static_cast<GLfloat>(height);
        } else {
            m_tex_coords[0] = static_cast<GLfloat>(region.x) / static_cast<GLfloat>(width);
            m_tex_coords[1] = static_cast<GLfloat>(region.y) / static_cast<GLfloat>(height);
            m_tex_coords[2] = static_cast<GLfloat>(region.x + region.w) / static_cast<GLfloat>(width);
            m_tex_coords[3] = static_cast<GLfloat>(region.y + region.h) / static_cast<GLfloat>(height);
        }
    }

    void GLImage::useSharedImage(ImagePtr const & shared, Rect const & region)
    {
        auto* img = dynamic_cast<GLImage*>(shared.get());

        m_shared_img   = img;
        m_texId        = img->m_texId;
        m_shared       = true;
        m_subimagerect = region;
        m_atlas_img    = shared;
        m_surface      = m_shared_img->m_surface;
        m_compressed   = m_shared_img->m_compressed;
        m_atlas_name   = m_shared_img->getName();

        if (m_texId != 0U) {
            generateGLSharedTexture(img, region);
        }

        setState(m_shared_img->getState());
    }

    void GLImage::forceLoadInternal()
    {
        if (m_texId == 0) {
            generateGLTexture();
        } else if (m_shared) {
            validateShared();
        }
    }

    void GLImage::validateShared()
    {
        // if image is valid we can return
        if ((m_shared_img->m_texId != 0U) && m_shared_img->m_texId == m_texId) {
            return;
        }

        if (m_shared_img->getState() == IResource::RES_NOT_LOADED) {
            m_shared_img->load();
            m_shared_img->generateGLTexture();
        } else if (m_shared_img->m_texId == 0U) {
            m_shared_img->generateGLTexture();
        }

        m_texId      = m_shared_img->m_texId;
        m_surface    = m_shared_img->m_surface;
        m_compressed = m_shared_img->m_compressed;
        generateGLSharedTexture(m_shared_img, m_subimagerect);
    }

    void GLImage::copySubimage(uint32_t xoffset, uint32_t yoffset, ImagePtr const & img)
    {
        Image::copySubimage(xoffset, yoffset, img);

        if (m_texId != 0U) {
            dynamic_cast<RenderBackendOpenGL*>(RenderBackend::instance())->bindTexture(m_texId);
            glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                static_cast<GLint>(xoffset),
                static_cast<GLint>(yoffset),
                static_cast<GLsizei>(img->getWidth()),
                static_cast<GLsizei>(img->getHeight()),
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                img->getSurface()->pixels);
        }
    }

    void GLImage::load()
    {
        if (m_shared) {
            // check atlas image
            // if it does not exist, it is generated.
            if (!ImageManager::instance()->exists(m_atlas_name)) {
                ImagePtr const newAtlas = ImageManager::instance()->create(m_atlas_name);
                auto* img               = dynamic_cast<GLImage*>(newAtlas.get());
                m_atlas_img             = newAtlas;
                m_shared_img            = img;
            }
            validateShared();
            // check if texture ids and surfaces are identical
            if (m_shared_img->m_surface != m_surface || m_texId != m_shared_img->m_texId) {
                m_texId      = m_shared_img->m_texId;
                m_surface    = m_shared_img->m_surface;
                m_compressed = m_shared_img->m_compressed;
                if (m_texId != 0U) {
                    generateGLSharedTexture(m_shared_img, m_subimagerect);
                }
            }
            m_state = IResource::RES_LOADED;
        } else {
            Image::load();
        }
    }

    void GLImage::free()
    {
        // save the image offsets
        int32_t const xshift = m_xshift;
        int32_t const yshift = m_yshift;
        setSurface(nullptr);
        m_xshift = xshift;
        m_yshift = yshift;
        m_state  = IResource::RES_NOT_LOADED;
    }

    GLuint GLImage::getTexId() const
    {
        return m_texId;
    }

    GLfloat const * GLImage::getTexCoords() const
    {
        return &m_tex_coords[0];
    }
} // namespace FIFE
