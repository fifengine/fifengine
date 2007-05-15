#ifndef FIFE_FS_MOVIE_GS_H
#define FIFE_FS_MOVIE_GS_H

#include <string>
#include <SDL_video.h>
#include "util/gamestate/gamestate.h"
#include "videostream.h"

namespace FIFE {
  /** Gamestate to display a video.
   *
   * \Bug only SDL supported
   * \Bug bad frame-blitting function
   */
  class VideoGameState : public GameState {
    public:
      VideoGameState();
      ~VideoGameState();
      void activate();
      void deactivate();
      void turn();
      void setFile(const std::string & file);
    private:
      /** Helper to blit a video-stream frame to a YUV overlay.
       *
       * \Bug assumes the overlay is larger than the video.
       */
      struct SDL_Video_Overlay : public VideoStream {
        SDL_Video_Overlay(SDL_Surface*);
        ~SDL_Video_Overlay();
        SDL_Overlay *yuv_overlay;
        SDL_Surface *screen;
        SDL_Rect     rect;
        void showFrame();
      };
      unsigned long now_t;
      float v_fps;

      SDL_Video_Overlay * sdl_ov;
      std::string filename;
  };
}

#endif
