![FIFE LOGO](https://raw.githubusercontent.com/fifengine/fifengine/master/doc/logo/FIFE_small_c3.png)

# FIFE

Flexible Isometric Free Engine (FIFE) is a multi-platform isometric game engine written in C++.
It comes with Python bindings allowing users to create games using Python as well as C++. 
The engine is extendable and enables you to add any feature you can imagine to your project.

| [Website](http://fifengine.net/) | [![#fife on Freenode](https://img.shields.io/badge/freenode-%23fife-green.svg)](https://webchat.freenode.net/?channels=fife) | [Changelog](https://github.com/fifengine/fifengine/blob/master/CHANGELOG.md) | [Releases](https://github.com/fifengine/fifengine/releases) | [Source Code (zip)](https://github.com/fifengine/fifengine/archive/master.zip) |
|:--------:|:--------:|:--------:|:--------:|:--------:|:--------:|:--------:|

| Continuous Integration | Linux |   Mac    | Windows |
|:----------------------:|:-----:|:--------:|:-------:|
| **Build Status** | [![Travis Build status](https://travis-ci.org/fifengine/fifengine.svg?branch=master)](https://travis-ci.org/fifengine/fifengine) | [![Travis Build status](https://travis-ci.org/fifengine/fifengine.svg?branch=master)](https://travis-ci.org/fifengine/fifengine) | [![Appveyor Build status](https://ci.appveyor.com/api/projects/status/github/fifengine/fifengine?branch=master&svg=true)](https://ci.appveyor.com/project/LinuxDonald/fifengine) | 
| **Development Releases**  |   -    |    -     | [All artifacts](https://ci.appveyor.com/project/LinuxDonald/fifengine/build/artifacts) |

| **Documentations** | | | |
|:----------------------:|:----------------:|:-------:|:----------:|
| [Overview](https://fifengine.github.io/fifengine-docs/) | [Developer Manual](https://fifengine.github.io/fifengine-docs/developer-manual/en/) | [C++ API](http://www.fifengine.net/doxygen) | [Python API](http://www.fifengine.net/epydoc) |

# Table of contents
1. [Features]         (#1-features)
2. [License]          (#2-license)
3. [Tests]            (#3-tests)
4. [Python Demos]     (#4-python-demos)
6. [Python Tutorials] (#5-python-tutorials)
5. [C++ Tutorials]    (#6-c-tutorials)
7. [Troubleshooting]  (#7-troubleshooting)
8. [Feedback]         (#8-feedback)

## 1) Features

A [complete list of the features](https://fifengine.github.io/fifengine-docs/user-manual/en/#_features) that FIFE offers can be found in the user-manual.


## 2) License

The source code (*.cpp, *.h & *.py) is licensed under [LGPL v2.1 or newer](http://www.gnu.org/licenses/lgpl-2.1.html):
Files in /engine/core/ext/glee/ are under BSD license
Files in /engine/core/util/utf8/ are under BSL license
Content was taken from a lot a lot of different 3rd party sources. Therefore 
each client directory comes with a separate LICENSE file that states the origin
of the content, the author and the actual license it was published under.


## 3) Tests

The test tool can be found within the `<FIFE>/tests/fife_test` directory.
You can launch it by running `run.py`. Open the console with `F10`.
To run a test enter `run` and the test name like `PathfinderTest`.

## 4) Python Demos

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


## 5) [Python Tutorials](https://github.com/fifengine/python-tutorials)

Our Python tutorials demonstrate how to use the engine when working with Python.


## 6) [C++ Tutorials](https://github.com/fifengine/cpp-tutorials)

There is also a set of C++ tutorials demonstrating the usage of the engine, when working with C++.


## 7) Troubleshooting

### Video Card/Driver Issues
There are some known driver/card combinations that exhibit some problems with
FIFE.  Users have reported seeing blank screens or fuzzy images.  If this is
happening to you please report it on our forums in the Help and troubleshooting
section here: http://forums.fifengine.net/index.php?board=4.0

Please include your Card/Driver/OS information in your post.  Currently the fix
is to modify your `settings.xml` file and set both GLUseFramebuffer and GLUseNPOT
to `False` (they are `True` by default).

### Audio
Win32 users tend to suffer from problems with the OpenAL drivers. If you don't 
hear sound while running a FIFE client, run `oalinst.exe` that ships with the
Win32 FIFE Development Kit.  This is the latest OpenAL driver for Win32.


## 8) Feedback

We appreciate every kind of feedback concerning the release, the project in 
general and the bundled techdemo. Feedback is a great way to help us to improve 
FIFE. If you would like to get in contact with us and provide feedback you can 
either visit our IRC channel or our forums:
 * #fife on freenode.net
 * http://forums.fifengine.net/
