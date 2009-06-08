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
%module fife

%{
#include "video/image.h"
#include "video/cursor.h"
#include "video/animation.h"
#include "video/imagepool.h"
#include "video/animationpool.h"
#include "video/renderbackend.h"
#include "video/image_location.h"
#include "util/base/exception.h"
%}

%include "util/structures/utilstructures.i"
%include "util/resource/resource.i"

namespace FIFE {
	class Pool;
	class Point;
	class ResourceLocation;

	class AbstractImage {
	public:
		virtual ~AbstractImage() {}
		virtual SDL_Surface* getSurface() = 0;
		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;
		virtual const Rect& getArea() = 0;
		virtual void getPixelRGBA(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) = 0;
 		virtual bool putPixel(int x, int y, int r, int g, int b) = 0;
		virtual void drawLine(const Point& p1, const Point& p2, int r, int g, int b) = 0;
		virtual void drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  int r, int g, int b) = 0;
		virtual void pushClipArea(const Rect& cliparea, bool clear=true) = 0;
		virtual void popClipArea() = 0;
		virtual const Rect& getClipArea() const = 0;
		virtual void saveImage(const std::string& filename) = 0;
		virtual void setAlphaOptimizerEnabled(bool enabled) = 0;
		virtual bool isAlphaOptimizerEnabled() = 0;
	};
	
	class Image : public AbstractImage, public ResourceClass {
	public:
		void render(const Rect& rect, unsigned char alpha = 255);
		virtual ~Image();
		SDL_Surface* getSurface() { return m_surface; }
		unsigned int getWidth() const;
		unsigned int getHeight() const;
		const Rect& getArea();
		void setXShift(int xshift);
		inline int getXShift() const;
		void setYShift(int yshift);
		inline int getYShift() const;
		void getPixelRGBA(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);
		void pushClipArea(const Rect& cliparea, bool clear=true);
		void popClipArea();
		const Rect& getClipArea() const;
		void setAlphaOptimizerEnabled(bool enabled);
		bool isAlphaOptimizerEnabled();
		void addRef();
		void decRef();
		unsigned int getRefCount();
		
	private:
		Image(SDL_Surface* surface);
		Image(const uint8_t* data, unsigned int width, unsigned int height);
	};
	
	class Animation: public ResourceClass {
	public:
		explicit Animation();
		~Animation();
		void addFrame(ResourcePtr image, unsigned int duration);
		int getFrameIndex(unsigned int timestamp);
		Image* getFrame(int index);
		Image* getFrameByTimestamp(unsigned int timestamp);
		int getFrameDuration(int index);
		unsigned int getNumFrames() const;
		void setActionFrame(int num);
		int getActionFrame();
		void setDirection(unsigned int direction);
		unsigned int getDirection();
		int getDuration();
		void addRef();
		void decRef();
		unsigned int getRefCount();
	};

	class ImageLocation: public ResourceLocation {
	public:
		ImageLocation(const std::string& filename);
		virtual ~ImageLocation() {};
		virtual void setXShift(int xshift) { m_xshift = xshift; }
		virtual int getXShift() const { return m_xshift; }
		virtual void setYShift(int yshift) { m_yshift = yshift; }
		virtual int getYShift() const { return m_yshift; }
		virtual void setWidth(unsigned int width) { m_width = width; }
		virtual unsigned int getWidth() const { return m_width; }
		virtual void setHeight(unsigned int height) { m_height = height; }
		virtual unsigned int getHeight() const { return m_height; }
		virtual void setParentSource(Image* image) { m_parent_image = image; }
		virtual Image* getParentSource() const { return m_parent_image; }
	};

	class ImagePool: public Pool {
	public:
		virtual ~ImagePool();
		inline Image& getImage(unsigned int index);
	private:
		ImagePool();
	};

	class AnimationPool: public Pool {
	public:
		virtual ~AnimationPool();
		inline Animation& getAnimation(unsigned int index);
	private:
		AnimationPool();
	};

	class RenderBackend: public AbstractImage {
	public:
		virtual ~RenderBackend();
		virtual const std::string& getName() const = 0;
		Image* getScreenImage() const { return m_screen; };
		void captureScreen(const std::string& filename);
		SDL_Surface* getSurface();
		unsigned int getWidth() const;
		unsigned int getHeight() const;
		unsigned int getScreenWidth() const { return getWidth(); }
		unsigned int getScreenHeight() const { return getHeight(); }
		const Rect& getArea();
		void getPixelRGBA(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);
 		bool putPixel(int x, int y, int r, int g, int b);
		void drawLine(const Point& p1, const Point& p2, int r, int g, int b);
		void drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  int r, int g, int b);
		void pushClipArea(const Rect& cliparea, bool clear=true);
		void popClipArea();
		const Rect& getClipArea() const;
		void setAlphaOptimizerEnabled(bool enabled);
		bool isAlphaOptimizerEnabled();
		void saveImage(const std::string& filename);
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
		void set(MouseCursorType ctype, unsigned int cursor_id=0);
		void setDrag(MouseCursorType ctype, unsigned int drag_id=0, int drag_offset_x=0, int drag_offset_y=0);
		MouseCursorType getType() const;
		unsigned int getId() const;
		MouseCursorType getDragType() const;
		unsigned int getDragId() const;
	
	private:
		Cursor(ImagePool* imgpool, AnimationPool* animpool);
	};
	
}
