# Dependencies for FIFE

The required dependencies for FIFE that are not available in the vcpkg repository are:

- Swig
- Fifechan

# How to build

You first install the needed VCPKG dependencies:

> ./build-tools/setup-vcpkg.sh

Then you can build the dependencies that are not available in VCPKG:

> ./dependencies/build.sh

For the exact build commands, see the `build.sh` script in this folder.

## Requirements

**CMake >= 4.3** is required: the vcpkg port scripts use
`string(JSON ... STRING_ENCODE)` (microsoft/vcpkg#53260). Older CMake (Debian
ships 3.31.x) fails inside a port build with a confusing
`invalid mode 'STRING_ENCODE'`, since vcpkg uses its own CMake from `PATH`.

Both `CMakeLists.txt` files enforce this as a hard floor. Ensure the newer
CMake is on `PATH` before invoking `vcpkg` or the build scripts, e.g.:

> export PATH=/opt/cmake/bin:$PATH
> cmake --version  # should be >= 4.3

