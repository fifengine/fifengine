// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "imageloader.h"

// Standard C++ library includes
#include <format>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

// 3rd party library includes
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

// FIFE includes
#include "controller/engine.h"
#include "util/base/exception.h"
#include "util/resource/resource.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "video/image.h"
#include "video/renderbackend.h"

namespace FIFE
{
    // RAII wrappers for SDL3 resources
    struct SDLSurfaceDeleter
    {
            void operator()(SDL_Surface* p) const noexcept
            {
                SDL_DestroySurface(p);
            }
    };
    struct SDLIOStreamDeleter
    {
            void operator()(SDL_IOStream* p) const noexcept
            {
                SDL_CloseIO(p);
            }
    };

    using SDL_SurfacePtr  = std::unique_ptr<SDL_Surface, SDLSurfaceDeleter>;
    using SDL_IOStreamPtr = std::unique_ptr<SDL_IOStream, SDLIOStreamDeleter>;

    // Helper: sanitize transparent pixels to prevent filtering artifacts
    void sanitizeTransparentPixels(SDL_Surface* surface)
    {
        SDL_LockSurface(surface);
        auto const * fmt = SDL_GetPixelFormatDetails(surface->format);

        auto allPixels =
            std::span(static_cast<uint32_t*>(surface->pixels), static_cast<size_t>((surface->pitch / 4) * surface->h));
        size_t const pitchPx = static_cast<size_t>(surface->pitch / 4);

        for (int y = 0; y < surface->h; ++y) {
            size_t const rowStart = static_cast<size_t>(y) * pitchPx;

            for (int x = 0; x < surface->w; ++x) {
                Uint8 r = 0;
                Uint8 g = 0;
                Uint8 b = 0;
                Uint8 a = 0;
                SDL_GetRGBA(allPixels[rowStart + static_cast<size_t>(x)], fmt, nullptr, &r, &g, &b, &a);
                if (a == 0) {
                    allPixels[rowStart + static_cast<size_t>(x)] = SDL_MapRGBA(fmt, nullptr, 0, 0, 0, 0);
                }
            }
        }
        SDL_UnlockSurface(surface);
    }

    // Helper: load surface from file or memory buffer
    SDL_SurfacePtr loadSurface(std::string const & filename, std::string& outError)
    {
        // Try direct file load first
        if (auto* surface = IMG_Load(filename.c_str())) {
            return SDL_SurfacePtr{surface};
        }
        outError = SDL_GetError();

        // Fallback: load from VFS memory buffer
        auto* vfs          = VFS::instance();
        auto data          = vfs->open(filename);
        auto const datalen = data->getDataLength();

        std::vector<uint8_t> buffer(datalen);
        data->readInto(buffer.data(), datalen);

        auto* iostream = SDL_IOFromConstMem(buffer.data(), datalen);
        if (iostream == nullptr) {
            return nullptr;
        }
        SDL_IOStreamPtr const ioGuard{iostream};

        auto* surface = IMG_Load_IO(iostream, false);
        if (surface == nullptr) {
            return nullptr;
        }
        return SDL_SurfacePtr{surface};
    }

    void ImageLoader::load(IResource* res)
    {
        auto* img = dynamic_cast<Image*>(res);

        auto const [xShiftSave, yShiftSave] = std::pair{img->getXShift(), img->getYShift()};

        if (img->isSharedImage()) {
            img->setXShift(xShiftSave);
            img->setYShift(yShiftSave);
            return;
        }

        std::string const filename = img->getName();
        std::string fileError;

        auto surface = loadSurface(filename, fileError);
        if (!surface) {
            throw SDLException(
                std::format(
                    "Fatal Error when loading image: file load failed: {}; memory load failed: {}",
                    fileError,
                    SDL_GetError()));
        }

        // Determine target pixel format: use RGBA32 (byte order [R,G,B,A] in memory on all platforms)
        // for the OpenGL backend to match GL_RGBA/GL_UNSIGNED_BYTE uploads.
        auto const target_fmt =
            (RenderBackend::instance()->getName() == "SDL") ? SDL_PIXELFORMAT_RGBA8888 : SDL_PIXELFORMAT_RGBA32;

        // Convert if needed
        if (surface->format != target_fmt) {
            auto conv = SDL_SurfacePtr{SDL_ConvertSurface(surface.get(), target_fmt)};
            if (!conv) {
                throw SDLException(
                    std::format("Fatal Error when converting surface to screen format: {}", SDL_GetError()));
            }
            sanitizeTransparentPixels(conv.get());
            img->setSurface(conv.release()); // transfer ownership
        } else {
            sanitizeTransparentPixels(surface.get());
            img->setSurface(surface.release()); // transfer ownership
        }

        img->setXShift(xShiftSave);
        img->setYShift(yShiftSave);
    }
} // namespace FIFE
