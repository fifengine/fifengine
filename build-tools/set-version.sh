#!/usr/bin/env bash

set -euo pipefail

# ------------------------------------------------------------------
# Setup Paths
# ------------------------------------------------------------------
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_ROOT"

# ------------------------------------------------------------------
# Extract Version from vcpkg.json using jq
# ------------------------------------------------------------------

if ! command -v jq &> /dev/null; then
    echo "Error: jq is required." >&2; exit 1
fi

VERSION=$(jq -r '."version-string"' vcpkg.json)
if [[ -z "$VERSION" || "$VERSION" == "null" ]]; then
    echo "Error: Could not read version from vcpkg.json" >&2; exit 1
fi

echo "Updating files with version: $VERSION"

# ------------------------------------------------------------------
# Insert Version Number
# ------------------------------------------------------------------

# Update doxygen.conf
# Match the string PROJECT_NUMBER ... = (version) and replace the version
sed -i "s/^\(PROJECT_NUMBER[[:space:]]*=[[:space:]]*\).*/\1${VERSION}/" doxygen.conf
echo "Updated doxygen.conf"

# Update pyproject.toml [project] version
sed -i "/^\[project\]/,/^\[.*\]/ s/^\(version[[:space:]]*=[[:space:]]*\).*/\1\"${VERSION}\"/" pyproject.toml
echo "Updated pyproject.toml"

# ------------------------------------------------------------------
# Remember that the following files should not be updated manually:
#  - version.hpp: is generated during build.
# ------------------------------------------------------------------

echo "Done."
