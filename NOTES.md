# Notes

## Run black format

- /tmp/fife-python-venv/bin/black src/python/fife tests/extension_tests tests/swig_tests run_tests.py

## Run ruff fixes

- /tmp/fife-python-venv/bin/ruff --fix src/python/fife tests/extension_tests tests/swig_tests run_tests.py

# Dependencies

How to compile on linux-64:

- apt update && apt install -y python3-dev libxmu-dev libxi-dev libgl-dev autoconf libtool python3-jinja2 autoconf autoconf-archive automake libtool
- apt install -y libglu1-mesa-dev
- cd /workspaces/fifengine && VCPKG_FEATURE_FLAGS=manifests vcpkg install --triplet x64-linux


Build dependencies first:

pushd dependencies && mkdir -p build && cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DDEPENDENCY_INSTALL_DIR=${PWD}/../includes  -DCMAKE_CXX_COMPILER=g++-14 -G Ninja && cmake --build build -j 4 && popd

Then build fifengine:

mkdir -p out/build/clang20-x64-linux-rel && cmake -S . -B out/build/clang20-x64-linux-rel -DCMAKE_BUILD_TYPE=Release  -DCMAKE_CXX_COMPILER=g++-14 -Dfifechan=ON -Dbuild-python=ON -Dbuild-library=ON && cmake --build out/build/clang20-x64-linux-rel --parallel

1. analyze how to simplify the get-fifechan.cmake script. its triggered as part of the first build step (dependencies), before building fifengine itself
2. fifechan has itself dependencies on sdl2 components and opengl
3. for that, it might be useful to do vcpkg install as an independent build step and not as part of the cmakelists configure step of the fifengine project
4. which leads to 1) vcpkg deps = install all dependencies for fifengine and fifechan, 2) install dependencies fifechan and swig (which are not part of vcpkg, but use vcpkg dependencies themself), 3) build fifengine
