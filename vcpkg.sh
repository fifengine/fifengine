#!/bin/zsh

# Save the current directory
pushd .

# Update vcpkg
cd /opt/vcpkg
git pull
./bootstrap-vcpkg.sh

# Return to the original directory
popd

# system packages required :

apt update
apt install -y python3-dev

# GLEW requirements
apt install -y libxmu-dev libxi-dev libgl-dev

# ALSA requirements
apt install -y autoconf libtool

apt install -y python3-jinja2

# libxcrypt requirements
apt install -y autoconf autoconf-archive automake libtool

# Ensure VCPKG_FEATURE_FLAGS is set for manifest mode
export VCPKG_FEATURE_FLAGS=manifests

# Install vcpkg
vcpkg install --triplet x64-linux

