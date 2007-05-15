#ifndef FIFE_VIDEO_STREAM_H
#define FIFE_VIDEO_STREAM_H

// I need this strange looking construct, or g++ fails with
// INT64_C undefined
#define INT64_C
#define __STDC_CONSTANT_MACROS
#include <stdint.h>

#include <string>

// strange errors occur when not ffmpeg/ prefix??? WTF!
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>

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
