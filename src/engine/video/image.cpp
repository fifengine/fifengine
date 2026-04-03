// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "loaders/native/video/imageloader.h"
#include "util/resource/resource.h"

#include "image.h"
#include "png_loader.h"
#include "png_writer.h"

namespace FIFE
{
    Image::Image(IResourceLoader* loader) :
        IResource(createUniqueImageName(), loader), m_surface(nullptr), m_xshift(0), m_yshift(0), m_shared(false)
    {
    }

    Image::Image(const std::string& name, IResourceLoader* loader) :
        IResource(name, loader), m_surface(nullptr), m_xshift(0), m_yshift(0), m_shared(false)
    {
    }

    Image::Image(SDL_Surface* surface) :
        IResource(createUniqueImageName(), nullptr), m_surface(surface), m_xshift(0), m_yshift(0), m_shared(false)
    {
    }

    Image::Image(const std::string& name, SDL_Surface* surface) :
        IResource(name, nullptr), m_surface(surface), m_xshift(0), m_yshift(0), m_shared(false)
    {
    }

    Image::Image(const uint8_t* data, uint32_t width, uint32_t height) :
        IResource(createUniqueImageName(), nullptr), m_surface(nullptr), m_xshift(0), m_yshift(0), m_shared(false)
    {
        if (data == nullptr || width == 0 || height == 0) {
            return;
        }

        SDL_Surface* surface =
            SDL_CreateRGBSurface(0, static_cast<int>(width), static_cast<int>(height), 32, RMASK, GMASK, BMASK, AMASK);
        if (surface == nullptr) {
            return;
        }

        SDL_LockSurface(surface);
        const uint8_t* src = data;
        uint8_t* dst       = static_cast<uint8_t*>(surface->pixels);
        const int pitch    = surface->pitch;
        const int bpp      = surface->format->BytesPerPixel;

        for (uint32_t y = 0; y < height; ++y) {
            uint8_t* row = dst + (y * pitch);
            for (uint32_t x = 0; x < width; ++x) {
                const uint32_t idx   = ((y * width) + x) * 4;
                const uint8_t r      = src[idx + 0];
                const uint8_t g      = src[idx + 1];
                const uint8_t b      = src[idx + 2];
                const uint8_t a      = src[idx + 3];
                const uint32_t pixel = SDL_MapRGBA(surface->format, r, g, b, a);
                if (bpp == 4) {
                    std::memcpy(row + (x * 4), &pixel, sizeof(pixel));
                } else {
                    std::memcpy(row + (x * bpp), &pixel, static_cast<size_t>(std::min(bpp, 4)));
                }
            }
        }

        SDL_UnlockSurface(surface);
        reset(surface);
    }

    Image::Image(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height) :
        IResource(name, nullptr), m_surface(nullptr), m_xshift(0), m_yshift(0), m_shared(false)
    {
        if (data == nullptr || width == 0 || height == 0) {
            return;
        }

        SDL_Surface* surface =
            SDL_CreateRGBSurface(0, static_cast<int>(width), static_cast<int>(height), 32, RMASK, GMASK, BMASK, AMASK);
        if (surface == nullptr) {
            return;
        }

        SDL_LockSurface(surface);
        const uint8_t* src = data;
        uint8_t* dst       = static_cast<uint8_t*>(surface->pixels);
        const int pitch    = surface->pitch;
        const int bpp      = surface->format->BytesPerPixel;

        for (uint32_t y = 0; y < height; ++y) {
            uint8_t* row = dst + (y * pitch);
            for (uint32_t x = 0; x < width; ++x) {
                const uint32_t idx   = ((y * width) + x) * 4;
                const uint8_t r      = src[idx + 0];
                const uint8_t g      = src[idx + 1];
                const uint8_t b      = src[idx + 2];
                const uint8_t a      = src[idx + 3];
                const uint32_t pixel = SDL_MapRGBA(surface->format, r, g, b, a);
                if (bpp == 4) {
                    std::memcpy(row + (x * 4), &pixel, sizeof(pixel));
                } else {
                    std::memcpy(row + (x * bpp), &pixel, static_cast<size_t>(std::min(bpp, 4)));
                }
            }
        }

        SDL_UnlockSurface(surface);
        reset(surface);
    }

    void Image::reset(SDL_Surface* surface)
    {
        if ((m_surface != nullptr) && !m_shared) {
            SDL_FreeSurface(m_surface);
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
        return m_surface->h * m_surface->pitch;
    }

    Rect Image::getArea() const
    {
        const uint32_t width  = getWidth();
        const uint32_t height = getHeight();
        assert(width <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
        assert(height <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
        Rect r(0, 0, static_cast<int32_t>(width), static_cast<int32_t>(height));
        return r;
    }

    void Image::getPixelRGBA(int32_t x, int32_t y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a)
    {
        Uint8* p = nullptr;

        assert(m_surface);

        int32_t const bpp = m_surface->format->BytesPerPixel;

        if (!isSharedImage()) {
            if ((x < 0) || (x >= m_surface->w) || (y < 0) || (y >= m_surface->h)) {
                (void)r;
                (void)g;
                (void)b;
                (void)a;
                return;
            }
            p = static_cast<Uint8*>(m_surface->pixels) + y * m_surface->pitch + x * bpp;
        } else {
            if ((x < 0) || ((x + m_subimagerect.x) >= m_surface->w) || (y < 0) ||
                ((y + m_subimagerect.y) >= m_surface->h)) {
                (void)r;
                (void)g;
                (void)b;
                (void)a;
                return;
            }
            p = static_cast<Uint8*>(m_surface->pixels) + (y + m_subimagerect.y) * m_surface->pitch +
                (x + m_subimagerect.x) * bpp;
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

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                pixel = p[0] << 16 | p[1] << 8 | p[2];
            } else {
                pixel = p[0] | p[1] << 8 | p[2] << 16;
            }
            break;

        case 4:
            std::memcpy(&pixel, p, sizeof(pixel));
            break;

        default:
            break;
        }
        SDL_GetRGBA(pixel, m_surface->format, r, g, b, a);
    }

    void Image::saveImage(const std::string& filename)
    {
        saveAsPng(filename, *m_surface);
    }

    void Image::saveAsPng(const std::string& filename, SDL_Surface& surface)
    {
        FILE* fp               = nullptr;
        png_structp pngptr     = nullptr;
        png_infop infoptr      = nullptr;
        int32_t colortype      = 0;
        png_bytep* rowpointers = nullptr;

#ifdef _MSC_VER
        errno_t _fopen_err = fopen_s(&fp, filename.c_str(), "wb");
        if (_fopen_err != 0 || fp == nullptr) {
            return;
        }
#else
        fp = fopen(filename.c_str(), "wb");

        if (fp == nullptr) {
            return;
        }
#endif

        // create the png file
        pngptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (pngptr == nullptr) {
            fclose(fp);
            return;
        }

        // create information struct
        infoptr = png_create_info_struct(pngptr);
        if (infoptr == nullptr) {
            fclose(fp);
            png_destroy_write_struct(&pngptr, (png_infopp) nullptr);
            return;
        }

        if (setjmp(png_jmpbuf(pngptr))) {
            png_destroy_write_struct(&pngptr, &infoptr);
            fclose(fp);
            return;
        }

        // initialize io
        png_init_io(pngptr, fp);

        // lock the surface for pixel access while encoding
        SDL_LockSurface(&surface);

        colortype = PNG_COLOR_TYPE_RGB;
        if (surface.format->palette != nullptr) {
            colortype |= PNG_COLOR_TYPE_PALETTE;
        } else if (surface.format->Amask != 0U) {
            colortype |= PNG_COLOR_TYPE_RGB_ALPHA;
        } else {
        }

        png_set_IHDR(
            pngptr,
            infoptr,
            static_cast<png_uint_32>(surface.w),
            static_cast<png_uint_32>(surface.h),
            8,
            colortype,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT);

        png_write_info(pngptr, infoptr);
        png_set_packing(pngptr);

        assert(surface.h >= 0);
        rowpointers = new png_bytep[static_cast<size_t>(surface.h)];
        for (int32_t i = 0; i < surface.h; i++) {
            rowpointers[i] = static_cast<png_bytep>(static_cast<Uint8*>(surface.pixels)) + i * surface.pitch;
        }
        // write the image
        png_write_image(pngptr, rowpointers);
        png_write_end(pngptr, infoptr);

        SDL_UnlockSurface(&surface);
        delete[] rowpointers;
        png_destroy_write_struct(&pngptr, &infoptr);
        fclose(fp);
    }

    std::string Image::createUniqueImageName()
    {
        // automated counting for name generation, in case the user doesn't provide a name
        static uint32_t uniqueNumber      = 0;
        static std::string const baseName = "image";

        std::ostringstream oss;
        oss << uniqueNumber << "_" << baseName;

        const std::string name = oss.str();
        ++uniqueNumber;

        return name;
    }

    void Image::copySubimage(uint32_t xoffset, uint32_t yoffset, const ImagePtr& srcimg)
    {
        if (srcimg->m_surface == nullptr) {
            return;
        }
        if (m_surface == nullptr) {
            const uint32_t srcWidth  = srcimg->getWidth();
            const uint32_t srcHeight = srcimg->getHeight();
            assert(srcWidth <= static_cast<uint32_t>(std::numeric_limits<int>::max()));
            assert(srcHeight <= static_cast<uint32_t>(std::numeric_limits<int>::max()));
            m_surface = SDL_CreateRGBSurface(
                0, static_cast<int>(srcWidth), static_cast<int>(srcHeight), 32, RMASK, GMASK, BMASK, AMASK);
        }
        // disable blending
        SDL_SetSurfaceBlendMode(srcimg->m_surface, SDL_BLENDMODE_NONE);
        if (this->isSharedImage()) {
            Rect const & rect = this->getSubImageRect();
            SDL_Rect dstrect  = {
                static_cast<Sint16>(rect.x + xoffset),
                static_cast<Sint16>(rect.y + yoffset),
                static_cast<Uint16>(srcimg->getWidth()),
                static_cast<Uint16>(srcimg->getHeight())};
            if (srcimg->isSharedImage()) {
                Rect const & srcRect   = srcimg->getSubImageRect();
                SDL_Rect const srcrect = {
                    static_cast<Sint16>(srcRect.x),
                    static_cast<Sint16>(srcRect.y),
                    static_cast<Uint16>(srcRect.w),
                    static_cast<Uint16>(srcRect.h)};
                SDL_BlitSurface(srcimg->m_surface, &srcrect, m_surface, &dstrect);
            } else {
                SDL_BlitSurface(srcimg->m_surface, nullptr, m_surface, &dstrect);
            }
        } else {
            SDL_Rect dstrect = {
                static_cast<Sint16>(xoffset),
                static_cast<Sint16>(yoffset),
                static_cast<Uint16>(srcimg->getWidth()),
                static_cast<Uint16>(srcimg->getHeight())};
            if (srcimg->isSharedImage()) {
                Rect const & rect      = srcimg->getSubImageRect();
                SDL_Rect const srcrect = {
                    static_cast<Sint16>(rect.x),
                    static_cast<Sint16>(rect.y),
                    static_cast<Uint16>(rect.w),
                    static_cast<Uint16>(rect.h)};
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

        int32_t const bpp = surface->format->BytesPerPixel;
        SDL_LockSurface(surface);
        Uint8* p           = static_cast<Uint8*>(surface->pixels) + (y * surface->pitch) + (x * bpp);
        Uint32 const pixel = SDL_MapRGBA(surface->format, r, g, b, a);
        switch (bpp) {
        case 1:
            *p = pixel;
            break;

        case 2: {
            const Uint16 pixel16 = static_cast<Uint16>(pixel);
            std::memcpy(p, &pixel16, sizeof(pixel16));
        } break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

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
