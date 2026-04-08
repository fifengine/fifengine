#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import os
import sys

print("Working directory:", os.getcwd())

fife_path = os.path.join("..", "..", "src", "python")
if os.path.isdir(fife_path) and fife_path not in sys.path:
    sys.path.insert(0, fife_path)

from fife import fife  # noqa: E402

print(
    "Using the FIFE python module found here: ",
    os.path.dirname(os.path.abspath(fife.__file__)),
)

from fife.extensions.fife_settings import Setting  # noqa: E402
from scripts.fife_test import FifeTestApplication  # noqa: E402

TDS = Setting(app_name="fife_test", settings_file="./settings.xml")


def main():
    app = FifeTestApplication(TDS)
    app.run()


if __name__ == "__main__":
    if TDS.get("FIFE", "ProfilingOn"):
        import cProfile
        import pstats

        print("Starting profiler")
        prof = cProfile.Profile()
        prof.runcall(main)
        print("analysing profiling results")
        stats = pstats.Stats(prof)
        stats.strip_dirs()
        stats.sort_stats("time", "calls")
        stats.print_stats(20)
    else:
        main()
