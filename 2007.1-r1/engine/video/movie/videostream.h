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
 
#ifndef FIFE_VIDEO_MOVIE_VIDEOSTREAM_H
#define FIFE_VIDEO_MOVIE_VIDEOSTREAM_H

// I need this strange looking construct, or g++ fails with
// INT64_C undefined
#define INT64_C
#define __STDC_CONSTANT_MACROS

// Standard C++ library includes
#include <string>

// Platform-specific includes
#include "util/fife_stdint.h"

// 3rd party library includes
// strange errors occur when not ffmpeg/ prefix??? WTF!
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

/* Derived from:
 *
 * http://www.inb.uni-luebeck.de/~boehme/avcodec_sample.cpp
 * http://www.gamedev.net/community/forums/topic.asp?topic_id=370896
 * http://webcvs.freedesktop.org/gstreamer/mirror/ffmpeg/ffplay.c?revision=1.9&content-type=text%2Fplain&pathrev=1.9
 *
 * See also: http://www.itdp.de/mplayer-dev-eng/2006-09/msg00120.html
 */

namespace FIFE {
	/** Helper around video stream data.
	 *
	 * @Bug loading from system-fs, not VFS!
	 * @Bug audio-stream ignored!
	 *
	 * You should derive from this class and use the
	 * data in the protected members.
	 *
	 * Call \em load before doing anything else, \em nextFrame
	 * once to switch to the first frame and continue until
	 * it returns false.
	 *
	 * \frame contains the image data.
	 */
	class VideoStream {
		public:
			VideoStream();
			~VideoStream();
			void  load(const std::string & file);
			bool  nextFrame();
			float getFps();
			uint32_t getWidth();
			uint32_t getHeight();

		protected:
			AVFormatContext *formatCtx;
			AVCodecContext  *codecCtx;
			AVCodec         *codec;
			AVFrame         *frame;
			AVFrame         *frameRGB;
			int              vStreamId;

		private:
			void setNull();
			void cleanup();
			uint8_t *vBuffer;
	};
}

#endif
