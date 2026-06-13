#!/usr/bin/env python3
"""parse_sdl3.py — Extract SDLK_* keycode definitions from SDL3's SDL_keycode.h.

The SDL3 version is read from SDL_version.h located in the same include
directory as SDL_keycode.h.

Usage:
    parse_sdl3.py /path/to/SDL_keycode.h  # prints JSON to stdout

Output (JSON):
    {
      "version": [3, 4, 4],
      "keys": [
        {"name": "SDLK_UNKNOWN", "value": "0x00000000", "comment": "0"},
        ...
      ]
    }
"""

import json
import os
import re
import sys

SDLK_RE = re.compile(
    r"#define\s+(SDLK_\w+)\s+(0x[0-9a-fA-F]+)u\s*(?:\/\*\*?\s*<?\s*(.*?)\s*\*\/)?"
)

VERSION_RE = re.compile(
    r"#define\s+SDL_MAJOR_VERSION\s+(\d+)\s*"
    r".*?"
    r"#define\s+SDL_MINOR_VERSION\s+(\d+)\s*"
    r".*?"
    r"#define\s+SDL_MICRO_VERSION\s+(\d+)",
    re.DOTALL,
)


def read_version(keycode_path: str) -> list[int]:
    """Read SDL_MAJOR/MINOR/MICRO_VERSION from SDL_version.h in same dir."""
    inc_dir = os.path.dirname(os.path.abspath(keycode_path))
    ver_path = os.path.join(inc_dir, "SDL_version.h")
    if not os.path.isfile(ver_path):
        print(f"Error: cannot find SDL_version.h at {ver_path}", file=sys.stderr)
        sys.exit(1)
    with open(ver_path) as f:
        text = f.read()
    m = VERSION_RE.search(text)
    if not m:
        print(f"Error: cannot parse version from {ver_path}", file=sys.stderr)
        sys.exit(1)
    return [int(m.group(1)), int(m.group(2)), int(m.group(3))]


def parse(path: str) -> dict:
    with open(path) as f:
        text = f.read()
    no_block = re.sub(r"/\*.*?\*/", "", text, flags=re.DOTALL)
    keys = []
    for m in SDLK_RE.finditer(no_block):
        name = m.group(1)
        value = m.group(2).lower()
        comment = (m.group(3) or "").strip()
        keys.append({"name": name, "value": value, "comment": comment})
    version = read_version(path)
    return {"version": version, "keys": keys}


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <path/to/SDL_keycode.h>", file=sys.stderr)
        sys.exit(1)
    data = parse(sys.argv[1])
    json.dump(data, sys.stdout, indent=2)
