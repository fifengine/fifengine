/***************************************************************************
 *   Copyright (C) 2005-2010 by the FIFE team                              *
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
%module fife

%{
#include "video/image.h"
#include "video/cursor.h"
#include "video/animation.h"
#include "video/imagemanager.h"
#include "video/renderbackend.h"
#include "video/devicecaps.h"
#include "util/base/sharedptr.h"
#include "util/base/exception.h"
%}

%include "util/structures/utilstructures.i"
%include "util/resource/resource.i"

namespace FIFE {
  class ScreenMode;
}

namespace std {
	%template(ScreenModeVector) std::vector<FIFE::ScreenMode>;
}

namespace FIFE {
	class Point;
	class ResourceLocation;

	%apply uint8_t *OUTPUT { uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a };

	class IImage {
	public:
		virtual ~IImage() {}
		virtual SDL_Surface* getSurface() = 0;
		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;
		virtual const Rect& getArea() = 0;
		virtual void getPixelRGBA(int32_t x, int32_t y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) = 0;
 		virtual bool putPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;
		virtual void drawLine(const Point& p1, const Point& p2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;
		virtual void drawTriangle(const Point& p1, const Point& p2, const Point& p3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;
		virtual void drawRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;
		virtual void fillRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;
		virtual void drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;
		virtual void drawLightPrimitive(const Point& p, uint8_t intensity, float radius, int32_t subdivisions, float xstretch, float ystretch, uint8_t red, uint8_t green, uint8_t blue);
		virtual void pushClipArea(const Rect& cliparea, bool clear=true) = 0;
		virtual void popClipArea() = 0;
		virtual const Rect& getClipArea() const = 0;
		virtual void saveImage(const std::string& filename) = 0;
		virtual void setAlphaOptimizerEnabled(bool enabled) = 0;
		virtual bool isAlphaOptimizerEnabled() = 0;
	};
	
	class Image : public IResource, public IImage {
	public:
		void render(const Rect& rect, uint8_t alpha = 255);
		virtual ~Image();
		SDL_Surface* getSurface() { return m_surface; }
		uint32_t getWidth() const;
		uint32_t getHeight() const;
		const Rect& getArea();
		void setXShift(int32_t xshift);
		inline int32_t getXShift() const;
		void setYShift(int32_t yshift);
		inline int32_t getYShift() const;
		void getPixelRGBA(int32_t x, int32_t y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);
		void pushClipArea(const Rect& cliparea, bool clear=true);
		void popClipArea();
		const Rect& getClipArea() const;
		void setAlphaOptimizerEnabled(bool enabled);
		bool isAlphaOptimizerEnabled();
		
	private:
		Image(SDL_Surface* surface);
		Image(const uint8_t* data, uint32_t width, uint32_t height);
	};
	
	typedef SharedPtr<Image> ImagePtr;
	
	%template(SharedImagePointer) SharedPtr<Image>;
	
	class ImageManager : public IResourceManager {
	public:
		virtual ~ImageManager();
		
		virtual size_t getMemoryUsed() const;
		virtual size_t getTotalResourcesCreated() const;
		virtual size_t getTotalResourcesLoaded() const;
		virtual size_t getTotalResources() const;

		virtual ImagePtr create(const std::string& name, IResourceLoader* loader = 0);
		virtual ImagePtr load(const std::string& name, IResourceLoader* loader = 0);		
		virtual ImagePtr add(Image* res);

		virtual bool exists(const std::string& name);
		virtual bool exists(ResourceHandle handle);

		virtual void reload(const std::string& name);
		virtual void reload(ResourceHandle handle);
		virtual void reloadAll();
		virtual void loadUnreferenced();

		virtual void free(const std::string& name);
		virtual void free(ResourceHandle handle);
		virtual void freeAll();
		virtual void freeUnreferenced();

		virtual void remove(ImagePtr& resource);
		virtual void remove(const std::string& name);
		virtual void remove(ResourceHandle handle);
		virtual void removeAll();
		virtual void removeUnreferenced();

		virtual ImagePtr get(const std::string& name);
		virtual ImagePtr get(ResourceHandle handle);

		virtual ResourceHandle getResourceHandle(const std::string& name);
		
	};
	
	
	class Animation: public FifeClass {
	public:
		explicit Animation();
		~Animation();
		void addFrame(ImagePtr image, uint32_t duration);
		int32_t getFrameIndex(uint32_t timestamp);
		uint32_t getFrame(int32_t index);
		uint32_t getFrameByTimestamp(uint32_t timestamp);
		int32_t getFrameDuration(int32_t index);
		uint32_t getFrameCount() const;
		void setActionFrame(int32_t num);
		int32_t getActionFrame();
		void setDirection(uint32_t direction);
		uint32_t getDirection();
		int32_t getDuration();
	};
	
	typedef SharedPtr<Animation> AnimationPtr;	
	%template(SharedAnimationPointer) SharedPtr<Animation>;

	class RenderBackend: public IImage {
	public:
		virtual ~RenderBackend();
		virtual const std::string& getName() const = 0;
		
		Image* getScreenImage() const { return m_screen; };
		void captureScreen(const std::string& filename);
		SDL_Surface* getSurface();
		const ScreenMode& getCurrentScreenMode() const;
		uint32_t getWidth() const;
		uint32_t getHeight() const;
		uint32_t getScreenWidth() const { return getWidth(); }
		uint32_t getScreenHeight() const { return getHeight(); }
		const Rect& getArea();
		void getPixelRGBA(int32_t x, int32_t y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);
		const Rect& getClipArea() const;
		void setAlphaOptimizerEnabled(bool enabled);
		bool isAlphaOptimizerEnabled();
		void saveImage(const std::string& filename);
		void setColorKeyEnabled(bool colorkeyenable);
		bool isColorKeyEnabled() const;
		void setColorKey(const SDL_Color& colorkey);
		const SDL_Color& getColorKey() const;
		const SDL_PixelFormat& getPixelFormat() const;
		void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
		void resetBackgroundColor();
		void setFrameLimitEnabled(bool limited);
		bool isFrameLimitEnabled() const;
		void setFrameLimit(uint16_t framelimit);
		uint16_t getFrameLimit() const;
	};
	
	enum MouseCursorType {
		CURSOR_NONE,
		CURSOR_NATIVE,
		CURSOR_IMAGE,
		CURSOR_ANIMATION
	};

	enum NativeCursor {
		NC_ARROW = 1000000,
		NC_IBEAM,
		NC_WAIT,
		NC_CROSS,
		NC_UPARROW,
		NC_RESIZENW,
		NC_RESIZESE,
		NC_RESIZESW,
		NC_RESIZENE,
		NC_RESIZEE,
		NC_RESIZEW,
		NC_RESIZEN,
		NC_RESIZES,
		NC_RESIZEALL,
		NC_NO,
		NC_HAND,
		NC_APPSTARTING,
		NC_HELP
	};
	
	class Cursor {
	public:
		virtual ~Cursor() {}
		virtual void draw();
		void set(MouseCursorType ctype, uint32_t cursor_id=0);
		void set(AnimationPtr anim);
		void setDrag(MouseCursorType ctype, uint32_t drag_id=0, int32_t drag_offset_x=0, int32_t drag_offset_y=0);
		void setDrag(AnimationPtr anim, int32_t drag_offset_x=0, int32_t drag_offset_y=0);
		MouseCursorType getType() const;
		uint32_t getId() const;
		AnimationPtr getAnimation();
		MouseCursorType getDragType() const;
		uint32_t getDragId() const;
		AnimationPtr getDragAnimation();
		uint32_t getX() const;
		uint32_t getY() const;
	
	private:
		Cursor();
	};
	
	class ScreenMode {
	public:
		~ScreenMode();

		uint16_t getWidth() const;
		uint16_t getHeight() const;
		uint16_t getBPP() const;
		uint32_t getSDLFlags() const;
		bool isFullScreen();
		bool isOpenGL();
		bool isSDL() const;
		bool isSDLHardwareSurface() const;
		
		static const uint32_t HW_WINDOWED_OPENGL;
		static const uint32_t HW_FULLSCREEN_OPENGL;
		static const uint32_t WINDOWED_SDL;
		static const uint32_t WINDOWED_SDL_DB_HW;
		static const uint32_t FULLSCREEN_SDL;
		static const uint32_t FULLSCREEN_SDL_DB_HW;
	};

	class DeviceCaps {
	public:
		DeviceCaps();
		~DeviceCaps();

		void fillDeviceCaps();
		std::vector<ScreenMode> getSupportedScreenModes() const;
		ScreenMode getNearestScreenMode(uint16_t width, uint16_t height, uint16_t bpp, const std::string& renderer, bool fs) const;
		std::string getDriverName() const;
		std::vector<string> getAvailableDrivers() const;
		bool isHwSurfaceAvail() const;
		bool isWindowManagerAvail() const;
		bool isHwBlitAccel() const;
		bool isHwColorkeyBlitAccel() const;
		bool isHwAlphaBlitAccel() const;
		bool isSwToHwBlitAccel() const;
		bool isSwToHwColorkeyBlitAccel() const;
		bool isSwToHwAlphaBlitAccel() const;
		bool isBlitFillAccel() const;
		
		uint32_t getVideoMemory() const;
	};
}
