# The SWIG Python Bindings of Fifengine

This directory contains SWIG interface templates for generating Python bindings.

## File Structure

### Main Templates

- `fife.i.templ.cm`
  -  FIFE engine SWIG interface for CMake. Generates `fife.i`.
- `fifechan.i.templ.cm`
  -  FIFEChan GUI library SWIG interface for CMake. Generates `fifechan.i`.

### Included Templates

- `extensions.i.templ`
 - Extends FIFE types (Point, Rect) with Python methods like `__str__`.
  - Included by `fife.i.templ.cm` and so part of `fife.i`.

## Build Process

1. CMake substitutes placeholders:
   - `@CMAKE_SWIG_IMPORTS@` + `@CMAKE_SWIG_IMPORTS_FCHAN@`: C++ headers to include
   - `@CMAKE_SWIG_INCLUSIONS@` + `@CMAKE_SWIG_INCLUSIONS_FCHAN@`: `%include` directives for .i files

2. SWIG processes the `.i` file and generates:
   - `fife.py` / `fifechan.py` - Python proxy classes
   - `_fife_swig.so` / `_fifechan_swig.so` - C extension module

## SWIG 4.x Workaround

`fifechan.i.templ.cm` contains a `#define SWIG_PYTHON_SILENT_MEMLEAK` to
silence false "no destructor found" warnings.
This is a known SWIG 4.x regression with multiple modules.
See https://github.com/swig/swig/issues/2154.
