#!/usr/bin/env bash

set -euo pipefail

echo "Cppcheck version: $(cppcheck --version)"

cppcheck \
    --enable=warning,style,performance,portability \
    --std=c++20 \
    --quiet \
    --inline-suppr \
    --error-exitcode=1 \
    src/fife tests/core_tests > cppcheck_run.log 2> cppcheck_err.log || true
