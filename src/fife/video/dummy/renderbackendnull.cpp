// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "video/dummy/renderbackendnull.h"

// Standard C++ library includes
#include <memory>
#include <string>

// FIFE includes
#include "video/dummy/dummyimage.h"

namespace FIFE
{
    RenderBackendNull::RenderBackendNull(SDL_Color const & colorkey, uint32_t width, uint32_t height) :
        RenderBackendDummy(colorkey, width, height)
    {
    }

    RenderBackendNull::~RenderBackendNull() = default;

    std::string const & RenderBackendNull::getName() const
    {
        static std::string const name = "DUMMY_NULL";
        return name;
    }

    // Deliberately does not initialize SDL video.
    void RenderBackendNull::init(std::string const & /*driver*/)
    {
    }

    // No window or screen surface is created in headless mode.
    void RenderBackendNull::createMainScreen(std::string const & /*title*/, std::string const & /*icon*/)
    {
    }

    std::unique_ptr<Image> RenderBackendNull::createImage(IResourceLoader* loader)
    {
        return std::make_unique<DummyImage>(loader);
    }

    std::unique_ptr<Image> RenderBackendNull::createImage(std::string const & name, IResourceLoader* loader)
    {
        return std::make_unique<DummyImage>(name, loader);
    }

    std::unique_ptr<Image> RenderBackendNull::createImage(uint8_t const * data, uint32_t width, uint32_t height)
    {
        return std::make_unique<DummyImage>(data, width, height);
    }

    std::unique_ptr<Image> RenderBackendNull::createImage(
        std::string const & name, uint8_t const * data, uint32_t width, uint32_t height)
    {
        return std::make_unique<DummyImage>(name, data, width, height);
    }

    std::unique_ptr<Image> RenderBackendNull::createImage(SDL_Surface* surface)
    {
        return std::make_unique<DummyImage>(surface);
    }

    std::unique_ptr<Image> RenderBackendNull::createImage(std::string const & name, SDL_Surface* surface)
    {
        return std::make_unique<DummyImage>(name, surface);
    }

} // namespace FIFE
