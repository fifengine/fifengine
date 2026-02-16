#!/usr/bin/env bash

# Exit on errors, unset vars, and pipe failures
set -euo pipefail

# Remember the directory the script was invoked from and restore it before exiting
START_DIR="$(pwd)"

cd /opt/vcpkg || exit 1

git pull

vcpkg integrate install

# Return to the original working directory (project root when invoked from there)
cd "$START_DIR" || exit 1
