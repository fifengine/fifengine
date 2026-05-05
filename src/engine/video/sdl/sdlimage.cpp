// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "sdlimage.h"

// Standard C++ library includes
#include <cassert>
#include <iostream>
#include <string>

// 3rd party library includes

// FIFE includes
#include "renderbackendsdl.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/structures/rect.h"
#include "video/imagemanager.h"
#include "video/renderbackend.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_VIDEO);

    SDLImage::SDLImage(IResourceLoader* loader) : Image(loader)
    {
        resetSdlimage();
    }

    SDLImage::SDLImage(std::string const & name, IResourceLoader* loader) : Image(name, loader)
    {
        resetSdlimage();
    }

    SDLImage::SDLImage(SDL_Surface* surface) : Image(surface)
    {
        resetSdlimage();
    }

    SDLImage::SDLImage(std::string const & name, SDL_Surface* surface) : Image(name, surface)
    {
        resetSdlimage();
    }

    SDLImage::SDLImage(uint8_t const * data, uint32_t width, uint32_t height) : Image(data, width, height)
    {
        resetSdlimage();
    }

    SDLImage::SDLImage(std::string const & name, uint8_t const * data, uint32_t width, uint32_t height) :
        Image(name, data, width, height)
    {
        resetSdlimage();
    }

    void SDLImage::resetSdlimage()
    {
        m_colorkey = RenderBackend::instance()->getColorKey();
        m_texture  = nullptr;
    }

    SDLImage::~SDLImage()
    {
        invalidate();
    }

    void SDLImage::invalidate()
    {
        if ((m_texture != nullptr) && !m_shared) {
            SDL_DestroyTexture(m_texture);
        }
        m_texture = nullptr;
    }

    void SDLImage::setSurface(SDL_Surface* surface)
    {
        invalidate();
        reset(surface);
        resetSdlimage();
    }

    void SDLImage::render(Rect const & rect, uint8_t alpha, uint8_t const * rgb)
    {
        if (alpha == 0) {
            return;
        }

        SDL_Surface const * target = RenderBackend::instance()->getRenderTargetSurface();
        assert(target != m_surface); // can't draw on the source surface

        if (rect.right() < 0 || rect.x > static_cast<int32_t>(target->w) || rect.bottom() < 0 ||
            rect.y > static_cast<int32_t>(target->h)) {
            return;
        }

        SDL_FRect tarRect;
        tarRect.x = static_cast<float>(rect.x);
        tarRect.y = static_cast<float>(rect.y);
        tarRect.w = static_cast<float>(rect.w);
        tarRect.h = static_cast<float>(rect.h);

        Rect const tmpRect = m_shared ? getSubImageRect() : getArea();
        SDL_FRect srcRect;
        srcRect.x = static_cast<float>(tmpRect.x);
        srcRect.y = static_cast<float>(tmpRect.y);
        srcRect.w = static_cast<float>(tmpRect.w);
        srcRect.h = static_cast<float>(tmpRect.h);

        RenderBackendSDL* backend = dynamic_cast<RenderBackendSDL*>(RenderBackend::instance());
        if (backend == nullptr) {
            throw SDLException("Render backend is not SDL.");
        }
        SDL_Renderer* renderer = backend->getRenderer();

        // create texture
        if (m_texture == nullptr) {
            if (m_surface == nullptr) {
                load();
            }
            m_texture = SDL_CreateTextureFromSurface(renderer, m_surface);
        }

        // set additonal color and alpha mods
        if (rgb != nullptr) {
            SDL_SetTextureColorMod(m_texture, rgb[0], rgb[1], rgb[2]);
            SDL_SetTextureAlphaMod(m_texture, rgb[3]);
        } else {
            SDL_SetTextureColorMod(m_texture, 255, 255, 255);
            SDL_SetTextureAlphaMod(m_texture, 255);
        }
        // set render color
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);

        if (SDL_RenderTexture(renderer, m_texture, &srcRect, &tarRect) != 0) {
            throw SDLException(SDL_GetError());
        }
    }

    size_t SDLImage::getSize()
    {
        size_t size = 0;
        if ((m_surface != nullptr) && !m_shared) {
            size += static_cast<size_t>(m_surface->h) * static_cast<size_t>(m_surface->pitch);
        }

        return size;
    }

    void SDLImage::useSharedImage(ImagePtr const & shared, Rect const & region)
    {
        if (shared->getState() != IResource::RES_LOADED) {
            shared->load();
        }
        SDL_Surface* surface = shared->getSurface();
        if (surface == nullptr) {
            shared->load();
            surface = shared->getSurface();
        }
        auto* image = dynamic_cast<SDLImage*>(shared.get());
        m_texture   = image->getTexture();
        if (m_texture == nullptr) {
            // create atlas texture
            SDL_Renderer* renderer = dynamic_cast<RenderBackendSDL*>(RenderBackend::instance())->getRenderer();
            m_texture              = SDL_CreateTextureFromSurface(renderer, surface);
            image->setTexture(m_texture);
        }
        setSurface(surface);
        m_shared       = true;
        m_subimagerect = region;
        m_atlas_img    = shared;
        m_atlas_name   = shared->getName();
        setState(IResource::RES_LOADED);
    }

    void SDLImage::forceLoadInternal()
    {
        validateShared();
    }

    void SDLImage::validateShared()
    {
        if (m_atlas_name.empty()) {
            return;
        }

        if (m_atlas_img->getState() == IResource::RES_NOT_LOADED || getState() == IResource::RES_NOT_LOADED) {
            load();
        }
    }

    void SDLImage::load()
    {
        if (!m_atlas_name.empty()) {
            // check atlas image
            // if it does not exist, it is created.
            if (!ImageManager::instance()->exists(m_atlas_name)) {
                ImagePtr const newAtlas = ImageManager::instance()->create(m_atlas_name);
                m_atlas_img             = newAtlas;
            }
            useSharedImage(m_atlas_img, m_subimagerect);
        } else {
            Image::load();
        }
    }

    void SDLImage::free()
    {
        // save the image offsets
        int32_t const xshift = m_xshift;
        int32_t const yshift = m_yshift;
        setSurface(nullptr);
        m_xshift = xshift;
        m_yshift = yshift;
        m_state  = IResource::RES_NOT_LOADED;
    }

    SDL_Texture* SDLImage::getTexture()
    {
        return m_texture;
    }

    void SDLImage::setTexture(SDL_Texture* texture)
    {
        if (m_texture == texture) {
            return;
        }
        if ((m_texture != nullptr) && !m_shared) {
            SDL_DestroyTexture(m_texture);
        }
        m_texture = texture;
    }
} // namespace FIFE
