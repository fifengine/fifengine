- Lua-library around GNU gettext -

Install:

* requires: gettext + lua 5.0 (not tested with 5.1)
* run: scons

Demo:
lua test.lua

Currently this currently will *not* work from inside FIFE on all platforms!

It may work on Windows (where the default lua library supports 'loadlib')
but will fail (on Linux) when the lua library has been compiled with
default settings.

The solution is to ship our own lua library (with fife binary releases).

See also: http://www.gnu.org/software/gettext/manual/html_chapter/gettext_toc.html
