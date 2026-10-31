#!/usr/bin/env bash

set -euo pipefail

# Keep this in sync with the cppcheck job in .github/workflows/qa.yml.
# The PyPI wheel bundles a pinned upstream build: cppcheck-wheel 1.5.1 == 2.17.1.
if [[ -n "${CPPCHECK:-}" ]]; then
  read -r -a CPPCHECK_CMD <<< "${CPPCHECK}"
elif command -v uv >/dev/null 2>&1; then
  CPPCHECK_CMD=(uv tool run --from cppcheck==1.5.1 cppcheck)
else
  CPPCHECK_CMD=(cppcheck)
fi

echo "Cppcheck version: $("${CPPCHECK_CMD[@]}" --version)"

status=0
# --check-level=exhaustive also drops the "Limiting analysis of branches"
# information messages, which would otherwise trip --error-exitcode even
# though they are not findings.
"${CPPCHECK_CMD[@]}" \
    --enable=warning,style,performance,portability \
    --std=c++20 \
    --check-level=exhaustive \
    --quiet \
    --inline-suppr \
    --error-exitcode=1 \
    src/fife tests/core_tests > cppcheck.log 2> cppcheck_error.log || status=$?

echo "Cppcheck exit code: ${status} (details in cppcheck.log / cppcheck_error.log)"
exit "${status}"
