# Movie support

FFmpeg/libav-based playback of movie files, either fullscreen through `Engine` or
inside a FifeGUI widget. Backed by
[#166](https://github.com/fifengine/fifengine/issues/166).

The feature is **off by default**. A build must opt in with `ENABLE_MOVIE=ON`, so
the existing build matrix, CI and wheel builds are unaffected.

---

## 1. Building

FFmpeg must be linked as a shared object. See §4 for why, and §3 for the triplet
you need.

```
cmake --preset clang22-x64-linux-dbg-cov \
      -B out/build/movie-dynamic \
      -D VCPKG_TARGET_TRIPLET=x64-linux-dynamic \
      -D VCPKG_INSTALLED_DIR=out/vcpkg_installed-dynamic \
      -D ENABLE_MOVIE=ON
cmake --build out/build/movie-dynamic --parallel
```

`VCPKG_INSTALLED_DIR` must differ from a static build's. A manifest install for one
triplet **removes the other triplets' directories**, so a default build and a movie
build sharing `vcpkg_installed/` will each delete what the other installed.

| Platform | Triplet |
| --- | --- |
| Windows | `x64-windows` (already dynamic, nothing to do) |
| Linux | `x64-linux-dynamic` |
| macOS | `x64-osx-dynamic` |

`ENABLE_MOVIE=ON` adds the `movie` feature to `VCPKG_MANIFEST_FEATURES`, which pulls
in `ffmpeg` with the port features `avcodec`, `avformat`, `swscale` and
`swresample`. Nothing is downloaded unless the option is set.

The build fails with a `FATAL_ERROR` if the triplet turns out not to be dynamic.
There is no preset change for this: triplets are a per-build property, and vcpkg
offers no dynamic variant of the triplet a preset already selects. Override on the
command line as shown above.

The check reads `VCPKG_LIBRARY_LINKAGE` out of the triplet `.cmake` file, because
that variable is internal to vcpkg and not exposed to projects. If the triplet file
cannot be found the guard fails closed, because "could not determine" must not read
as "fine".

To link FFmpeg statically anyway, acknowledge that you will ship its object files
and relinking instructions with `-D FIFE_MOVIE_ALLOW_STATIC_LINKAGE=ON`.

Important: ffmpeg has a build tool requirement for `nasm` on x86/x64.
If you see `nasm: command not found` in the vcpkg build log, install it and rebuild.

---

## 2. Supported formats

Only patent-free codecs are supported. This is deliberate: the codecs below can be
redistributed inside a proprietary game, whereas H.264, HEVC and AAC expose the
distributor to patent claims.

| Kind | Supported |
| --- | --- |
| Video | AV1, VP9, VP8, Theora |
| Audio | Opus, Vorbis, FLAC, MP3, PCM |
| Container | WebM, Matroska, Ogg |

**Not supported:** H.264/AVC, HEVC/H.265, AAC. A file using any of these opens and
then fails on the first video frame with `InvalidFormat`.

---

## 3. Usage

### Fullscreen

```cpp
#ifdef HAVE_MOVIE
engine.playMovie("cutscenes/intro.webm");
#endif
```

`playMovie()` takes over the screen until the movie ends or `stopMovie()` is called.
The world is not rendered while a movie plays. The movie is drawn *after* the world
but *before* the GUI, so a GUI stays on top of it.

| Method | Purpose |
| --- | --- |
| `playMovie(path, looping=false)` | Start playback, `CannotOpenFile` / `InvalidFormat` on failure |
| `stopMovie()` | Stop and return to normal rendering |
| `isMoviePlaying()` | Whether a movie is currently playing |
| `getMovieTime()` | Playback position in seconds |

`getMovieTime()` needs `Engine::pump()` to run; the movie origin is captured on the
first pump, so the first call returns 0.

### Widget

`fcn::VideoWidget` derives from `fcn::Icon` and publishes the current decoded frame
as an image, which the inherited `Icon::draw` blits. Use it like any other FifeGUI
widget, calling `logic()` each frame.

```cpp
auto widget = std::make_shared<fcn::VideoWidget>("intro.webm");
widget->setLooping(true);
widget->play();
// per frame:
widget->logic();
```

| Method | Purpose |
| --- | --- |
| `setMovie(path)` / `clearMovie()` | Attach or detach a movie |
| `setMoviePlayer(shared_ptr<MoviePlayer>)` | Drive the widget from a player you own |
| `play()` / `pause()` / `stop()` | Transport control |
| `isPlaying()` / `isFinished()` | State |
| `hasAudio()` | Whether the file carries an audio track |
| `getDuration()` | Length in seconds |
| `setLooping(bool)` / `isLooping()` | Loop control |

`getMoviePlayer()` returns the attached player for C++ callers. It is **not**
exposed to Python — the return type is a non-owning `MoviePlayer*` into an object the
widget owns, so a Python handle could outlive it. `hasAudio()` exists so Python can
ask that question without reaching through the widget.

---

## 4. Licensing

FFmpeg is LGPL-2.1-or-later. FIFE itself is LGPL-2.1, so the combination is
distributable, but two obligations come with it:

1. **FFmpeg must be a shared object.** LGPL §6 lets users replace the library. That
   is only possible if it is not baked into `libfifengine`, which is what the
   dynamic-triplet guard in §1 enforces.
2. **Users must be able to find it and replace it.** The attribution in the README
   names FFmpeg, its licence and where to obtain it.

Building FFmpeg with `--enable-gpl` or `--enable-nonfree` is not supported. The vcpkg
`ffmpeg` port does neither, and the `movie` feature deliberately does not request
`x264`, `x265` or any other external codec library.

The consequence worth stating plainly: **linkage is per-build, not per-port.** A
`libfifengine.so` built against a dynamic triplet cannot later be statically linked
against FFmpeg. The guard exists so that mistake is caught at configure time instead
of at release time.

---

## 5. Architecture

| File | Role |
| --- | --- |
| `src/fife/video/movie/moviesource.{h,cpp}` | Bridges a VFS source into libav's AVIOContext, so movies can live in ZIP/DAT archives. Also forwards libav logging to FIFE's logger |
| `src/fife/video/movie/moviedecoder.{h,cpp}` | Demuxing and video decoding; `swscale` converts to RGBA. Sequential-only, no B-frame reordering beyond libav's own |
| `src/fife/video/movie/movieaudiodecoder.{h,cpp}` | A `SoundDecoder`, so audio reuses the existing OpenAL streaming machinery |
| `src/fife/video/movie/movieplayer.{h,cpp}` | Owns decoder and audio decoder, exposes `update(currentTime)`, handles A/V sync and frame upload |
| `src/fife/gui/fifechan/widgets/videowidget.{h,cpp}` | FifeGUI widget wrapping a `MoviePlayer` |

### A/V synchronisation

Video is slaved to the **audio clock**: playback position comes from the OpenAL
emitter's playback offset, not from the wall clock. When audio is absent, the
`TimeManager` clock is used instead.

Decoding runs one frame ahead. A frame whose presentation time is still in the
future is stashed rather than dropped, and the player returns until the clock reaches
it. Without that, a slow pump would skip frames outright and a fast one would stall.

`play(currentTime)` ignores its argument. The start time comes from the first
`update()` call instead, because callers commonly read their clock *after* calling
`play()` — honouring the passed-in value would make the movie look like it started in
the far past.

### Texture upload

`MoviePlayer` writes into a persistent `Image` and calls `updateTexture()`:

- `GLImage::updateTexture()` re-uploads via `glTexSubImage2D`, reusing the existing
  texture object.
- `SDLImage::updateTexture()` calls `SDL_UpdateTexture` on a
  `SDL_TEXTUREACCESS_STREAMING` texture.
- `Image::updateTexture()` is a virtual no-op fallback, so subclasses that do not
  stream do not have to override it.

### Timing

`MoviePlayer::update()` takes the current time as a parameter rather than reading a
clock itself. That makes it testable with synthetic times, and it is why
`MoviePlayer` and `VideoWidget` tests assert different things: the player is driven
with explicit timestamps, whereas the widget's `logic()` reads `TimeManager`, which
only advances when something calls `update()` — normally `Engine::pump()`. A bare
`logic()` loop therefore does not advance the clock far enough to cross a frame
boundary at `SDL_GetTicks()` millisecond resolution.

---

## 6. Tests

`tests/core_tests/test_movie.cpp` — test cases are gated on `HAVE_MOVIE`. The test asset is
`tests/data/movie-test.webm` (VP9 + Opus, 320×240, 25 fps, ~2.01 s).

Coverage: source loading through VFS, decode to RGBA, dimensions, frame rate and
duration, seeking, frame timestamps, audio-track detection, transport control,
looping, the `VideoWidget`, texture streaming through both the SDL and the OpenGL
backend, and one end-to-end `Engine::playMovie()` integration test that pumps the
engine and asserts that movie time actually progresses.

The integration test asserts on real elapsed time and is therefore sensitive to how
much work a pump does; its margins (a 40 ms frame boundary against a ~9 ms pump) are
wide but not infinite.

```bash
LD_LIBRARY_PATH=out/build/movie-dynamic:out/fife-dependencies/x64-linux-dynamic/install/lib \
SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy ALSOFT_DRIVERS=null \
ctest --test-dir out/build/movie-dynamic
```

The `ALSOFT_DRIVERS=null` is what lets OpenAL initialise headlessly. `run_tests.py`
sets it, so `./run_tests.py -a` works without extra environment setup.

---

## 7. Known limitations

- **The OpenGL test skips without a display.** `MoviePlayer streams frames through the
  OpenGL texture path` needs a GL context and calls `SKIP()` when one cannot be created,
  which is the case on a bare headless machine. CI runs it under Xvfb with llvmpipe.
  This test is the only coverage of `GLImage::updateTexture()`, so a green local run
  without a display does not mean the GL path was exercised.
- **No software-rasteriser fallback.** Movie playback needs a real GL context or SDL
  renderer; there is no way to decode a frame without a backend.
- **`drawImage` sub-rectangles are ignored** by both GUI backends. Movie playback is
  unaffected, because `fcn::Icon::draw` always passes `0, 0`, which is what
  `VideoWidget` uses. See `todo-fifechan-upstream.md` §2 and `todo-movie-support.md`
  §10a for the analysis.
- **Not exposed to Python:** `MovieSource` and `MovieAudioDecoder` are only
  constructible from a `std::shared_ptr`, and wrapping that type would mean adding
  `std_shared_ptr.i` to the main interface file, which no other interface uses. Python
  goes through `MoviePlayer` or `Engine.playMovie()` instead. `VideoWidget`'s
  `getMoviePlayer()` is likewise excluded; §3 explains why.
- **`getMovieTime()` returns a negative value when no movie is loaded**, not `0`. Use
  `isMoviePlaying()` to tell "no movie" apart from "a movie at its first frame".
- **No progress reporting.** `playMovie()` is blocking until the movie ends or
  `stopMovie()` is called from another thread; there is no completion callback.

---

## 8. Python bindings

`tests/swig_tests/test_movie.py` covers the exposed surface.
Its skipped in a default build with `ENABLE_MOVIE=OFF`.

Two important things:

- **`MovieSource` and `MovieAudioDecoder` are guarded against appearing.** The
  `test_movie_source_is_not_exposed` test fails if they are ever wrapped by accident,
  since that would mean pulling SWIG stdlib support into the interface.
- **Use the `shared_engine` fixture, not `engine`.** Only one Engine can be alive at
  a time; the per-test `engine` fixture builds a fresh one, and a second construction
  aborts the interpreter. `shared_engine` is module-scoped for that reason. Several
  other SWIG modules work around the same limitation differently, by having exactly
  one engine-using test each.
