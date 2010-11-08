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

#ifndef FIFE_VIDEO_RENDERBACKEND_H
#define FIFE_VIDEO_RENDERBACKEND_H

// Standard C++ library includes
#include <string>
#include <vector>

// Platform specific includes
#include "util/base/fife_stdint.h"

// 3rd party library includes
#include <SDL.h>
#include <SDL_video.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/singleton.h"
#include "util/structures/point.h"
#include "util/structures/rect.h"
#include "video/devicecaps.h"

#include "image.h"

namespace FIFE {

	class Image;

	 /** Abstract interface for all the renderbackends. */
	class RenderBackend: public AbstractImage, public DynamicSingleton<RenderBackend> {
	public:
		/** Constructor.
		 * @param name The name of the new renderbackend.
		 */
		RenderBackend(const SDL_Color& colorkey);

		/** Destructor.
		 */
		virtual ~RenderBackend();

		/** The name of the renderbackend.
		 * @return The name of this renderbackend.
		 */
		virtual const std::string& getName() const = 0;

		/** Called when a new frame starts.
		 */
		virtual void startFrame() = 0;

		/** Called when a frame is finished and ready to be displayed.
		 */
		virtual void endFrame() = 0;

		/** Initializes the backend.
		 */
		virtual void init(const std::string& driver) = 0;

		/** Forces a clear of the backbuffer
		 */
		virtual void clearBackBuffer() = 0;

		/** Initializes the light.
		 */
		virtual void setLightingModel(unsigned int lighting) = 0;

		/** Gets the current light model.
		 */
		virtual unsigned int getLightingModel() const = 0;

		/** Enable the lighting.
		 */
		virtual void enableLighting() = 0;

		/** Disable the lighting.
		 */
		virtual void disableLighting() = 0;

		/** Set colors for lighting
		 */
		virtual void setLighting(float red, float green, float blue, float alpha) = 0;

		/** Reset lighting with default values.
		 */
		virtual void resetLighting() = 0;

		/** Enable the stencil test.
		 */
		virtual void enableStencilTest() = 0;

		/** Disable the stencil test.
		 */
		virtual void disableStencilTest() = 0;

		/** Set reference for the stencil test.
		 */
		virtual void setStencilTest(Uint8 stencil_ref, unsigned int stencil_op, unsigned int stencil_func) = 0;

		/** Reset stencil buffer with given value.
		 */
		virtual void resetStencilBuffer(Uint8 buffer) = 0;

		/** Return the reference value for the stencil test.
		 */
		virtual Uint8 getStencilRef() const = 0;

		/** Enable the alpha test.
		 */
		virtual void enableAlphaTest() = 0;

		/** Disable the stencil test.
		 */
		virtual void disableAlphaTest() = 0;

		/** Set reference for the alpha test.
		 */
		virtual void setAlphaTest(float ref_alpha) = 0;

		/** Change the Blendingmodel.
		 */
		virtual void changeBlending(int scr, int dst) = 0;

		/** Performs cleanup actions.
		 */
		virtual void deinit();

		/** Creates the mainscreen (the display window).
		 * @param mode The ScreenMode to use.  @see FIFE::ScreenMode.
		 * @param tite The window title to use.
		 * @param icon The window icon to use.
		 * @return The new Screen Image
		 */
		virtual Image* createMainScreen(const ScreenMode& mode, const std::string& title, const std::string& icon) = 0;

		/** Sets the mainscreen display mode.
		 * @param mode The ScreenMode to change the display to.  @see FIFE::ScreenMode.
		 * @return The new Screen Image
		 */
		virtual Image* setScreenMode(const ScreenMode& mode) = 0;

		/** Creates an Image suitable for this renderbackend.
		 * @param data Pointer to the imagedata (needs to be in RGBA, 8 bits per channel).
		 * @param width Width of the image.
		 * @param height Height of the image.
		 * @return The new Image.
		 */
		virtual Image* createImage(const uint8_t* data, unsigned int width, unsigned int height) = 0;

		/** Helper function to create images from SDL_Surfaces.
		 * Takes ownership over the surface.
		 * @param surface The surface to convert.
		 * @return The new Image.
		 */
		virtual Image* createImage(SDL_Surface* surface) = 0;

		/** Returns a pointer to the main screen Image
		 * @return A pointer to the main screen Image, or 0 if no mainscreen exists.
		 */
		Image* getScreenImage() const { return m_screen; };

		/** Creates a Screenshot and saves it to a file.
		 */
		void captureScreen(const std::string& filename);

		SDL_Surface* getSurface();

		/** Get current screen mode
		 * @return The current screen mode
		 */
		const ScreenMode& getCurrentScreenMode() const;

		unsigned int getWidth() const;
		unsigned int getHeight() const;
		unsigned int getScreenWidth() const { return getWidth(); }
		unsigned int getScreenHeight() const { return getHeight(); }
		const Rect& getArea();
		void getPixelRGBA(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);
		void pushClipArea(const Rect& cliparea, bool clear=true);
		void popClipArea();
		const Rect& getClipArea() const;
		void setAlphaOptimizerEnabled(bool enabled);
		bool isAlphaOptimizerEnabled();
		void saveImage(const std::string& filename);

		/** Sets whether to use the colorkey feature
		*/
		void setColorKeyEnabled(bool colorkeyenable);

		/** Gets whether the colorkey feature is in use
		*/
		bool isColorKeyEnabled() const;

		/** Sets the global colorkey to use for images
		 */
		void setColorKey(const SDL_Color& colorkey);

		/** Gets the global colorkey setting
		 */
		const SDL_Color& getColorKey() const;

	protected:
		Image* m_screen;
		bool m_isalphaoptimized;
		unsigned int m_chunkingsize;
		bool m_iscolorkeyenabled;
		SDL_Color m_colorkey;
		ScreenMode m_screenMode;
	};
}

#endif
