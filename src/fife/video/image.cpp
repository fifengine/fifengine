// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "image.h"

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <limits>
#include <span>
#include <sstream>
#include <string>

// 3rd party library includes
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

// FIFE includes
#include "loaders/native/video/imageloader.h"
#include "util/resource/resource.h"

namespace FIFE
{
    Image::Image(IResourceLoader* loader) :
        IResource(createUniqueImageName(), loader), m_surface(nullptr), m_xshift(0), m_yshift(0), m_shared(false)
    {
    }

    Image::Image(std::string const & name, IResourceLoader* loader) :
        IResource(name, loader), m_surface(nullptr), m_xshift(0), m_yshift(0), m_shared(false)
    {
    }

    Image::Image(SDL_Surface* surface) :
        IResource(createUniqueImageName(), nullptr), m_surface(surface), m_xshift(0), m_yshift(0), m_shared(false)
    {
    }

    Image::Image(std::string const & name, SDL_Surface* surface) :
        IResource(name, nullptr), m_surface(surface), m_xshift(0), m_yshift(0), m_shared(false)
    {
    }

    Image::Image(uint8_t const * data, uint32_t width, uint32_t height) :
        IResource(createUniqueImageName(), nullptr), m_surface(nullptr), m_xshift(0), m_yshift(0), m_shared(false)
    {
        if (data == nullptr || width == 0 || height == 0) {
            return;
        }

        // Use RGBA32 (ABGR8888 on LE, RGBA8888 on BE) to match the byte order expected by
        // GL_RGBA/GL_UNSIGNED_BYTE texture uploads, since raw pixel data is in R,G,B,A byte order.
        SDL_Surface* surface =
            SDL_CreateSurface(static_cast<int>(width), static_cast<int>(height), SDL_PIXELFORMAT_RGBA32);
        if (surface == nullptr) {
            return;
        }

        SDL_LockSurface(surface);
        std::span<uint8_t const> const src_span(data, static_cast<size_t>(height) * static_cast<size_t>(width) * 4U);
        std::span<uint8_t> const dst_span(
            static_cast<uint8_t*>(surface->pixels),
            static_cast<size_t>(surface->h) * static_cast<size_t>(surface->pitch));
        int const pitch                        = surface->pitch;
        SDL_PixelFormatDetails const * details = SDL_GetPixelFormatDetails(surface->format);
        int const bpp                          = SDL_BYTESPERPIXEL(surface->format);

        for (uint32_t y = 0; y < height; ++y) {
            size_t const y_base = static_cast<size_t>(y) * static_cast<size_t>(pitch);
            for (uint32_t x = 0; x < width; ++x) {
                size_t const idx     = (((static_cast<size_t>(y) * static_cast<size_t>(width)) + x) * 4U);
                uint8_t const r      = src_span[idx + 0]; // NOLINT(cppcoreguidelines-init-variables)
                uint8_t const g      = src_span[idx + 1]; // NOLINT(cppcoreguidelines-init-variables)
                uint8_t const b      = src_span[idx + 2]; // NOLINT(cppcoreguidelines-init-variables)
                uint8_t const a      = src_span[idx + 3]; // NOLINT(cppcoreguidelines-init-variables)
                uint32_t const pixel = SDL_MapRGBA(details, SDL_GetSurfacePalette(surface), r, g, b, a);
                if (bpp == 4) {
                    std::memcpy(&dst_span[y_base + (static_cast<size_t>(x) * 4U)], &pixel, sizeof(pixel));
                } else {
                    std::memcpy(
                        &dst_span[y_base + (static_cast<size_t>(x) * static_cast<size_t>(bpp))],
                        &pixel,
                        static_cast<size_t>(std::min(bpp, 4)));
                }
            }
        }

        SDL_UnlockSurface(surface);
        reset(surface);
    }

    Image::Image(std::string const & name, uint8_t const * data, uint32_t width, uint32_t height) :
        IResource(name, nullptr), m_surface(nullptr), m_xshift(0), m_yshift(0), m_shared(false)
    {
        if (data == nullptr || width == 0 || height == 0) {
            return;
        }

        SDL_Surface* surface =
            SDL_CreateSurface(static_cast<int>(width), static_cast<int>(height), SDL_PIXELFORMAT_RGBA32);
        if (surface == nullptr) {
            return;
        }

        SDL_LockSurface(surface);
        std::span<uint8_t const> const src_span(data, static_cast<size_t>(height) * static_cast<size_t>(width) * 4U);
        std::span<uint8_t> const dst_span(
            static_cast<uint8_t*>(surface->pixels),
            static_cast<size_t>(surface->h) * static_cast<size_t>(surface->pitch));
        int const pitch                        = surface->pitch;
        SDL_PixelFormatDetails const * details = SDL_GetPixelFormatDetails(surface->format);
        int const bpp                          = SDL_BYTESPERPIXEL(surface->format);

        for (uint32_t y = 0; y < height; ++y) {
            size_t const y_base = static_cast<size_t>(y) * static_cast<size_t>(pitch);
            for (uint32_t x = 0; x < width; ++x) {
                size_t const idx     = (((static_cast<size_t>(y) * static_cast<size_t>(width)) + x) * 4U);
                uint8_t const r      = src_span[idx + 0]; // NOLINT(cppcoreguidelines-init-variables)
                uint8_t const g      = src_span[idx + 1]; // NOLINT(cppcoreguidelines-init-variables)
                uint8_t const b      = src_span[idx + 2]; // NOLINT(cppcoreguidelines-init-variables)
                uint8_t const a      = src_span[idx + 3]; // NOLINT(cppcoreguidelines-init-variables)
                uint32_t const pixel = SDL_MapRGBA(details, SDL_GetSurfacePalette(surface), r, g, b, a);
                if (bpp == 4) {
                    std::memcpy(&dst_span[y_base + (static_cast<size_t>(x) * 4U)], &pixel, sizeof(pixel));
                } else {
                    std::memcpy(
                        &dst_span[y_base + (static_cast<size_t>(x) * static_cast<size_t>(bpp))],
                        &pixel,
                        static_cast<size_t>(std::min(bpp, 4)));
                }
            }
        }

        SDL_UnlockSurface(surface);
        reset(surface);
    }

    void Image::reset(SDL_Surface* surface)
    {
        if ((m_surface != nullptr) && !m_shared) {
            SDL_DestroySurface(m_surface);
        }

        m_xshift  = 0;
        m_yshift  = 0;
        m_surface = surface;
    }

    Image::~Image()
    {
        reset(nullptr);
    }

    void Image::load()
    {
        if (m_loader != nullptr) {
            m_loader->load(this);
        } else {
            ImageLoader loader;
            loader.load(this);
        }
        m_state = IResource::RES_LOADED;
    }

    void Image::free()
    {
        // save the image offsets
        int32_t const xshift = m_xshift;
        int32_t const yshift = m_yshift;
        reset(nullptr);
        m_xshift = xshift;
        m_yshift = yshift;
        m_state  = IResource::RES_NOT_LOADED;
    }

    SDL_Surface* Image::detachSurface()
    {
        SDL_Surface* srf = m_surface;
        m_surface        = nullptr;
        return srf;
    }

    uint32_t Image::getWidth() const
    {
        if (m_shared) {
            assert(m_subimagerect.w >= 0);
            return static_cast<uint32_t>(m_subimagerect.w);
        }
        if (m_surface == nullptr) {
            return 0;
        }
        assert(m_surface->w >= 0);
        return static_cast<uint32_t>(m_surface->w);
    }

    uint32_t Image::getHeight() const
    {
        if (m_shared) {
            assert(m_subimagerect.h >= 0);
            return static_cast<uint32_t>(m_subimagerect.h);
        }
        if (m_surface == nullptr) {
            return 0;
        }
        assert(m_surface->h >= 0);
        return static_cast<uint32_t>(m_surface->h);
    }

    size_t Image::getSize()
    {
        if ((m_surface == nullptr) || m_shared) {
            return 0;
        }
        return static_cast<size_t>(m_surface->h) * static_cast<size_t>(m_surface->pitch);
    }

    Rect Image::getArea() const
    {
        uint32_t const width  = getWidth();
        uint32_t const height = getHeight();
        assert(width <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
        assert(height <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
        Rect r(0, 0, static_cast<int32_t>(width), static_cast<int32_t>(height));
        return r;
    }

    void Image::getPixelRGBA(int32_t x, int32_t y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a)
    {
        assert(m_surface);

        SDL_PixelFormatDetails const * const details = SDL_GetPixelFormatDetails(m_surface->format);
        int32_t const bpp                            = SDL_BYTESPERPIXEL(m_surface->format);

        std::span<Uint8 const> const pixels_span(
            static_cast<Uint8 const *>(m_surface->pixels),
            static_cast<size_t>(m_surface->h) * static_cast<size_t>(m_surface->pitch));
        Uint8 const * const p = [&]() -> Uint8 const * {
            if (!isSharedImage()) {
                if ((x < 0) || (x >= m_surface->w) || (y < 0) || (y >= m_surface->h)) {
                    return nullptr;
                }
                return &pixels_span
                    [(static_cast<size_t>(y) * static_cast<size_t>(m_surface->pitch)) +
                     (static_cast<size_t>(x) * static_cast<size_t>(bpp))];
            }
            if ((x < 0) || ((x + m_subimagerect.x) >= m_surface->w) || (y < 0) ||
                ((y + m_subimagerect.y) >= m_surface->h)) {
                return nullptr;
            }
            return &pixels_span
                [(static_cast<size_t>(y + m_subimagerect.y) * static_cast<size_t>(m_surface->pitch)) +
                 (static_cast<size_t>(x + m_subimagerect.x) * static_cast<size_t>(bpp))];
        }();

        if (p == nullptr) {
            (void)r;
            (void)g;
            (void)b;
            (void)a;
            return;
        }

        uint32_t pixel = 0;
        switch (bpp) {
        case 1:
            pixel = *p;
            break;

        case 2: {
            Uint16 pixel16 = 0;
            std::memcpy(&pixel16, p, sizeof(pixel16));
            pixel = pixel16;
        } break;

        case 3: {
            auto const p_span = std::span(p, 3U);
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                pixel = static_cast<uint32_t>(p_span[0] << 16 | p_span[1] << 8 | p_span[2]);
            } else {
                pixel = static_cast<uint32_t>(p_span[0] | p_span[1] << 8 | p_span[2] << 16);
            }
        } break;

        case 4:
            std::memcpy(&pixel, p, sizeof(pixel));
            break;

        default:
            break;
        }
        SDL_GetRGBA(pixel, details, SDL_GetSurfacePalette(m_surface), r, g, b, a);
    }

    void Image::saveImage(std::string const & filename)
    {
        saveAsPng(filename, *m_surface);
    }

    void Image::saveAsPng(std::string const & filename, SDL_Surface& surface)
    {
        if (!IMG_SavePNG(&surface, filename.c_str())) {
            std::cerr << "Failed to save PNG '" << filename << "': " << SDL_GetError() << '\n';
        }
    }

    std::string Image::createUniqueImageName()
    {
        // automated counting for name generation, in case the user doesn't provide a name
        static uint32_t uniqueNumber      = 0;
        static std::string const baseName = "image";

        std::ostringstream oss;
        oss << uniqueNumber << "_" << baseName;

        std::string const name = oss.str();
        ++uniqueNumber;

        return name;
    }

    void Image::copySubimage(uint32_t xoffset, uint32_t yoffset, ImagePtr const & srcimg)
    {
        if (srcimg->m_surface == nullptr) {
            return;
        }
        if (m_surface == nullptr) {
            uint32_t const srcWidth  = srcimg->getWidth();  // NOLINT(cppcoreguidelines-init-variables)
            uint32_t const srcHeight = srcimg->getHeight(); // NOLINT(cppcoreguidelines-init-variables)
            assert(srcWidth <= static_cast<uint32_t>(std::numeric_limits<int>::max()));
            assert(srcHeight <= static_cast<uint32_t>(std::numeric_limits<int>::max()));
            m_surface =
                SDL_CreateSurface(static_cast<int>(srcWidth), static_cast<int>(srcHeight), SDL_PIXELFORMAT_RGBA32);
        }
        // disable blending
        SDL_SetSurfaceBlendMode(srcimg->m_surface, SDL_BLENDMODE_NONE);
        if (this->isSharedImage()) {
            Rect const & rect      = this->getSubImageRect();
            SDL_Rect const dstrect = {
                .x = static_cast<int>(rect.x) + static_cast<int>(xoffset),
                .y = static_cast<int>(rect.y) + static_cast<int>(yoffset),
                .w = static_cast<Uint16>(srcimg->getWidth()),
                .h = static_cast<Uint16>(srcimg->getHeight())};
            if (srcimg->isSharedImage()) {
                Rect const & srcRect   = srcimg->getSubImageRect();
                SDL_Rect const srcrect = {
                    .x = static_cast<Sint16>(srcRect.x),
                    .y = static_cast<Sint16>(srcRect.y),
                    .w = static_cast<Uint16>(srcRect.w),
                    .h = static_cast<Uint16>(srcRect.h)};
                SDL_BlitSurface(srcimg->m_surface, &srcrect, m_surface, &dstrect);
            } else {
                SDL_BlitSurface(srcimg->m_surface, nullptr, m_surface, &dstrect);
            }
        } else {
            SDL_Rect const dstrect = {
                .x = static_cast<Sint16>(xoffset),
                .y = static_cast<Sint16>(yoffset),
                .w = static_cast<Uint16>(srcimg->getWidth()),
                .h = static_cast<Uint16>(srcimg->getHeight())};
            if (srcimg->isSharedImage()) {
                Rect const & rect      = srcimg->getSubImageRect();
                SDL_Rect const srcrect = {
                    .x = static_cast<Sint16>(rect.x),
                    .y = static_cast<Sint16>(rect.y),
                    .w = static_cast<Uint16>(rect.w),
                    .h = static_cast<Uint16>(rect.h)};
                SDL_BlitSurface(srcimg->m_surface, &srcrect, m_surface, &dstrect);
            } else {
                SDL_BlitSurface(srcimg->m_surface, nullptr, m_surface, &dstrect);
            }
        }
        // enable blending
        SDL_SetSurfaceBlendMode(srcimg->m_surface, SDL_BLENDMODE_BLEND);
    }

    bool Image::putPixel(SDL_Surface* surface, int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if ((x < 0) || (x >= surface->w) || (y < 0) || (y >= surface->h)) {
            return false;
        }

        SDL_PixelFormatDetails const * details = SDL_GetPixelFormatDetails(surface->format);
        int32_t const bpp                      = SDL_BYTESPERPIXEL(surface->format);
        SDL_LockSurface(surface);

        std::span<Uint8> const pixels_span(
            static_cast<Uint8*>(surface->pixels),
            static_cast<size_t>(surface->h) * static_cast<size_t>(surface->pitch));
        Uint8* p = &pixels_span // NOLINT(cppcoreguidelines-init-variables)
                       [(static_cast<size_t>(y) * static_cast<size_t>(surface->pitch)) +
                        (static_cast<size_t>(x) * static_cast<size_t>(bpp))];

        Uint32 const pixel = SDL_MapRGBA(details, SDL_GetSurfacePalette(surface), r, g, b, a);
        switch (bpp) {
        case 1:
            *p = static_cast<Uint8>(pixel & 0xffU);
            break;

        case 2: {
            Uint16 const pixel16 = static_cast<Uint16>(pixel);
            std::memcpy(p, &pixel16, sizeof(pixel16));
        } break;

        case 3: {
            auto p_span = std::span(p, 3U);
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p_span[0] = static_cast<uint8_t>((pixel >> 16) & 0xff);
                p_span[1] = static_cast<uint8_t>((pixel >> 8) & 0xff);
                p_span[2] = static_cast<uint8_t>(pixel & 0xff);
            } else {
                p_span[0] = static_cast<uint8_t>(pixel & 0xff);
                p_span[1] = static_cast<uint8_t>((pixel >> 8) & 0xff);
                p_span[2] = static_cast<uint8_t>((pixel >> 16) & 0xff);
            }
        } break;

        case 4:
            std::memcpy(p, &pixel, sizeof(pixel));
            break;

        default:
            break;
        }
        SDL_UnlockSurface(surface);
        return true;
    }
} // namespace FIFE
