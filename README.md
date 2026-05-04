![FIFE LOGO](https://raw.githubusercontent.com/fifengine/fifengine/main/docs/logo/FIFE_small_c3.png)

# FIFE

Flexible Isometric Free Engine (FIFE) is a multi-platform isometric game engine written in C++.
It comes with Python bindings allowing users to create games using Python as well as C++.
The engine is extendable and enables you to add any feature you can imagine to your project.

[Website](http://fifengine.net/) | [Changelog](https://github.com/fifengine/fifengine/blob/main/CHANGELOG.md) | [Releases](https://github.com/fifengine/fifengine/releases) | [Docs](https://fifengine.github.io/fifengine-docs/) | [Developer Manual](https://fifengine.github.io/fifengine-docs/developer-manual/en/) | [C++ API](https://fifengine.github.io/fifengine/api/) | [Python API](https://fifengine.github.io/fifengine/api/python)


| Continuous Integration | Windows | Linux  |   Mac   | Python Wheels |
|:----------------------:|:-----:|:--------:|:-------:|:-------------:|
| **Build Status** | [![Build on Windows (C++ Library)](https://github.com/fifengine/fifengine/actions/workflows/build-on-windows.yml/badge.svg?branch=main)](https://github.com/fifengine/fifengine/actions/workflows/build-on-windows.yml) | [![Build on Linux (C++ Library)](https://github.com/fifengine/fifengine/actions/workflows/build-on-linux.yml/badge.svg?branch=main)](https://github.com/fifengine/fifengine/actions/workflows/build-on-linux.yml) | 👷🏼 | [![Build Fifengine (Python Wheels)](https://github.com/fifengine/fifengine/actions/workflows/build-wheels.yml/badge.svg?branch=main)](https://github.com/fifengine/fifengine/actions/workflows/build-wheels.yml)



# Table of contents
1. [Features](#1-features)
2. [Releases](#2-releases)
3. [License](#3-license)
4. [How to Build](#4-how-to-build)
5. [Tests](#5-tests)
6. [Python Demos](#6-python-demos)
7. [Python Tutorials](#7-python-tutorials)
8. [C++ Tutorials](#8-c-tutorials)
9. [Troubleshooting](#9-troubleshooting)
10. [Feedback](#10-feedback)

## 1) Features

A [detailed feature list for FIFE](https://fifengine.github.io/fifengine-docs/user-manual/en/#_features) is documented in the user manual.

## 2) Releases

#### Latest Release

You find the latest releases on [Github Releases](https://github.com/fifengine/fifengine/releases).

#### Development Releases

We also provide releases for the latest successful build on Github Actions CI.

#### Availability in Package Repositories

Fifengine is also already available from the following package repositories:

[![Packaging status](https://repology.org/badge/vertical-allrepos/fife.svg?columns=3)](https://repology.org/metapackage/fife)

## 3) License

The source code (*.cpp, *.h & *.py) is licensed under [LGPL v2.1 or newer](http://www.gnu.org/licenses/lgpl-2.1.html).

Content was taken from a lot a lot of different 3rd party sources.
Therefore each client directory comes with a separate LICENSE file that states the origin
of the content, the author and the actual license it was published under.

## 4) How to Build

### Linux

FIFE uses CMake and Ninja for building.
The build process requires several dependencies that can be installed as system packages or via vcpkg.

#### Prerequisites

- CMake 3.25 or newer
- C++20 compatible compiler (GCC, Clang, MSVC 17 or 18)
- vcpkg package manager (optional)

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
   cmake --preset clang20-x64-linux-rel -DENABLE_FIFEGUI=OFF -DBUILD_PYTHON_BINDINGS=OFF -DBUILD_LIBRARY=ON
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

- To build with fifechan GUI support: `-DENABLE_FIFEGUI=ON`
- To build Python bindings: `-DBUILD_PYTHON_BINDINGS=ON`
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
1. fife               | C++ library                     | -DBUILD_LIBRARY=ON
2. fife.headers       | Custom target for headers       |
3. fife.swig-wrappers | Custom target for SWIG wrappers |
4. fife_swig          | Python extension for FIFE       | -DBUILD_PYTHON_BINDINGS=ON
5. fifechan_swig      | Python extension for fifechan   | -DBUILD_PYTHON_BINDINGS=ON -DENABLE_FIFEGUI=ON
6. test_*             | Various C++ tests               | -DBUILD_LIBRARY=ON -DBUILD_TESTS=ON

#### How to build using vcpkg and VSCode

1. The vcpkg setup and environment bootstrap script can be found in:

> ./build-tools/setup-vcpkg.sh

The devcontainer postStartCommand will automatically run the script and set up the environment for you.

2. Then install the dependencies for fifengine:

> ./dependencies/build.sh

3. Then use VSCode to build the project with the appropriate preset.
   The initial build can take a while as all dependencies are built from source.

#### Build commands (example):

##### Build SWIG Python modules

```
cmake -B build -S . -DBUILD_PYTHON_BINDINGS=ON
cmake --build build --target fife_swig fifechan_swig
```
##### Build C++ library + tests

```
cmake -B build -S . -DBUILD_LIBRARY=ON -DBUILD_TESTS=ON
cmake --build build
```

## 5) Tests

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
1. Configure with `-DBUILD_LIBRARY=ON -DBUILD_TESTS=ON`
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

## 6) Python Demos

The Python demos are located in the `demos` directory.
Each demo has its own folder and can be launched by running `run.py` in the respective folder.

For details about each demo, please refer to the demo's [README](demos/README.md) file.

## 7) [Python Tutorials](https://github.com/fifengine/python-tutorials)

Our Python tutorials demonstrate how to use the engine when working with Python.

## 8) [C++ Tutorials](https://github.com/fifengine/cpp-tutorials)

There is also a set of C++ tutorials that demonstrate how to use the engine when working with C++.

## 9) Troubleshooting

### Video Card/Driver Issues

Certain driver and graphics card combinations are known to cause issues with FIFE.
Users may experience blank screens or blurred images.
If you encounter this, please report it on our [issue tracker](https://github.com/fifengine/fifengine/issues).

Please include your graphics card, driver, and operating system information in your report.
Currently, the workaround is to edit your `settings.xml` file and set
both `GLUseFramebuffer` and `GLUseNPOT` to `False` (they are `True` by default).

### Audio

Win32 users tend to suffer from problems with the OpenAL drivers. If you don't
hear sound while running a FIFE client, run `oalinst.exe` that ships with the
Win32 FIFE Development Kit.  This is the latest OpenAL driver for Win32.

## 10) Feedback

We appreciate every kind of feedback concerning the release, the project in
general and the bundled techdemo. Feedback is a great way to help us to improve
FIFE. If you would like to get in contact with us or provide feedback,
you can post in the [discussions section](https://github.com/fifengine/fifengine/discussions).
