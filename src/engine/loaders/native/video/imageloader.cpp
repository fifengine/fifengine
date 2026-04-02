// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <memory>
#include <string>

// 3rd party library includes
#include <SDL.h>
#include <SDL_image.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "controller/engine.h"
#include "util/base/exception.h"
#include "util/resource/resource.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "video/image.h"
#include "video/renderbackend.h"

#include "imageloader.h"

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
            const std::string& filename = img->getName();
            std::unique_ptr<RawData> data(vfs->open(filename));
            size_t const datalen = data->getDataLength();
            std::unique_ptr<uint8_t[]> const darray(new uint8_t[datalen]);
            data->readInto(darray.get(), datalen);
            SDL_RWops* rwops = SDL_RWFromConstMem(darray.get(), static_cast<int>(datalen));

            SDL_Surface* surface = IMG_Load_RW(rwops, 0);

            if (surface == nullptr) {
                throw SDLException(std::string("Fatal Error when loading image into a SDL_Surface: ") + SDL_GetError());
            }

            RenderBackend* rb = RenderBackend::instance();
            // in case of SDL we don't need to convert the surface
            if (rb->getName() == "SDL") {
                img->setSurface(surface);
                // in case of OpenGL we need a 32bit surface
            } else {
                SDL_PixelFormat dst_format       = rb->getPixelFormat();
                SDL_PixelFormat const src_format = *surface->format;
                uint8_t const dstbits            = dst_format.BitsPerPixel;
                uint8_t const srcbits            = src_format.BitsPerPixel;

                if (srcbits != 32 || dst_format.Rmask != src_format.Rmask || dst_format.Gmask != src_format.Gmask ||
                    dst_format.Bmask != src_format.Bmask || dst_format.Amask != src_format.Amask) {
                    dst_format.BitsPerPixel = 32;
                    SDL_Surface* conv       = SDL_ConvertSurface(surface, &dst_format, 0);
                    dst_format.BitsPerPixel = dstbits;

                    if (conv == nullptr) {
                        throw SDLException(
                            std::string("Fatal Error when converting surface to the screen format: ") + SDL_GetError());
                    }

                    img->setSurface(conv);
                    SDL_FreeSurface(surface);
                } else {
                    img->setSurface(surface);
                }
            }

            SDL_FreeRW(rwops);
        }
        // restore saved x and y shifts
        img->setXShift(xShiftSave);
        img->setYShift(yShiftSave);
    }
} // namespace FIFE
