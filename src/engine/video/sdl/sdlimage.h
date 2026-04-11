// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_RENDERBACKENDS_SDL_SDLIMAGE_H
#define FIFE_VIDEO_RENDERBACKENDS_SDL_SDLIMAGE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <SDL_video.h>

// FIFE includes
#include "video/image.h"

namespace FIFE
{

    /** The SDL implementation of the @c Image base class.
     */
    class FIFE_API SDLImage : public Image
    {
    public:
        explicit SDLImage(IResourceLoader* loader = nullptr);
        explicit SDLImage(const std::string& name, IResourceLoader* loader = nullptr);
        explicit SDLImage(SDL_Surface* surface);
        SDLImage(const std::string& name, SDL_Surface* surface);
        explicit SDLImage(const uint8_t* data, uint32_t width, uint32_t height);
        SDLImage(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height);

        virtual ~SDLImage();
        virtual void invalidate();
        virtual void setSurface(SDL_Surface* surface);
        virtual void render(const Rect& rect, uint8_t alpha = 255, uint8_t const * rgb = nullptr);
        virtual size_t getSize();
        virtual void useSharedImage(const ImagePtr& shared, const Rect& region);
        virtual void forceLoadInternal();
        virtual void load();
        virtual void free();

        SDL_Texture* getTexture();
        void setTexture(SDL_Texture* texture);

    private:
        void resetSdlimage();
        void validateShared();

        // colorkey for the image
        SDL_Color m_colorkey{};
        // texture of image
        SDL_Texture* m_texture{nullptr};

        // Holds Atlas ImagePtr if this is a shared image
        ImagePtr m_atlas_img;
        // Holds Atlas Name if this is a shared image
        std::string m_atlas_name;
    };

} // namespace FIFE

#endif
