![FIFE LOGO](https://raw.githubusercontent.com/fifengine/fifengine/master/doc/logo/FIFE_small_c3.png)

# FIFE

Flexible Isometric Free Engine (FIFE) is a multi-platform isometric game engine written in C++.
It comes with Python bindings allowing users to create games using Python as well as C++.
The engine is extendable and enables you to add any feature you can imagine to your project.

| [Website](http://fifengine.net/) | [![#fife on Freenode](https://img.shields.io/badge/freenode-%23fife-green.svg)](https://webchat.freenode.net/?channels=fife) | [Changelog](https://github.com/fifengine/fifengine/blob/master/CHANGELOG.md) | [Releases](https://github.com/fifengine/fifengine/releases) | [Source Code (zip)](https://github.com/fifengine/fifengine/archive/master.zip) |
|:--------:|:--------:|:--------:|:--------:|:--------:|

| Continuous Integration | Linux |   Mac    | Windows |
|:----------------------:|:-----:|:--------:|:-------:|
| **Build Status** | [![Travis Build status](https://travis-ci.org/fifengine/fifengine.svg?branch=master)](https://travis-ci.org/fifengine/fifengine) | [![Travis Build status](https://travis-ci.org/fifengine/fifengine.svg?branch=master)](https://travis-ci.org/fifengine/fifengine) | [![Appveyor Build status](https://ci.appveyor.com/api/projects/status/github/fifengine/fifengine?branch=master&svg=true)](https://ci.appveyor.com/project/LinuxDonald/fifengine) |
| **Development Releases**  |   -    |    -     | [All artifacts](https://ci.appveyor.com/project/LinuxDonald/fifengine/build/artifacts) |

| **Documentations** | | | |
|:----------------------:|:----------------:|:-------:|:----------:|
| [Overview](https://fifengine.github.io/fifengine-docs/) | [Developer Manual](https://fifengine.github.io/fifengine-docs/developer-manual/en/) | [C++ API](http://www.ragingreality.com/doxygen/) | [Python API](http://www.ragingreality.com/epydoc) |

Fifengine is also already available from the following package repositories:

[![Packaging status](https://repology.org/badge/vertical-allrepos/fife.svg)](https://repology.org/metapackage/fife)

# Table of contents
1. [Features](#1-features)
2. [License](#2-license)
3. [How to Build](#3-how-to-build)
4. [Tests](#4-tests)
5. [Python Demos](#5-python-demos)
6. [Python Tutorials](#6-python-tutorials)
7. [C++ Tutorials](#7-c-tutorials)
8. [Troubleshooting](#8-troubleshooting)
9. [Feedback](#9-feedback)

## 1) Features

A [complete list of the features](https://fifengine.github.io/fifengine-docs/user-manual/en/#_features) that FIFE offers can be found in the user-manual.


## 2) License

- The source code (*.cpp, *.h & *.py) is licensed under [LGPL v2.1 or newer](http://www.gnu.org/licenses/lgpl-2.1.html).
- Files in /src/engine/util/utf8/ are under BSL license
- Content was taken from a lot a lot of different 3rd party sources. Therefore
each client directory comes with a separate LICENSE file that states the origin
of the content, the author and the actual license it was published under.


## 3) How to Build

### Linux

FIFE uses CMake for building. The build process requires several dependencies that can be installed via vcpkg.

#### Prerequisites
- CMake 3.15 or newer
- C++17 compatible compiler (GCC 7+, Clang 5+, or MSVC 2017+)
- vcpkg package manager

#### Install Dependencies
1. Install vcpkg if not already installed:
   ```bash
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.sh
   ./vcpkg integrate install
   ```

2. Install required system packages:
   ```bash
   sudo apt-get update
   sudo apt-get install libglu1-mesa-dev
   ```

3. Install FIFE dependencies via vcpkg:
   ```bash
   ./vcpkg install --triplet x64-linux
   ```
   Or run the provided script:
   ```bash
   ./vcpkg.sh
   ```

#### Build Steps
1. Configure the build using CMake presets:
   ```bash
   cmake --preset clang20-x64-linux-rel -Dfifechan=OFF -Dbuild-python=OFF -Dbuild-library=ON
   ```
   This creates the build directory `out/build/clang20-x64-linux-rel` and configures for:
   - Release build with Clang 20
   - Disabled fifechan GUI library
   - Disabled Python bindings
   - Enabled shared library build

2. Build the library:
   ```bash
   cmake --build out/build/clang20-x64-linux-rel
   ```

3. (Optional) Install the library:
   ```bash
   cmake --install out/build/clang20-x64-linux-rel
   ```

The built library `libfife.so` will be located in `out/build/clang20-x64-linux-rel/`.

#### Alternative Build Options
- To build with fifechan GUI support: `-Dfifechan=ON`
- To build Python bindings: `-Dbuild-python=ON`
- For debug build, use preset `clang20-x64-linux-dbg`
- For different compilers, use appropriate presets (see `CMakePresets.json`)

#### Build Targets

To list all available build targets:

```bash
cmake --build <build-dir> --target help
```

Existing Targets:

Target                | Description                     | CMake Options Required
---                   | ---                             | ---
1. fife               | C++ library                     | -Dbuild-library=ON
2. fife.headers       | Custom target for headers       |
3. fife.swig-wrappers | Custom target for SWIG wrappers |
4. fife_swig          | Python extension for FIFE       | -Dbuild-python=ON (default)
5. fifechan_swig      | Python extension for fifechan   | -Dbuild-python=ON -Dfifechan=ON (default)
6. C++ tests:         | Various test_*                  | -Dbuild-library=ON -Dbuild-tests=ON

#### Build commands (example):

##### Build SWIG Python modules

```
cmake -B build -S . -Dbuild-python=ON
cmake --build build --target fife_swig fifechan_swig
```
##### Build C++ library + tests

```
cmake -B build -S . -Dbuild-library=ON -Dbuild-tests=ON
cmake --build build
```

## 4) Tests

The folllowing tests are available:

- fife_test (`tests/fife_test/`): Custom runner run.py
- swig_tests (`tests/swig_tests/`): Uses unittest
- extension_tests (`tests/extension_tests/`): Uses unittest
- core_tests (`tests/core_tests/`): Uses Catch2 (C++)

The test tool can be found within the `<FIFE>/tests/fife_test` directory.
You can launch it by running `run.py`. Open the console with `F10`.
To run a test enter `run` and the test name like `PathfinderTest`.

Prerequisites:
1. Build fife_swig (and optionally fifechan_swig)
2. Install or set PYTHONPATH to include the built modules

### Running C++ Core Tests

C++ tests use Catch2 and require the fife library target to exist.

Prerequisites:
1. Configure with `-Dbuild-library=ON -Dbuild-tests=ON`
2. Build the targets

Running tests:

# Via CTest (after building)

```
cd build
ctest --test-dir . -L core -V
```

# Or run directly

./test_dat1
./test_dat2


## 5) Python Demos

### [Editor](https://github.com/fifengine/fifengine-editor)
The Python based editor tool can be found within the [fifengine-editor repo](https://github.com/fifengine/fifengine-editor).
You can launch it by running `run.py`. It is used to edit map files for the [Rio De Hola demo](https://github.com/fifengine/fifengine-demos/tree/master/rio_de_hola). Other clients extend it and use it to edit their maps.

### [Rio De Hola](https://github.com/fifengine/fifengine-demos/tree/master/rio_de_hola)
Rio de hola is a technology demo showing off many of the FIFE features.  It is
located in the `rio_de_hola` directory and can be launched by running `run.py`.
Once started as an example game, it's now a playground for developers to test their code.
It serves as a good starting point for own game developments.

### [Shooter](https://github.com/fifengine/fifengine-demos/tree/master/shooter)
The Shooter demo was an attempt to show the versatility and flexibility of FIFE.
It is a simple side scrolling shooter that has a main menu, one level and an end boss.
Try your luck and see if you can defeat the boss!

### [PyChan](https://github.com/fifengine/fifengine-demos/tree/master/pychan_demo)
There is an example client residing in `pychan_demo` that shows how the pychan GUI library works.
You may start the GUI demo application by running `pychan_demo.py`.

### Configuring the Editor and Demos
The engine utilizes special settings files for configuring FIFE. This file is
called `settings.xml` and resides in the `~/.fife` directory (in
`<User>\Application Data\fife` for Windows users).  The Shooter Demo and the
PyChan demo are exceptions.  They both store their `settings.xml` file in their
root directories.

NOTE that the `settings.xml` file is auto generated and wont be there until you
run the demos for the first time.  FIFE automatically fills the settings file
with default values.  You find more information on [FIFE settings in the
Developer-Manual](https://fifengine.github.io/fifengine-docs/developer-manual/en/#_engine_settings).


## 6) [Python Tutorials](https://github.com/fifengine/python-tutorials)

Our Python tutorials demonstrate how to use the engine when working with Python.


## 7) [C++ Tutorials](https://github.com/fifengine/cpp-tutorials)

There is also a set of C++ tutorials demonstrating the usage of the engine, when working with C++.


## 8) Troubleshooting

### Video Card/Driver Issues
There are some known driver/card combinations that exhibit some problems with
FIFE.  Users have reported seeing blank screens or fuzzy images.  If this is
happening to you please report it on our [issue tracker](https://github.com/fifengine/fifengine/issues).

Please include your Card/Driver/OS information in your post.  Currently the fix
is to modify your `settings.xml` file and set both GLUseFramebuffer and GLUseNPOT
to `False` (they are `True` by default).

### Audio
Win32 users tend to suffer from problems with the OpenAL drivers. If you don't
hear sound while running a FIFE client, run `oalinst.exe` that ships with the
Win32 FIFE Development Kit.  This is the latest OpenAL driver for Win32.


## 9) Feedback

We appreciate every kind of feedback concerning the release, the project in
general and the bundled techdemo. Feedback is a great way to help us to improve
FIFE. If you would like to get in contact with us and provide feedback you can
either visit our IRC channel: #fife on freenode.net.
