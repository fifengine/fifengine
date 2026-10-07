// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "videowidget.h"

// Standard C++ library includes
#include <memory>
#include <string>
#include <utility>

// Platform specific includes

// 3rd party library includes

// FIFE includes
#include "gui/fifechan/base/gui_image.h"
#include "util/base/exception.h"
#include "util/time/timemanager.h"
#include "video/image.h"
#include "video/movie/movieplayer.h"

namespace fcn
{
    VideoWidget::VideoWidget() :
        mTimemanager(FIFE::TimeManager::instance()),
        mOwnsPlayer(false),
        mStartTime(0),
        mFrameTime(-1.0),
        mLooping(false),
        mPlay(false)
    {
        setScaling(false);
        setTiling(false);
        setOpaque(true);
        adjustSize();
    }

    VideoWidget::VideoWidget(std::string const & path) : VideoWidget()
    {
        setMovie(path);
    }

    VideoWidget::~VideoWidget()
    {
        clearMovie();
    }

    void VideoWidget::setMovie(std::string const & path)
    {
        clearMovie();
        mPlayer     = std::make_shared<FIFE::MoviePlayer>();
        mOwnsPlayer = true;
        mPlayer->load(path);
        mPlayer->setLooping(mLooping);

        // Sized explicitly: adjustSize() would collapse to zero because no
        // frame image exists yet.
        setWidth(static_cast<int>(mPlayer->getWidth()));
        setHeight(static_cast<int>(mPlayer->getHeight()));

        play();
    }

    void VideoWidget::setMoviePlayer(std::shared_ptr<FIFE::MoviePlayer> player)
    {
        clearMovie();
        mPlayer     = std::move(player);
        mOwnsPlayer = false;
        if (mPlayer) {
            setWidth(static_cast<int>(mPlayer->getWidth()));
            setHeight(static_cast<int>(mPlayer->getHeight()));
        }
    }

    std::shared_ptr<FIFE::MoviePlayer> VideoWidget::getMoviePlayer() const
    {
        return mPlayer;
    }

    void VideoWidget::clearMovie()
    {
        if (mPlayer) {
            mPlayer->stop();
            if (mOwnsPlayer) {
                mPlayer->unload();
            }
        }
        mPlayer.reset();
        mCurrentImage.reset();
        setImage(nullptr);
        mOwnsPlayer = false;
        mPlay       = false;
        mFrameTime  = -1.0;
        mStartTime  = 0;
    }

    void VideoWidget::setLooping(bool looping)
    {
        mLooping = looping;
        if (mPlayer) {
            mPlayer->setLooping(looping);
        }
    }

    bool VideoWidget::isLooping() const
    {
        return mLooping;
    }

    void VideoWidget::play()
    {
        if (!mPlayer) {
            return;
        }
        mStartTime = mTimemanager->now64();
        mFrameTime = -1.0;
        mPlay      = true;
        mPlayer->rewind();
        mPlayer->play(static_cast<double>(mStartTime) / 1000.0);
    }

    void VideoWidget::pause()
    {
        mPlay = false;
    }

    void VideoWidget::stop()
    {
        if (!mPlayer) {
            return;
        }
        mPlay = false;
        mPlayer->stop();
    }

    bool VideoWidget::isPlaying() const
    {
        return mPlay && mPlayer != nullptr;
    }

    bool VideoWidget::hasAudio() const
    {
        return mPlayer && mPlayer->hasAudio();
    }

    bool VideoWidget::isFinished() const
    {
        return mPlayer != nullptr && mPlayer->isFinished();
    }

    double VideoWidget::getDuration() const
    {
        return mPlayer ? mPlayer->getDuration() : 0.0;
    }

    void VideoWidget::logic()
    {
        if (!mPlay || !mPlayer || !mPlayer->isLoaded()) {
            return;
        }

        double const now = static_cast<double>(mTimemanager->now64()) / 1000.0;
        mPlayer->update(now);

        if (mPlayer->isFinished()) {
            mPlay = false;
            return;
        }

        // Only rebuild the GuiImage when a new frame is actually showing,
        // otherwise this would allocate every frame.
        double const frameTime = mPlayer->getFrameTimestamp();
        if (frameTime == mFrameTime) {
            return;
        }

        FIFE::ImagePtr const frame = mPlayer->getFrameImage();
        if (!frame) {
            return;
        }
        mFrameTime    = frameTime;
        mCurrentImage = std::make_unique<FIFE::GuiImage>(frame);
        setImage(mCurrentImage.get());
    }
} // namespace fcn
