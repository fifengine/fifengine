# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""SWIG surface tests for the optional ENABLE_MOVIE feature.

Skipped entirely in a default build: ENABLE_MOVIE is OFF there, so none of the
movie types exist in the bindings.
"""

import pytest

from fife import fife

MOVIE_FILE = "tests/data/movie-test.webm"

# 320x240, 25 fps, ~2.01s of VP9 + Opus, see tests/core_tests/test_movie.cpp
MOVIE_WIDTH = 320
MOVIE_HEIGHT = 240
MOVIE_FRAMERATE = 25.0
MOVIE_DURATION = 2.01

pytestmark = pytest.mark.skipif(
    not hasattr(fife, "MoviePlayer"),
    reason="built without ENABLE_MOVIE",
)


# One engine for the whole module, see the shared_engine fixture in conftest.py.
# Several tests here need the VFS and the render backend, and only a single
# Engine can be alive at a time.
engine = pytest.mark.usefixtures("shared_engine")


def test_movie_types_are_exposed():
    assert hasattr(fife, "MoviePlayer")
    assert hasattr(fife, "MovieDecoder")
    assert hasattr(fife, "VideoWidget")

    for method in ("playMovie", "stopMovie", "isMoviePlaying", "getMovieTime"):
        assert hasattr(fife.Engine, method), method


def test_movie_source_is_not_exposed():
    # Both are only constructible from a shared MovieSource, and wrapping
    # std::shared_ptr would need SWIG stdlib support the rest of the interface
    # does not use. Guarded deliberately, so this test fails if that changes.
    assert not hasattr(fife, "MovieSource")
    assert not hasattr(fife, "MovieAudioDecoder")


def test_video_widget_does_not_expose_its_player():
    # getMoviePlayer() hands out a non-owning pointer into an object the widget
    # owns, so a Python handle could outlive it. hasAudio() is the replacement.
    assert not hasattr(fife.VideoWidget, "getMoviePlayer")
    assert hasattr(fife.VideoWidget, "hasAudio")


def test_movie_decoder_reports_container_properties(engine):
    decoder = fife.MovieDecoder(MOVIE_FILE)

    assert decoder.getWidth() == MOVIE_WIDTH
    assert decoder.getHeight() == MOVIE_HEIGHT
    assert decoder.getFrameRate() == pytest.approx(MOVIE_FRAMERATE)
    assert decoder.getDuration() == pytest.approx(MOVIE_DURATION, abs=0.1)


def test_movie_decoder_next_frame_and_seek(engine):
    decoder = fife.MovieDecoder(MOVIE_FILE)

    # The first frame sits at t=0, and its pixels must be readable, otherwise
    # MoviePlayer has nothing to upload.
    assert decoder.nextFrame()
    assert decoder.getFrameTimestamp() == pytest.approx(0.0, abs=0.001)
    assert decoder.getFrameData() is not None

    # Seeking lands on a keyframe at or before the target, so decoding has to
    # run forward from there rather than jumping straight to the wanted time.
    assert decoder.seek(1.0)
    skipped = 0
    reached = None
    while decoder.nextFrame():
        if decoder.getFrameTimestamp() >= 1.0:
            reached = decoder.getFrameTimestamp()
            break
        skipped += 1
    assert skipped > 0
    assert reached == pytest.approx(1.0, abs=0.1)

    # Seeking back must land before the timestamp reached above.
    assert decoder.seek(0.0)
    since_rewind = 0
    while decoder.nextFrame() and decoder.getFrameTimestamp() < 0.5:
        since_rewind += 1
    assert 0 < since_rewind < skipped


def test_movie_decoder_rejects_non_movie_data(engine):
    with pytest.raises(fife.InvalidFormat):
        fife.MovieDecoder("tests/data/rpg_tiles_01.png")


def test_movie_decoder_reports_missing_files(engine):
    # MovieSource reads through VFS::open, which raises NotFound.
    with pytest.raises(fife.NotFound):
        fife.MovieDecoder("tests/data/does-not-exist.webm")


def test_movie_player_transport(engine):
    player = fife.MoviePlayer()

    assert not player.isLoaded()
    assert not player.isFinished()
    assert not player.isLooping()
    assert not player.hasAudio()

    player.load(MOVIE_FILE)
    assert player.isLoaded()
    assert player.getWidth() == MOVIE_WIDTH
    assert player.getHeight() == MOVIE_HEIGHT
    assert player.getDuration() == pytest.approx(MOVIE_DURATION, abs=0.1)
    assert player.hasAudio()

    # Playing and updating must produce a frame without a render backend being
    # involved; this is the same call path Engine::pump() drives.
    player.play(0.0)
    player.update(1.0)
    frame = player.getFrameImage()
    assert frame is not None
    assert frame.getWidth() == MOVIE_WIDTH
    assert frame.getHeight() == MOVIE_HEIGHT

    # Past the end the player must finish, and stop() must clear that again.
    player.update(MOVIE_DURATION + 1.0)
    assert player.isFinished()

    player.stop()
    assert not player.isFinished()

    player.setLooping(True)
    assert player.isLooping()

    player.unload()
    assert not player.isLoaded()


def test_movie_player_loops_instead_of_finishing(engine):
    player = fife.MoviePlayer()
    player.load(MOVIE_FILE)
    player.setLooping(True)
    player.play(0.0)

    player.update(MOVIE_DURATION + 1.0)
    assert player.isLooping()
    assert not player.isFinished()


def test_movie_player_sound_clip_is_registered(engine):
    player = fife.MoviePlayer()
    player.load(MOVIE_FILE)
    player.play(0.0)

    # The audio track is handed to OpenAL as a SoundClip, so Python can inspect
    # it even though MovieAudioDecoder itself is not wrapped.
    # It is a smart pointer, so the clip itself is reached via get().
    clip = player.getSoundClip()
    assert clip is not None
    sound_clip = clip.get()
    assert sound_clip is not None
    # The decoder was adopted, so the clip decodes on demand rather than
    # streaming: isStream() is false and the name carries the movie path.
    assert not sound_clip.isStream()
    assert "movie-test.webm" in sound_clip.getName()

    # Asking again must hand back the same clip, not build a new one. SWIG wraps
    # every returned pointer in a fresh Python proxy, so identity is compared on
    # the underlying handle rather than with "is".
    assert player.getSoundClip().getHandle() == clip.getHandle()


def test_video_widget_playback(engine):
    widget = fife.VideoWidget(MOVIE_FILE)

    assert widget.hasAudio()
    assert widget.getDuration() == pytest.approx(MOVIE_DURATION, abs=0.1)
    assert widget.getWidth() == MOVIE_WIDTH
    assert widget.getHeight() == MOVIE_HEIGHT

    # setMovie() starts playback, so attaching is enough to get frames.
    assert widget.isPlaying()
    assert not widget.isFinished()

    widget.pause()
    assert not widget.isPlaying()

    widget.play()
    assert widget.isPlaying()

    widget.setLooping(True)
    assert widget.isLooping()

    widget.stop()
    assert not widget.isPlaying()
    widget.play()
    assert widget.isPlaying()

    # The widget publishes the current frame as the icon image it blits.
    for _ in range(10):
        widget.logic()
        if widget.getImage() is not None:
            break
    image = widget.getImage()
    assert image is not None
    assert image.getWidth() == MOVIE_WIDTH
    assert image.getHeight() == MOVIE_HEIGHT

    widget.clearMovie()
    assert widget.getImage() is None


def test_engine_plays_a_movie_fullscreen(engine):
    assert not engine.isMoviePlaying()
    # Negative means no movie is loaded at all, which is not the same as 0.
    assert engine.getMovieTime() < 0.0

    engine.playMovie(MOVIE_FILE)
    assert engine.isMoviePlaying()

    # getMovieTime() tracks playback, so it must advance across pumps. It only
    # moves once the engine has pumped, since the movie origin is captured on
    # the first pump, and the first pump therefore still reports 0.
    assert engine.getMovieTime() == pytest.approx(0.0, abs=0.001)
    previous = engine.getMovieTime()
    for _ in range(50):
        engine.pump()
        if engine.getMovieTime() > previous:
            break
    assert engine.getMovieTime() > previous

    engine.stopMovie()
    assert not engine.isMoviePlaying()


def test_engine_stays_idle_without_a_movie(engine):
    # An idle engine must not report a movie or move the movie clock, even after
    # the module has played one, so stopMovie() really did release it.
    engine.pump()
    assert not engine.isMoviePlaying()
    assert engine.getMovieTime() < 0.0


def test_engine_play_movie_reports_bad_input(engine):
    with pytest.raises(fife.NotFound):
        engine.playMovie("tests/data/does-not-exist.webm")

    with pytest.raises(fife.InvalidFormat):
        engine.playMovie("tests/data/rpg_tiles_01.png")

    # A failed playMovie must not leave the engine in a playing state.
    assert not engine.isMoviePlaying()
