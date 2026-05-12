# Tests

This folder contains tests for FIFE.

The test files are organized into subfolders based on the type of tests and the subjects under test.

We use Catch2 for C++ tests and pytest for Python tests.

Pytest: https://docs.pytest.org/en/stable/
Catch2: https://catch2-temp.readthedocs.io/en/latest/index.html

### core_tests

C++ tests.
Catch2 based.
Subjects under test are in `/src/fife`.

### extension_test

Python tests for FIFE extensions, such as pychan and the XML serializer.
Pytest based.
Subjects under test are in `/src/python/fife/extensions`.

### fife_test

Python integration and benchmark tests.
Self-contained test application, not pytest based.
Subject under test is the whole engine, but especially the application loop and rendering.

### swig_tests

Python tests for the low-level SWIG bindings of FIFE's C++ API.
Pytest based.
The SWIG-generated files (fife.py, fifechan.py, _fife_swig.so, _fifechan_swig.so) are needed.
