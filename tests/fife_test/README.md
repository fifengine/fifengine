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
