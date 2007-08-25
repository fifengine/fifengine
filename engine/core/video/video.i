/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/
%module video

%{
#include "video/image.h"
#include "video/animation.h"
#include "video/imagepool.h"
#include "video/animationpool.h"
#include "video/renderbackend.h"
#include "util/exception.h"
%}

namespace FIFE {
	class Pool;
	class Point;

	class Image {
	public:
		virtual ~Image();
		void setXShift(int xshift);
		void setYShift(int yshift);
		int getXShift() const;
		int getYShift() const;
		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;
		void addRef();
		void decRef();
		unsigned int getRefCount();
	private:
		Image(SDL_Surface* surface);
	};

	class Animation {
	public:
		~Animation();
		void addFrame(Image* image, unsigned int duration);
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
	private:
		explicit Animation();
	};

	class ImagePool: public Pool {
	public:
		virtual ~ImagePool();
		Image& getImage(unsigned int index) throw(NotFound);
	private:
		ImagePool();
	};

	class AnimationPool: public Pool {
	public:
		virtual ~AnimationPool();
		Animation& getAnimation(unsigned int index) throw(NotFound);
	private:
		AnimationPool();
	};

	class RenderBackend {
	public:
		virtual ~RenderBackend();
		unsigned int getScreenWidth() const;
		unsigned int getScreenHeight() const;
		virtual void captureScreen(const std::string& filename);
		virtual void drawLine(const Point& p1, const Point& p2, int r, int g, int b);
	private:
		RenderBackend(const std::string& name);
	};
}