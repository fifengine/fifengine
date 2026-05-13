#!/usr/bin/env python3
"""generate.py — One-step SDL3 keycode → C++ header generator.

Usage:
    generate.py /path/to/SDL_keycode.h -o key_gen.h

Reads SDL3 version from SDL_version.h alongside SDL_keycode.h.
"""

import argparse
import os
import subprocess
import sys

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))


def main():
    p = argparse.ArgumentParser(description='Generate key_gen.h from SDL_keycode.h')
    p.add_argument('sdl_keycode_h', help='Path to SDL_keycode.h')
    p.add_argument('-o', '--output', required=True, help='Output header path')
    args = p.parse_args()

    parse_script = os.path.join(SCRIPT_DIR, 'parse_sdl3.py')
    gen_script = os.path.join(SCRIPT_DIR, 'generate_enum.py')

    parse_proc = subprocess.Popen(
        [sys.executable, parse_script, args.sdl_keycode_h],
        stdout=subprocess.PIPE,
    )
    gen_proc = subprocess.run(
        [sys.executable, gen_script],
        stdin=parse_proc.stdout,
        capture_output=True,
        text=True,
    )
    parse_proc.stdout.close()
    parse_proc.wait()

    if gen_proc.returncode != 0:
        print('Error generating key header:', gen_proc.stderr, file=sys.stderr)
        sys.exit(1)

    with open(args.output, 'w') as f:
        f.write(gen_proc.stdout)
    print(f'Wrote {args.output}', file=sys.stderr)


if __name__ == '__main__':
    main()
