/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include <png.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fife_stdint.h"
#include "util/base/resourceclass.h"
#include "util/resource/resource.h"
#include "util/structures/point.h"
#include "util/structures/rect.h"

namespace FIFE {

	class AbstractImage {
	public:
		virtual ~AbstractImage() {}

		/** Get the surface used by this image
		 * @return pointer to used surface
		 */
		virtual SDL_Surface* getSurface() = 0;

		/** Returns the @b maximum width occupied by this image.
		 * This should return the maximum width that could be covered by the
		 * image.
		 */
		virtual unsigned int getWidth() const = 0;

		/** Returns the @b maximum height occupied by this image.
		 */
		virtual unsigned int getHeight() const = 0;

		/** Gets the area of the image
		 * @return Image area in rect
		 */
		virtual const Rect& getArea() = 0;

		/** Writes pixel to given position. Returns true, if pixel was written (not out of bounds)
		 */
 		virtual bool putPixel(int x, int y, int r, int g, int b, int a = 255) = 0;

		/** Draws line between given points with given RGBA
		 */
		virtual void drawLine(const Point& p1, const Point& p2, int r, int g, int b, int a = 255) = 0;

		/** Draws triangle between given points with given RGBA
		 */
		virtual void drawTriangle(const Point& p1, const Point& p2, const Point& p3, int r, int g, int b, int a = 255) = 0;

		/** Draws an axis parallel rectangle.
		 */
		virtual void drawRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;

		/** Draws a filled axis parallel rectangle.
		 */
		virtual void fillRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)  = 0;

		/** Draws quad between given points with given RGBA
		 */
		virtual void drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  int r, int g, int b, int a = 255) = 0;

		/** Draws a quad that represents a vertex with given RGBA
		 */
		virtual void drawVertex(const Point& p, const uint8_t size, int r, int g, int b, int a = 255) = 0;

		/** Draws a light primitive that based on a triangle fan
		 */
		virtual void drawLightPrimitive(const Point& p, uint8_t intensity, float radius, int subdivisions, float xstretch, float ystretch, uint8_t red, uint8_t green, uint8_t blue) = 0;

		/** Returns pixel RGBA values from given position
		 */
		virtual void getPixelRGBA(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) = 0;

		/** Pushes clip area to clip stack
		 *  Clip areas define which area is drawn on screen. Usable e.g. with viewports
		 *  note that previous items in stack do not affect the latest area pushed
		 */
		virtual void pushClipArea(const Rect& cliparea, bool clear=true) = 0;

		/** Pops clip area from clip stack
		 *  @see pushClipArea
		 */
		virtual void popClipArea() = 0;

		/** Gets the current clip area
		 *  @see pushClipArea
		 */
		virtual const Rect& getClipArea() const = 0;

		/** Saves the image using given filename
		 */
		virtual void saveImage(const std::string& filename) = 0;

		/** Enable or disable the alpha 'optimizing' code
		 *
		 * @param optimize Wether the image shall be analysed for 'fake' alpha images.
		 * Only implemented by and useful for the SDL backend at the moment.
		 */
		virtual void setAlphaOptimizerEnabled(bool enabled) = 0;

		/** @see setAlphaOptimizerEnabled
		 */
		virtual bool isAlphaOptimizerEnabled() = 0;
	};

	/** Base Class for Images.
	 */
	class Image : public ResourceClass, public AbstractImage {
	public:
		/** Constructor.
		* @note Takes ownership of the SDL Surface
		* @param surface SDL Surface in RGBA format
		 */
		Image(SDL_Surface* surface);

		/** Constructor
		* @param data Pointer to the imagedata (needs to be in RGBA, 8 bits per channel).
		* @param width Width of the image.
		* @param height Height of the image.
		 */
		Image(const uint8_t* data, unsigned int width, unsigned int height);

		/** Invalidates the Image causing it to be reset or re-loaded
		 */
		virtual void invalidate() = 0;

		/** Renders itself to the Destination surface at the rectangle rect.
		 *
		 * @param rect The position and clipping where to draw this image to.
		 * @param target Target surface to draw to, e.g. main screen or other image
		 * @param alpha The alpha value, with which to draw self. opaque by default.
		 */
		virtual void render(const Rect& rect, SDL_Surface* dst, unsigned char alpha = 255) = 0;

		/** Renders itself to the main screen at the rectangle rect.
		 * Convenience function
		 * @param rect The position and clipping where to draw this image to.
		 * @param alpha The alpha value, with which to draw self.
		 */
		void render(const Rect& rect, unsigned char alpha = 255);

		/** Removes underlying SDL_Surface from the image (if exists) and returns this
		 * @note this effectively causes SDL_Surface not to be freed on destruction
		 */
		SDL_Surface* detachSurface();

		virtual ~Image();
		SDL_Surface* getSurface() { return m_surface; }
		unsigned int getWidth() const;
		unsigned int getHeight() const;
		const Rect& getArea();
		void setXShift(int xshift);
		inline int getXShift() const {
			return m_xshift;
		}
		void setYShift(int yshift);
		inline int getYShift() const {
			return m_yshift;
		}
		void getPixelRGBA(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);
		void pushClipArea(const Rect& cliparea, bool clear=true);
		void popClipArea();
		const Rect& getClipArea() const;
		void setAlphaOptimizerEnabled(bool enabled) { m_isalphaoptimized = enabled; }
		bool isAlphaOptimizerEnabled() { return m_isalphaoptimized; }

	protected:
		/** Sets given clip area into image
		 *  @see pushClipArea
		 */
		virtual void setClipArea(const Rect& cliparea, bool clear) = 0;
		//saves images to png format
		virtual void saveAsPng(const std::string& filename, SDL_Surface& surface);
		/** Clears any possible clip areas
		 *  @see pushClipArea
		 */
		virtual void clearClipArea();

		// The SDL Surface used.
		SDL_Surface* m_surface;
		// The X shift of the Image
		int m_xshift;
		// The Y shift of the Image
		int m_yshift;

		class ClipInfo {
		public:
			Rect r;
			bool clearing;
		};
		std::stack<ClipInfo> m_clipstack;

		// image area
		Rect m_area;
		bool m_isalphaoptimized;

	private:
		void reset(SDL_Surface* surface);
	};

}

#endif
