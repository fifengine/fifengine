# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import ctypes
import json
import os
import pickle
import random
import sys
import time
from pathlib import Path


def _prepend_env_path(var_name, path):
    path_str = str(path)
    current = os.environ.get(var_name, "")
    parts = [p for p in current.split(os.pathsep) if p]
    if path_str in parts:
        return
    os.environ[var_name] = path_str if not current else path_str + os.pathsep + current


def _bootstrap_runtime_paths(repo_root):
    local_build = repo_root / "out" / "build" / "clang22-x64-linux-dbg-cov"
    if local_build.is_dir() and str(local_build) not in sys.path:
        sys.path.insert(0, str(local_build))
        _prepend_env_path("PYTHONPATH", local_build)

    dependency_lib = repo_root / "out" / "fife-dependencies" / "x64-linux" / "install" / "lib"
    if dependency_lib.is_dir():
        _prepend_env_path("LD_LIBRARY_PATH", dependency_lib)


def _set_headless_defaults():
    os.environ.setdefault("SDL_VIDEODRIVER", "dummy")
    os.environ.setdefault("SDL_AUDIODRIVER", "dummy")


def _preload_native_libs(repo_root):
    candidates = [
        repo_root / "out" / "fife-dependencies" / "x64-linux" / "install" / "lib" / "libfifechan.so.0.2.0",
        repo_root / "out" / "fife-dependencies" / "x64-linux" / "install" / "lib" / "libfifechan.so",
        repo_root / "out" / "build" / "clang22-x64-linux-dbg-cov" / "libfifengine.so.0.5.0",
        repo_root / "out" / "build" / "clang22-x64-linux-dbg-cov" / "libfifengine.so",
    ]
    for lib in candidates:
        if lib.is_file():
            ctypes.CDLL(str(lib), mode=ctypes.RTLD_GLOBAL)


def _measure(label, fn):
    start = time.perf_counter()
    fn()
    end = time.perf_counter()
    return label, end - start


def _build_engine(fife, repo_root):
    engine = fife.Engine()
    settings = engine.getSettings()
    settings.setRenderBackend("SDL")
    settings.setScreenWidth(1)
    settings.setScreenHeight(1)
    settings.setFullScreen(False)
    settings.setDisplay(0)
    settings.setDefaultFontPath(str(repo_root / "tests" / "data" / "FreeMono.ttf"))
    settings.setDefaultFontGlyphs(
        " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
        + ".,!?-+/:();%`'*#=[]"
    )
    settings.setDefaultFontSize(12)
    settings.setWindowTitle("FIFE time64 benchmark")
    engine.init()
    return engine


def main():
    repo_root = Path(__file__).resolve().parents[2]
    _bootstrap_runtime_paths(repo_root)
    _set_headless_defaults()
    _preload_native_libs(repo_root)

    src_python = repo_root / "src" / "python"
    if src_python.is_dir() and str(src_python) not in sys.path:
        sys.path.insert(0, str(src_python))

    from fife import fife  # noqa: PLC0415

    engine = _build_engine(fife, repo_root)

    try:
        rng = random.Random(9001)

        # Scheduler performance proxy: TimeManager update throughput.
        tm = engine.getTimeManager()

        def bench_scheduler_updates():
            for _ in range(25000):
                tm.update()

        # Event queue overhead proxy: event timestamp set/get including boundary-heavy values.
        cmd = fife.Command()
        event_values = [
            0,
            1,
            0xFFFFFFFF,
            0x100000000,
            0xFFFFFFFFFFFFFFFF,
            *(rng.getrandbits(64) for _ in range(10000)),
        ]

        def bench_event_timestamp_ops():
            for v in event_values:
                cmd.setTimeStamp64(v)
                _ = cmd.getTimeStamp64()
                _ = cmd.getTimeStamp()

        # Serialization impact: JSON and pickle for same boundary-heavy dataset.
        payload = {"timestamps": event_values}

        def bench_json_roundtrip():
            blob = json.dumps(payload)
            _ = json.loads(blob)

        def bench_pickle_roundtrip():
            blob = pickle.dumps(payload)
            _ = pickle.loads(blob)

        results = [
            _measure("scheduler_performance_updates_25000", bench_scheduler_updates),
            _measure("event_queue_overhead_timestamp_ops_10005", bench_event_timestamp_ops),
            _measure("serialization_impact_json_roundtrip", bench_json_roundtrip),
            _measure("serialization_impact_pickle_roundtrip", bench_pickle_roundtrip),
        ]

        print("time64 benchmark results (seconds)")
        for label, elapsed in results:
            print(f"- {label}: {elapsed:.6f}")

    finally:
        engine.destroy()


if __name__ == "__main__":
    main()
