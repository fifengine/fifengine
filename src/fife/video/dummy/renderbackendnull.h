// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_DUMMY_RENDERBACKENDNULL_H
#define FIFE_VIDEO_DUMMY_RENDERBACKENDNULL_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <memory>
#include <string>

// 3rd party library includes

// FIFE includes
#include "video/dummy/renderbackenddummy.h"

namespace FIFE
{

    /** Minimal headless render backend.
     *
     * It performs no SDL video initialization, creates no window and no screen
     * surface, and hands out @c DummyImage instances for all image creation.
     */
    class FIFE_API RenderBackendNull : public RenderBackendDummy
    {
        public:
            explicit RenderBackendNull(SDL_Color const & colorkey, uint32_t width = 800, uint32_t height = 600);
            ~RenderBackendNull() override;

            RenderBackendNull(RenderBackendNull const &)            = delete;
            RenderBackendNull& operator=(RenderBackendNull const &) = delete;

            std::string const & getName() const override;

            void init(std::string const & driver) override;
            void createMainScreen(std::string const & title, std::string const & icon) override;

            std::unique_ptr<Image> createImage(IResourceLoader* loader = nullptr) override;
            std::unique_ptr<Image> createImage(std::string const & name, IResourceLoader* loader = nullptr) override;
            std::unique_ptr<Image> createImage(uint8_t const * data, uint32_t width, uint32_t height) override;
            std::unique_ptr<Image> createImage(
                std::string const & name, uint8_t const * data, uint32_t width, uint32_t height) override;
            std::unique_ptr<Image> createImage(SDL_Surface* surface) override;
            std::unique_ptr<Image> createImage(std::string const & name, SDL_Surface* surface) override;
    };

} // namespace FIFE

#endif
