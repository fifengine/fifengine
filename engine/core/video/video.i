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
%}

namespace FIFE {
	class Image {
	public:
		virtual ~Image();
		virtual void setXShift(int xshift);
		virtual void setYShift(int yshift);
		virtual int getXShift() const;
		virtual int getYShift() const;
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
		void setActionFrame(int num) { m_action_frame = num; }
		int getActionFrame() { return m_action_frame; }
		int getDuration() { return m_animation_endtime; }
		void addRef();
		void decRef();
		unsigned int getRefCount();
	private:
		explicit Animation();
	};

	class ImagePool {
	public:
		virtual ~ImagePool();
		Image& getImage(unsigned int index);
	private:
		ImagePool();
	};

	class AnimationPool {
	public:
		virtual ~AnimationPool();
		Animation& getAnimation(unsigned int index);
	private:
		AnimationPool();
	};

	class RenderBackend {
	public:
		virtual ~RenderBackend();
		unsigned int getScreenWidth() const { return m_screen->w; }
		unsigned int getScreenHeight() const { return m_screen->h; }
		virtual void captureScreen(const std::string& filename);
	private:
		RenderBackend(const std::string& name);
	};
}