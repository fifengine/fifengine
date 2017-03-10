/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_VIDEO_IMAGE_H
#define FIFE_VIDEO_IMAGE_H

// Standard C++ library includes
#include <stack>

// 3rd party library includes
#include <SDL.h>
#define PNG_SKIP_SETJMP_CHECK
#include <png.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fife_stdint.h"
#include "util/resource/resource.h"
#include "util/structures/point.h"
#include "util/structures/rect.h"

namespace FIFE {
	class Image;
	typedef SharedPtr<Image> ImagePtr;

	/** Base Class for Images.
	 */
	class Image : public IResource {
	public:
		/** Constructor.
		*/
		Image(IResourceLoader* loader = 0);
		Image(const std::string& name, IResourceLoader* loader = 0);

		/** Constructor.
		* @note Takes ownership of the SDL Surface
		* @param surface SDL Surface in RGBA format
		 */
		Image(SDL_Surface* surface);
		Image(const std::string& name, SDL_Surface* surface);

		/** Constructor
		* @param data Pointer to the imagedata (needs to be in RGBA, 8 bits per channel).
		* @param width Width of the image.
		* @param height Height of the image.
		 */
		Image(const uint8_t* data, uint32_t width, uint32_t height);
		Image(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height);

		/** Destructor.
		*/
		virtual ~Image();

		/** Invalidates the Image causing it to be reset or re-loaded
		 */
		virtual void invalidate() = 0;

		/** Renders itself to the current render target (main screen or attached destination image) at the rectangle rect.
		 * Convenience function
		 * @param rect The position and clipping where to draw this image to.
		 * @param alpha The alpha value, with which to draw self.
		 * @param rgb The color value of overlay if any.
		 */
		virtual void render(const Rect& rect, uint8_t alpha = 255, uint8_t const* rgb = 0) = 0;
		virtual void render(const Rect& rect, const ImagePtr& overlay, uint8_t alpha = 255, uint8_t const* rgb = 0) {}
		virtual void renderZ(const Rect& rect, float vertexZ, uint8_t alpha = 255, uint8_t const* rgb = 0) {}
		virtual void renderZ(const Rect& rect, float vertexZ, const ImagePtr& overlay, uint8_t alpha = 255, uint8_t const* rgb = 0) {}

		virtual void renderZ(const Rect& rect, float vertexZ, uint8_t alpha = 255, bool forceNewBatch = false, uint8_t const* rgb = 0) {}

		/** Removes underlying SDL_Surface from the image (if exists) and returns this
		 * @note this effectively causes SDL_Surface not to be freed on destruction
		 */
		SDL_Surface* detachSurface();

		SDL_Surface* getSurface() { assert(m_surface); return m_surface; }
		const SDL_Surface* getSurface() const { assert(m_surface); return m_surface; }

		/** This frees the current suface and replaces it with the
		 * surface passed in the parameter (which can be NULL).
		 * @see Image::reset(SDL_Surface* surface)
		 * @param surface the SDL_Surface to use for this image
		 */
		virtual void setSurface(SDL_Surface* surface) = 0;

		/** Saves the image using given filename.
		 */
		void saveImage(const std::string& filename);

		/** Saves the SDL_Surface to png format
		 */
		static void saveAsPng(const std::string& filename, const SDL_Surface& surface);
		static bool putPixel(SDL_Surface* surface, int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

		uint32_t getWidth() const;
		uint32_t getHeight() const;
		Rect getArea() const;

		void setXShift(int32_t xshift) {
			m_xshift = xshift;
		}
		int32_t getXShift() const {
			return m_xshift;
		}
		void setYShift(int32_t yshift) {
			m_yshift = yshift;
		}
		int32_t getYShift() const {
			return m_yshift;
		}

		void getPixelRGBA(int32_t x, int32_t y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);

		virtual size_t getSize();
		virtual void load();
		virtual void free();

		/** After this call all image data will be taken from the given image and its subregion
		 */
		virtual void useSharedImage(const ImagePtr& shared, const Rect& region) = 0;

		/** Forces to load the image into internal memory of GPU
		 */
		virtual void forceLoadInternal() = 0;

		/** Returns true if this image shares data with another one
		 */
		bool isSharedImage() const { return m_shared; }

		/** Returns area of the image it occupies in the shared image
		 */
		const Rect& getSubImageRect() const { return m_subimagerect; }

		/** Copies given image into this one with respect to given offsets
		 */
		virtual void copySubimage(uint32_t xoffset, uint32_t yoffset, const ImagePtr& img);

	protected:
		// The SDL Surface used.
		SDL_Surface* m_surface;
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
		// Area which this image occupy in shared image
		Rect m_subimagerect;

	private:
		std::string createUniqueImageName();
	};
}

#endif
