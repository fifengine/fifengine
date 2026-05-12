// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_IMAGE_H
#define FIFE_VIDEO_IMAGE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <stack>
#include <string>

// 3rd party library includes
#include <SDL3/SDL.h>
#define PNG_SKIP_SETJMP_CHECK
#include <png.h>

// FIFE includes
#include "util/base/fife_stdint.h"
#include "util/resource/resource.h"
#include "util/structures/point.h"
#include "util/structures/rect.h"

namespace FIFE
{
    class Image;
    using ImagePtr = SharedPtr<Image>;
    /** Base Class for Images.
     */
    class FIFE_API Image : public IResource
    {
        public:
            /** Constructor.
             */
            explicit Image(IResourceLoader* loader = nullptr);
            explicit Image(std::string const & name, IResourceLoader* loader = nullptr);

            /** Constructor.
             * @note Takes ownership of the SDL Surface
             * @param surface SDL Surface in RGBA format
             */
            explicit Image(SDL_Surface* surface);
            Image(std::string const & name, SDL_Surface* surface);

            /** Constructor
             * @param data Pointer to the imagedata (needs to be in RGBA, 8 bits per channel).
             * @param width Width of the image.
             * @param height Height of the image.
             */
            explicit Image(uint8_t const * data, uint32_t width, uint32_t height);
            Image(std::string const & name, uint8_t const * data, uint32_t width, uint32_t height);

            /** Destructor.
             */
            virtual ~Image();

            /** Invalidates the Image causing it to be reset or re-loaded
             */
            virtual void invalidate() = 0;

            /** Renders itself to the current render target (main screen or attached destination image) at the rectangle
             * rect. Convenience function
             * @param rect The position and clipping where to draw this image to.
             * @param alpha The alpha value, with which to draw self.
             * @param rgb The color value of overlay if any.
             */
            virtual void render(Rect const & rect, uint8_t alpha = 255, uint8_t const * rgb = nullptr) = 0;
            virtual void render(
                Rect const & rect, ImagePtr const & overlay, uint8_t alpha = 255, uint8_t const * rgb = nullptr)
            {
                static_cast<void>(rect);
                static_cast<void>(overlay);
                static_cast<void>(alpha);
                static_cast<void>(rgb);
            }
            virtual void renderZ(Rect const & rect, float vertexZ, uint8_t alpha = 255, uint8_t const * rgb = nullptr)
            {
                static_cast<void>(rect);
                static_cast<void>(vertexZ);
                static_cast<void>(alpha);
                static_cast<void>(rgb);
            }
            virtual void renderZ(
                Rect const & rect,
                float vertexZ,
                ImagePtr const & overlay,
                uint8_t alpha       = 255,
                uint8_t const * rgb = nullptr)
            {
                static_cast<void>(rect);
                static_cast<void>(vertexZ);
                static_cast<void>(overlay);
                static_cast<void>(alpha);
                static_cast<void>(rgb);
            }

            virtual void renderZ(
                Rect const & rect,
                float vertexZ,
                uint8_t alpha       = 255,
                bool forceNewBatch  = false,
                uint8_t const * rgb = nullptr)
            {
                static_cast<void>(rect);
                static_cast<void>(vertexZ);
                static_cast<void>(alpha);
                static_cast<void>(forceNewBatch);
                static_cast<void>(rgb);
            }

            /** Removes underlying SDL_Surface from the image (if exists) and returns this
             * @note this effectively causes SDL_Surface not to be freed on destruction
             */
            SDL_Surface* detachSurface();

            SDL_Surface* getSurface()
            {
                assert(m_surface);
                return m_surface;
            }
            SDL_Surface const * getSurface() const
            {
                assert(m_surface);
                return m_surface;
            }

            /** This frees the current suface and replaces it with the
             * surface passed in the parameter (which can be NULL).
             * @see Image::reset(SDL_Surface* surface)
             * @param surface the SDL_Surface to use for this image
             */
            virtual void setSurface(SDL_Surface* surface) = 0;

            /** Saves the image using given filename.
             */
            void saveImage(std::string const & filename);

            /** Saves the SDL_Surface to png format
             */
            static void saveAsPng(std::string const & filename, SDL_Surface& surface);
            static bool putPixel(
                SDL_Surface* surface, int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

            uint32_t getWidth() const;
            uint32_t getHeight() const;
            Rect getArea() const;

            void setXShift(int32_t xshift)
            {
                m_xshift = xshift;
            }
            int32_t getXShift() const
            {
                return m_xshift;
            }
            void setYShift(int32_t yshift)
            {
                m_yshift = yshift;
            }
            int32_t getYShift() const
            {
                return m_yshift;
            }

            void getPixelRGBA(int32_t x, int32_t y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);

            virtual size_t getSize();
            virtual void load();
            virtual void free();

            /** After this call all image data will be taken from the given image and its subregion
             */
            virtual void useSharedImage(ImagePtr const & shared, Rect const & region) = 0;

            /** Forces to load the image into internal memory of GPU
             */
            virtual void forceLoadInternal() = 0;

            /** Returns true if this image shares data with another one
             */
            bool isSharedImage() const
            {
                return m_shared;
            }

            /** Returns area of the image it occupies in the shared image
             */
            Rect const & getSubImageRect() const
            {
                return m_subimagerect;
            }

            /** Copies given image into this one with respect to given offsets
             */
            virtual void copySubimage(uint32_t xoffset, uint32_t yoffset, ImagePtr const & img);

        protected:
            // The SDL Surface used.
            SDL_Surface* m_surface;
            // Area which this image occupy in shared image
            Rect m_subimagerect;
            // The X shift of the Image
            int32_t m_xshift;
            // The Y shift of the Image
            int32_t m_yshift;

            /** Resets the image to default values (including the x and y shift
             * values), frees the current surface  and sets the surface to the
             * passed SDL_Surface (which can be NULL).
             * @see IImage::setSurface(SDL_Surface* surface)
             * @param surface the SDL_Surface to use for this image
             */
            void reset(SDL_Surface* surface);

            // Does this image share data with another
            bool m_shared;

        private:
            std::string createUniqueImageName();
    };
} // namespace FIFE

#endif
