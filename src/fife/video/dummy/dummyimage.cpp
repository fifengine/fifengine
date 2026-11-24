// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "video/dummy/dummyimage.h"

// C++ standard library includes
#include <string>

// FIFE includes
#include "video/renderbackend.h"

namespace FIFE
{
    DummyImage::DummyImage(IResourceLoader* loader) : Image(loader)
    {
    }

    DummyImage::DummyImage(std::string const & name, IResourceLoader* loader) : Image(name, loader)
    {
    }

    DummyImage::DummyImage(SDL_Surface* surface) : Image(surface)
    {
    }

    DummyImage::DummyImage(std::string const & name, SDL_Surface* surface) : Image(name, surface)
    {
    }

    DummyImage::DummyImage(uint8_t const * data, uint32_t width, uint32_t height) : Image(data, width, height)
    {
    }

    DummyImage::DummyImage(std::string const & name, uint8_t const * data, uint32_t width, uint32_t height) :
        Image(name, data, width, height)
    {
    }

    DummyImage::~DummyImage() = default;

    // There is no GPU resource to release in a headless image.
    void DummyImage::invalidate()
    {
    }

    void DummyImage::setSurface(SDL_Surface* surface)
    {
        reset(surface);
    }

    // Delegate to the backend so Surface can blit the image onto its software
    // target while Null and other backends keep the default no-op behaviour.
    void DummyImage::render(Rect const & rect, uint8_t alpha, uint8_t const * rgb)
    {
        if (alpha == 0 || !RenderBackend::exists()) {
            return;
        }
        RenderBackend::instance()->drawImageToTarget(*this, rect, alpha, rgb);
    }

    // Sharing is meaningful only for GPU textures, which this image has none.
    void DummyImage::useSharedImage(ImagePtr const &, Rect const &)
    {
    }

    void DummyImage::forceLoadInternal()
    {
    }

    void DummyImage::load()
    {
        Image::load();
    }

    void DummyImage::free()
    {
        Image::free();
    }

} // namespace FIFE
