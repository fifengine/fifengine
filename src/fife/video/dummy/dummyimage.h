// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_DUMMY_DUMMYIMAGE_H
#define FIFE_VIDEO_DUMMY_DUMMYIMAGE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "video/image.h"

namespace FIFE
{

    /** Headless image implementation used by the dummy render backends.
     *
     * It reuses the SDL_Surface storage owned by @c Image and allocates neither
     * an SDL texture nor an OpenGL resource, so it needs no renderer or window.
     */
    class FIFE_API DummyImage : public Image
    {
        public:
            explicit DummyImage(IResourceLoader* loader = nullptr);
            explicit DummyImage(std::string const & name, IResourceLoader* loader = nullptr);
            explicit DummyImage(SDL_Surface* surface);
            DummyImage(std::string const & name, SDL_Surface* surface);
            explicit DummyImage(uint8_t const * data, uint32_t width, uint32_t height);
            DummyImage(std::string const & name, uint8_t const * data, uint32_t width, uint32_t height);

            ~DummyImage() override;

            void invalidate() override;
            void setSurface(SDL_Surface* surface) override;
            void render(Rect const & rect, uint8_t alpha = 255, uint8_t const * rgb = nullptr) override;
            void useSharedImage(ImagePtr const & shared, Rect const & region) override;
            void forceLoadInternal() override;
            void load() override;
            void free() override;
    };

} // namespace FIFE

#endif
