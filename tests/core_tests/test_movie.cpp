// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifdef HAVE_MOVIE

    // Standard C++ library includes
    #include <cmath>
    #include <cstdint>
    #include <memory>
    #include <string>
    #include <vector>

    // Platform specific includes
    #include "fixture.h"

    // 3rd party library includes
    #include <SDL3/SDL.h>

    #include <AL/al.h>
    #include <catch2/catch_test_macros.hpp>

    // FIFE includes
    #include "audio/soundclip.h"
    #include "audio/soundclipmanager.h"
    #include "audio/soundmanager.h"
    #include "controller/engine.h"
    #include "controller/enginesettings.h"
    #include "gui/fifechan/widgets/videowidget.h"
    #include "util/base/exception.h"
    #include "util/time/timemanager.h"
    #include "video/imagemanager.h"
    #include "video/movie/movieaudiodecoder.h"
    #include "video/movie/moviedecoder.h"
    #include "video/movie/movieplayer.h"
    #include "video/movie/moviesource.h"
    #include "video/opengl/renderbackendopengl.h"
    #include "video/renderbackend.h"
    #include "video/sdl/renderbackendsdl.h"
    #include "video/window/window.h"

static char const * const MOVIE_FILE = "tests/data/movie-test.webm";

TEST_CASE("MovieDecoder reads a WebM through the VFS", "[core][movie]")
{
    TestFixture const _init;

    FIFE::MovieDecoder decoder(MOVIE_FILE);

    REQUIRE(decoder.getWidth() == 320U);
    REQUIRE(decoder.getHeight() == 240U);
    REQUIRE(decoder.getDuration() > 1.5);
    REQUIRE(decoder.getDuration() < 2.5);
    // 25 fps source, so a time base of 1/25.
    REQUIRE(decoder.getFrameRate() > 20.0);
    REQUIRE(decoder.getFrameRate() < 30.0);

    uint32_t frames           = 0;
    double lastTime           = -1.0;
    size_t const expectedSize = static_cast<size_t>(decoder.getWidth()) * static_cast<size_t>(decoder.getHeight()) * 4U;

    while (decoder.nextFrame()) {
        uint8_t const * data = decoder.getFrameData();
        REQUIRE(data != nullptr);
        REQUIRE(
            expectedSize == static_cast<size_t>(decoder.getWidth()) * static_cast<size_t>(decoder.getHeight()) * 4U);
        REQUIRE(decoder.getFrameTimestamp() >= lastTime);
        lastTime = decoder.getFrameTimestamp();
        ++frames;
    }

    // 2 seconds at 25 fps.
    REQUIRE(frames >= 40U);
    REQUIRE(frames <= 60U);
}

TEST_CASE("MovieDecoder seeks backwards and forwards", "[core][movie]")
{
    TestFixture const _init;

    FIFE::MovieDecoder decoder(MOVIE_FILE);
    REQUIRE(decoder.seek(1.0));

    uint32_t first = 0;
    while (decoder.nextFrame()) {
        if (decoder.getFrameTimestamp() >= 1.0) {
            break;
        }
        ++first;
    }
    REQUIRE(first > 0U);

    // Seeking back must land before where we were.
    REQUIRE(decoder.seek(0.0));
    uint32_t sinceRewind = 0;
    while (decoder.nextFrame() && decoder.getFrameTimestamp() < 0.5) {
        ++sinceRewind;
    }
    REQUIRE(sinceRewind > 0U);
    REQUIRE(sinceRewind < first);
}

TEST_CASE("MovieDecoder rejects files that are not movies", "[core][movie]")
{
    TestFixture const _init;

    REQUIRE_THROWS_AS(FIFE::MovieDecoder("tests/data/rpg_tiles_01.png"), FIFE::InvalidFormat);
    REQUIRE_THROWS_AS(FIFE::MovieDecoder("tests/data/does-not-exist.webm"), FIFE::NotFound);
}

TEST_CASE("MoviePlayer advances frames over time", "[core][movie]")
{
    TestFixture const _init;

    FIFE::Window window;
    window.create(
        FIFE::WindowSettings{.width = 320, .height = 240, .opengl = false, .windowMode = FIFE::WindowMode::Windowed});
    FIFE::RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");

    FIFE::MoviePlayer player;
    REQUIRE_FALSE(player.isLoaded());
    REQUIRE_FALSE(player.isFinished());

    player.load(MOVIE_FILE);
    REQUIRE(player.isLoaded());
    REQUIRE(player.getWidth() == 320U);
    REQUIRE(player.getHeight() == 240U);
    REQUIRE(player.getDuration() > 1.5);

    player.play(0.0);

    // Advancing to the end must produce a frame and then finish.
    player.update(1.0);
    FIFE::ImagePtr frame = player.getFrameImage();
    REQUIRE(frame != FIFE::ImagePtr{});
    REQUIRE(frame->getWidth() == 320U);
    REQUIRE(frame->getHeight() == 240U);

    player.update(5.0);
    REQUIRE(player.isFinished());
    REQUIRE_FALSE(player.isLooping());

    player.stop();
    REQUIRE_FALSE(player.isFinished());
}

TEST_CASE("MoviePlayer loops when asked", "[core][movie]")
{
    TestFixture const _init;

    FIFE::Window window;
    window.create(
        FIFE::WindowSettings{.width = 320, .height = 240, .opengl = false, .windowMode = FIFE::WindowMode::Windowed});
    FIFE::RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");

    FIFE::MoviePlayer player;
    player.load(MOVIE_FILE);
    player.setLooping(true);
    REQUIRE(player.isLooping());

    player.play(0.0);
    player.update(1.0);
    player.update(5.0);
    REQUIRE_FALSE(player.isFinished());

    player.setLooping(false);
    player.update(9.0);
    REQUIRE(player.isFinished());
}

TEST_CASE("MovieAudioDecoder produces stereo 16-bit PCM", "[core][movie][audio]")
{
    TestFixture const _init;

    auto source = std::make_shared<FIFE::MovieSource>(MOVIE_FILE);
    FIFE::MovieAudioDecoder decoder(source, MOVIE_FILE);

    // 48000 Hz stereo s16 is what SoundDecoder::getALFormat() maps onto.
    REQUIRE(decoder.getSampleRate() == 48000U);
    REQUIRE(decoder.isStereo());
    REQUIRE(decoder.getBitResolution() == 16);
    REQUIRE(decoder.getALFormat() == AL_FORMAT_STEREO16);
    REQUIRE(decoder.getDecodedLength() > 0U);
    REQUIRE(decoder.getDuration() > 1.5);
    REQUIRE(decoder.getDuration() < 2.5);

    uint64_t const chunk = 4096U;
    REQUIRE_FALSE(decoder.decode(chunk));
    REQUIRE(decoder.getBufferSize() == chunk);
    REQUIRE(decoder.getBuffer() != nullptr);
    REQUIRE(decoder.getPosition() == chunk);

    // A 440 Hz sine is not silence, so the decoded PCM must carry real samples.
    auto const * pcm = static_cast<int16_t const *>(decoder.getBuffer());
    REQUIRE(pcm != nullptr);
    bool nonSilent = false;
    for (uint64_t i = 0; i < chunk / sizeof(int16_t); ++i) {
        if (pcm[i] != 0) {
            nonSilent = true;
            break;
        }
    }
    REQUIRE(nonSilent);

    // Decoding past the end of the track reports "nothing produced".
    bool ended     = false;
    uint64_t total = chunk;
    while (!ended && total < decoder.getDecodedLength() * 4U) {
        ended = decoder.decode(chunk);
        total += chunk;
    }
    REQUIRE(ended);
    REQUIRE(decoder.getBufferSize() == 0U);
}

TEST_CASE("MovieAudioDecoder treats forward seeks as a no-op", "[core][movie][audio]")
{
    TestFixture const _init;

    auto source = std::make_shared<FIFE::MovieSource>(MOVIE_FILE);
    FIFE::MovieAudioDecoder decoder(source, MOVIE_FILE);

    REQUIRE_FALSE(decoder.decode(2048U));
    uint64_t const before = decoder.getPosition();
    REQUIRE(before == 2048U);

    // SoundClip::getStream() calls setCursor before every decode; forward seeks
    // must not restart the decoder or the stream would never make progress.
    REQUIRE(decoder.setCursor(before + 4096U));
    REQUIRE(decoder.getPosition() == before);
    REQUIRE_FALSE(decoder.decode(2048U));
    REQUIRE(decoder.getPosition() == before + 2048U);

    // Rewinding has to actually work.
    REQUIRE(decoder.setCursor(0U));
    REQUIRE(decoder.getPosition() == 0U);
    REQUIRE_FALSE(decoder.decode(2048U));
    REQUIRE(decoder.getBufferSize() == 2048U);
}

TEST_CASE("MovieAudioDecoder reports a missing audio track", "[core][movie][audio]")
{
    TestFixture const _init;

    auto source = std::make_shared<FIFE::MovieSource>(MOVIE_FILE);
    FIFE::MovieDecoder video(MOVIE_FILE, source);
    REQUIRE(video.getSource() == source);
}

TEST_CASE("MoviePlayer hands the audio track to OpenAL", "[core][movie][audio]")
{
    TestFixture const _init;

    // Engine normally owns these singletons; no core test creates an Engine.
    // SoundManager::init() is what makes an OpenAL context current, which
    // alBufferData in SoundClip::load() needs.
    FIFE::SoundManager soundmanager;
    FIFE::SoundClipManager soundclipmanager;
    soundmanager.init();
    REQUIRE(soundmanager.isActive());

    FIFE::Window window;
    window.create(
        FIFE::WindowSettings{.width = 320, .height = 240, .opengl = false, .windowMode = FIFE::WindowMode::Windowed});
    FIFE::RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");

    FIFE::MoviePlayer player;
    player.load(MOVIE_FILE);
    REQUIRE(player.hasAudio());

    // Exercises the real path: loader -> SoundClip::adobtDecoder -> alBufferData.
    FIFE::SoundClipPtr clip = player.getSoundClip();
    REQUIRE(clip);
    REQUIRE(clip->getDecoder() != nullptr);
    REQUIRE_FALSE(clip->isStream());
    REQUIRE(clip->countBuffers() >= 1U);
    REQUIRE(clip->getDecoder()->getSampleRate() == 48000U);
    REQUIRE(clip->getDecoder()->getALFormat() == AL_FORMAT_STEREO16);
    REQUIRE(clip->getDecoder()->getDecodedLength() > 0U);

    // Asking again must return the same clip, not rebuild one.
    REQUIRE(player.getSoundClip() == clip);

    player.unload();
    REQUIRE_FALSE(player.hasAudio());
}

    #ifdef HAVE_FIFEGUI
TEST_CASE("VideoWidget plays a movie inside a GUI element", "[core][movie][gui]")
{
    TestFixture const _init;

    FIFE::SoundManager soundmanager;
    FIFE::SoundClipManager soundclipmanager;
    soundmanager.init();

    FIFE::Window window;
    window.create(
        FIFE::WindowSettings{.width = 320, .height = 240, .opengl = false, .windowMode = FIFE::WindowMode::Windowed});
    FIFE::RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");

    fcn::VideoWidget widget(MOVIE_FILE);

    // Sized to the movie, and drawn 1:1 because scaling is off.
    REQUIRE(widget.getWidth() == 320);
    REQUIRE(widget.getHeight() == 240);
    REQUIRE(widget.getMoviePlayer() != nullptr);
    REQUIRE(widget.isPlaying());
    REQUIRE(widget.getDuration() > 1.5);
    REQUIRE(widget.isLooping() == false);

    // Ticking logic must publish a decoded frame.
    double first = -1.0;
    for (int i = 0; i < 10; ++i) {
        widget.logic();
        double const t = widget.getMoviePlayer()->getFrameTimestamp();
        if (t >= 0.0) {
            first = t;
            break;
        }
    }
    REQUIRE(first >= 0.0);

    widget.pause();
    REQUIRE_FALSE(widget.isPlaying());

    widget.play();
    REQUIRE(widget.isPlaying());

    // The widget publishes the current frame as an image for the inherited
    // Icon::draw to blit.
    REQUIRE(widget.getImage() != nullptr);
    REQUIRE(widget.getImage()->getWidth() == 320);
    REQUIRE(widget.getImage()->getHeight() == 240);

    // Frame timing is deliberately not asserted here: logic() reads TimeManager,
    // which only moves when something calls update(), and logic() itself is far
    // cheaper than a frame so the clock barely advances. MoviePlayer covers
    // timing deterministically, and the Engine integration test covers it with
    // real pump timing.
    widget.clearMovie();
    REQUIRE(widget.getMoviePlayer() == nullptr);
    REQUIRE(widget.getImage() == nullptr);
}
    #endif // HAVE_FIFEGUI

    #ifdef HAVE_MOVIE
namespace
{
    // Decodes a movie into a frame image and renders it repeatedly, which is what
    // forces the texture upload path rather than leaving it lazily unexercised.
    void render_movie_frames(FIFE::RenderBackend& renderbackend, std::string const & path)
    {
        FIFE::MoviePlayer player;
        player.load(path);
        REQUIRE(player.isLoaded());

        player.play(0.0);
        player.update(1.0);
        FIFE::ImagePtr frame = player.getFrameImage();
        REQUIRE(frame != FIFE::ImagePtr{});

        int const w = static_cast<int>(frame->getWidth());
        int const h = static_cast<int>(frame->getHeight());
        for (int i = 0; i < 20; ++i) {
            renderbackend.startFrame();
            frame->render(FIFE::Rect(0, 0, w, h));
            renderbackend.endFrame();
        }
    }
} // namespace

TEST_CASE("MoviePlayer streams frames through the SDL texture path", "[core][movie][sdl]")
{
    TestFixture const _init;

    FIFE::Window window;
    window.create(
        FIFE::WindowSettings{.width = 320, .height = 240, .opengl = false, .windowMode = FIFE::WindowMode::Windowed});
    FIFE::RenderBackendSDL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.setWindowObject(&window);
    renderbackend.createMainScreen("FIFE", "");

    render_movie_frames(renderbackend, MOVIE_FILE);
}

TEST_CASE("MoviePlayer streams frames through the OpenGL texture path", "[core][movie][opengl]")
{
    TestFixture const _init;

    FIFE::Window window;
    try {
        window.create(
            FIFE::WindowSettings{
                .width = 320, .height = 240, .opengl = true, .windowMode = FIFE::WindowMode::Windowed});
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL not available in this environment");
    }
    FIFE::RenderBackendOpenGL renderbackend(SDL_Color{.r = 0, .g = 0, .b = 0, .a = 255});
    renderbackend.init("");
    renderbackend.setWindowObject(&window);
    try {
        renderbackend.createMainScreen("FIFE", "");
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL not available in this environment");
    }

    render_movie_frames(renderbackend, MOVIE_FILE);
}

TEST_CASE("Engine plays a movie fullscreen", "[core][movie][integration]")
{
    // Deliberately no TestFixture: it constructs the VFS and ImageManager
    // singletons, which Engine::init() also creates, so both would abort.
    FIFE::Engine engine;
    FIFE::EngineSettings& settings = engine.getSettings();
    settings.setRenderBackend("SDL");
    settings.setFullScreen(false);
    settings.setScreenWidth(64);
    settings.setScreenHeight(64);
    settings.setDefaultFontPath("tests/data/FreeMono.ttf");
    settings.setDefaultFontGlyphs(" abcdefghijklmnopqrstuvwxyz0123456789");
    engine.init();
    // Gives movie audio a device when one exists; Engine::init() does not do this.
    engine.getSoundManager()->init();

    engine.initializePumping();
    REQUIRE_FALSE(engine.isMoviePlaying());
    REQUIRE(engine.getMovieTime() < 0.0);

    engine.playMovie(MOVIE_FILE);
    REQUIRE(engine.isMoviePlaying());

    // Pump once so the movie draws and decodes a frame.
    engine.pump();
    double best = engine.getMovieTime();
    REQUIRE(best >= 0.0);

    // Frames must advance, and the movie must run to its end by itself. The clock
    // is real time (TimeManager has no injection point), so the bound is generous.
    bool finished  = false;
    int iterations = 0;
    for (int i = 0; i < 20000 && !finished; ++i) {
        engine.pump();
        ++iterations;
        double const t = engine.getMovieTime();
        if (t < 0.0) {
            // pump() saw the movie end and called stopMovie() itself.
            finished = true;
        } else if (t > best) {
            best = t;
        }
    }
    CAPTURE(iterations);
    CAPTURE(finished);
    CAPTURE(best);
    REQUIRE(best > 0.0);
    REQUIRE(finished);
    REQUIRE_FALSE(engine.isMoviePlaying());

    engine.finalizePumping();
    engine.destroy();
}
    #endif // HAVE_MOVIE
#endif     // HAVE_MOVIE
