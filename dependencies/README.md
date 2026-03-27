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
