// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "imageloader.h"

// Standard C++ library includes
#include <memory>
#include <string>

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
    void ImageLoader::load(IResource* res)
    {
        VFS* vfs = VFS::instance();

        auto* img = dynamic_cast<Image*>(res);

        // Have to save the images x and y shift or it gets lost when it's
        // loaded again.
        int32_t const xShiftSave = img->getXShift();
        int32_t const yShiftSave = img->getYShift();

        if (!img->isSharedImage()) {
            std::string const & filename = img->getName();
            std::unique_ptr<RawData> data(vfs->open(filename));
            size_t const datalen = data->getDataLength();
            std::unique_ptr<uint8_t[]> const darray(new uint8_t[datalen]);
            data->readInto(darray.get(), datalen);
            SDL_IOStream* iostream = SDL_IOFromConstMem(darray.get(), datalen);

            SDL_Surface* surface = IMG_Load_IO(iostream, false);

            if (surface == nullptr) {
                throw SDLException(std::string("Fatal Error when loading image into a SDL_Surface: ") + SDL_GetError());
            }

            RenderBackend* rb = RenderBackend::instance();
            // in case of SDL we don't need to convert the surface
            if (rb->getName() == "SDL") {
                img->setSurface(surface);
                // in case of OpenGL we need a 32bit surface
            } else {
                SDL_PixelFormat dst_fmt                = rb->getPixelFormat();
                SDL_PixelFormatDetails const * details = SDL_GetPixelFormatDetails(dst_fmt);
                uint8_t const srcbits                  = SDL_BITSPERPIXEL(surface->format);

                if (srcbits != 32 || details->bits_per_pixel != 32) {
                    SDL_Surface* conv = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA8888);

                    if (conv == nullptr) {
                        throw SDLException(
                            std::string("Fatal Error when converting surface to the screen format: ") + SDL_GetError());
                    }

                    img->setSurface(conv);
                    SDL_DestroySurface(surface);
                } else {
                    img->setSurface(surface);
                }
            }

            SDL_CloseIO(iostream);
        }
        // restore saved x and y shifts
        img->setXShift(xShiftSave);
        img->setYShift(yShiftSave);
    }
} // namespace FIFE
