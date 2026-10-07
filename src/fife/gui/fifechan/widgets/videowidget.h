// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GUI_WIDGETS_VIDEOWIDGET_H
#define FIFE_GUI_WIDGETS_VIDEOWIDGET_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstdint>
#include <memory>
#include <string>
// 3rd party library includes
#include <fifechan/image.hpp>
#include <fifechan/platform.hpp>

#include <fifechan.hpp>

// FIFE includes
#include "util/base/fife_stdint.h"

namespace FIFE
{
    class MoviePlayer;
    class TimeManager;
} // namespace FIFE

namespace fcn
{

    /**
     * Icon that plays a movie inside a GUI element.
     *
     * Frames are decoded by a FIFE::MoviePlayer and handed to the inherited
     * fcn::Icon drawing path, so no codec support is needed in FifeChan itself.
     *
     * The widget does not letterbox: the frame is drawn 1:1 and sized to the
     * movie, so the surrounding container decides how to fit it.
     */
    class FIFE_API VideoWidget : public Icon
    {
        public:
            /**
             * Default constructor, no movie loaded.
             */
            VideoWidget();

            /**
             * Constructor.
             *
             * @param path VFS path of the movie file
             * @throws FIFE::NotFound if the file cannot be read
             * @throws FIFE::InvalidFormat if it is not a movie this engine understands
             */
            explicit VideoWidget(std::string const & path);

            virtual ~VideoWidget();

            VideoWidget(VideoWidget const &)            = delete;
            VideoWidget& operator=(VideoWidget const &) = delete;

            /**
             * Loads a movie and resizes the widget to match.
             *
             * @param path VFS path of the movie file
             * @throws FIFE::NotFound if the file cannot be read
             * @throws FIFE::InvalidFormat if it is not a movie this engine understands
             */
            void setMovie(std::string const & path);

            /**
             * Uses an externally driven movie player.
             *
             * Useful when the player is shared, for example with the engine's
             * fullscreen playback. The player is not stopped when the widget is
             * destroyed.
             */
            void setMoviePlayer(std::shared_ptr<FIFE::MoviePlayer> player);

            /**
             * Gets the movie player in use, may be null.
             */
            std::shared_ptr<FIFE::MoviePlayer> getMoviePlayer() const;

            /**
             * Releases the movie and the decoded frame.
             */
            void clearMovie();

            /**
             * Sets whether playback restarts at the end.
             */
            void setLooping(bool looping);

            /**
             * Gets whether playback restarts at the end.
             */
            bool isLooping() const;

            /**
             * Starts playback from the beginning.
             */
            void play();

            /**
             * Stops playback at the current frame.
             */
            void pause();

            /**
             * Stops playback and returns to the first frame.
             */
            void stop();

            /**
             * Gets if the movie is playing.
             */
            bool isPlaying() const;

            /**
             * Gets if the movie played to the end without looping.
             */
            bool isFinished() const;

            /**
             * Gets if the movie has a usable audio track.
             */
            bool hasAudio() const;

            /**
             * Gets the length of the movie in seconds.
             */
            double getDuration() const;

            // Inherited from Widget

            virtual void logic();

        protected:
            /**
             * Holds pointer to Fifes TimeManager.
             */
            FIFE::TimeManager* mTimemanager;

            /**
             * The player driving the decoded frames.
             */
            std::shared_ptr<FIFE::MoviePlayer> mPlayer;

            /**
             * True if this widget created the player and should stop it.
             */
            bool mOwnsPlayer;

            /**
             * Currently used image, the current frame wrapped in a GuiImage.
             */
            std::unique_ptr<Image const> mCurrentImage;

            /**
             * The time as playback was started.
             */
            uint64_t mStartTime;

            /**
             * The timestamp of the frame currently displayed.
             */
            double mFrameTime;

            /**
             * True if the movie should restart at the end.
             */
            bool mLooping;

            /**
             * True if playback was started, otherwise false.
             */
            bool mPlay;
    };
} // namespace fcn

#endif
