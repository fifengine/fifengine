# Notes

# Dependencies

1. librocket
    - https://github.com/libRocket/libRocket
    - dead
    - not on vcpkg

2. unittest++
    - https://github.com/unittest-cpp/unittest-cpp
    - dead
    - used in tests/core_testS/fife_unittest.h


TODO convert tests to doctest

How to compile on linux-64:

- apt update && apt install -y python3-dev libxmu-dev libxi-dev libgl-dev autoconf libtool python3-jinja2 autoconf autoconf-archive automake libtool
- apt install -y libglu1-mesa-dev
- cd /workspaces/fifengine && VCPKG_FEATURE_FLAGS=manifests vcpkg install --triplet x64-linux
