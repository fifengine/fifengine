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

#ifndef FIFE_VIDEO_RENDERBACKENDS_SDL_RENDERBACKENDSDL_H
#define FIFE_VIDEO_RENDERBACKENDS_SDL_RENDERBACKENDSDL_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"

namespace FIFE {

	class ScreenMode;

	/** The main class of the SDL-based renderer.
	 *
	 * @see RenderBackend
	 */
	class RenderBackendSDL : public RenderBackend {
	public:
		RenderBackendSDL(const SDL_Color& colorkey);
		virtual ~RenderBackendSDL();
		virtual const std::string& getName() const;
		virtual void startFrame();
		virtual void endFrame();
		virtual void init(const std::string& driver);
		virtual void clearBackBuffer();
		virtual void setLightingModel(uint32_t lighting);
		virtual uint32_t getLightingModel() const;
		virtual void setLighting(float red, float green, float blue);
		virtual void resetLighting();
		virtual void resetStencilBuffer(uint8_t buffer);
		virtual void changeBlending(int32_t scr, int32_t dst);

		virtual void createMainScreen(const ScreenMode& mode, const std::string& title, const std::string& icon);
		virtual void setScreenMode(const ScreenMode& mode);

		virtual Image* createImage(IResourceLoader* loader = 0);
		virtual Image* createImage(const std::string& name, IResourceLoader* loader = 0);
		virtual Image* createImage(const uint8_t* data, uint32_t width, uint32_t height);
		virtual Image* createImage(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height);
		virtual Image* createImage(SDL_Surface* surface);
		virtual Image* createImage(const std::string& name, SDL_Surface* surface);

		virtual void renderVertexArrays();
		virtual void addImageToArray(uint32_t id, const Rect& rec, float const* st, uint8_t alpha, uint8_t const* rgba);
		virtual void changeRenderInfos(RenderDataType type, uint16_t elements, int32_t src, int32_t dst, bool light, bool stentest, uint8_t stenref, GLConstants stenop, GLConstants stenfunc, OverlayType otype = OVERLAY_TYPE_NONE);
		virtual void captureScreen(const std::string& filename);
		virtual void captureScreen(const std::string& filename, uint32_t width, uint32_t height);

		virtual bool putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawLine(const Point& p1, const Point& p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawThickLine(const Point& p1, const Point& p2, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawPolyLine(const std::vector<Point>& points, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawBezier(const std::vector<Point>& points, int32_t steps, uint8_t width, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawTriangle(const Point& p1, const Point& p2, const Point& p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void fillRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawVertex(const Point& p, const uint8_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawCircle(const Point& p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawFillCircle(const Point& p, uint32_t radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawCircleSegment(const Point& p, uint32_t radius, int32_t sangle, int32_t eangle, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawFillCircleSegment(const Point& p, uint32_t radius, int32_t sangle, int32_t eangle, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
		virtual void drawLightPrimitive(const Point& p, uint8_t intensity, float radius, int32_t subdivisions, float xstretch, float ystretch, uint8_t red, uint8_t green, uint8_t blue);

		virtual void enableScissorTest();
		virtual void disableScissorTest();
		
		virtual void attachRenderTarget(ImagePtr& img, bool discard);
		virtual void detachRenderTarget();

		virtual void renderGuiGeometry(const std::vector<GuiVertex>& vertices, const std::vector<int>& indices, const DoublePoint& translation, ImagePtr texture);

		SDL_Renderer* getRenderer() { return m_renderer; }
		
	protected:
		virtual void setClipArea(const Rect& cliparea, bool clear);

		SDL_Renderer* m_renderer;
	};

}

#endif
