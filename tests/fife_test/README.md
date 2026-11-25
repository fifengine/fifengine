# FIFE Integration and Benchmark Test Suite

## Overview

The `/tests/fife_test/` directory contains an integration/benchmark test suite.

Its designed for interactive testing of the full engine.

## Files

- `run.py` - Is the entry point and provides an interactive test selector
- `scripts/fife_test.py` - Main application with test selection UI
- `scripts/test.py` - Base `Test` class all tests extend
- `tests/*.py` - Individual test implementations (BenchmarkTest, PathfinderTest, etc.)

## Details

- Interactive GUI: A menu lets users pick which test to run
- Full engine running: Loads actual game maps (`data/maps/*.xml`)
- Input handling: Mouse/keyboard listeners for manual interaction
- Benchmarking: Includes profiling with Python's `cProfile` module
- Test base class: Tests extend `scripts.test.Test` (not pytest)

## ParticleTest

`tests/ParticleTest.py` is an interactive smoke demo of the CPU particle
engine (rain, smoke and sparks). Run it with:

```
cd tests/fife_test && python run.py
```

Then select `ParticleTest` from the menu. The engine advances emitters once
per frame through `TimeManager`; the test itself never ticks them.

Budgets and caveats: roughly 1k–3k particles at 60 FPS on the SDL backend and
5k+ on OpenGL. Particle images are drawn through `Image::render()`, so the
appearance may differ between backends; identical output is not promised.
