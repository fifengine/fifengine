#!/usr/bin/env bash
shopt -s expand_aliases

#
# A build script for building Fife Engine on a mac osx machine
#
# https://github.com/fifengine/fifengine
#

echo
echo -e "\e[1;32m\tBuilding and installing FIFE Engine \e[0m"
echo -e "\t----------------------------"
echo

# how many virtual processors are there?
export NUMCPUS=`grep ^processor /proc/cpuinfo | wc -l`

# parallel make
alias pmake='time ionice -c3 nice -n 19 make -j$NUMCPUS --load-average=$NUMCPUS'

# Install all package dependencies
function install_dependencies() {
    echo
    echo -e "\e[1;33mInstalling package dependencies...\e[0m"
    echo

    brew install swig sdl_image sdl_mixer sdl_ttf scons boost-jam wget git libvorbis libogg

    pip install pyrex --allow-all-external --allow-unverified pyrex
    pip install pyyaml cython

    echo -e "\e[1;32m> Done.\e[0m"
    echo
}

# fifechan
function install_fifechan() {
    echo
    echo -e "\e[1;33mInstalling fifechan...\e[0m"
    echo

    git clone --quiet --depth 1 git://github.com/fifengine/fifechan.git
    # https://github.com/fifengine/fifechan/blob/master/INSTALL.md
    cd fifechan
    cmake .
    make
    sudo make install
    cd ..

    echo -e "\e[1;32m> Done.\e[0m"
    echo
}

function build() {
    # https://github.com/fifengine/fifengine/wiki/scons
    scons
}

function install() {
    install_dependencies
      install_fifechan
    build
}

install

## Success
echo
echo -e "\e[1;32m *** Fife Engine is now installed! *** \e[0m"
echo

exit 0
