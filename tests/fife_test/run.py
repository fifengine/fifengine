#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import os
import sys

fife_path = os.path.join("..", "..", "engine", "python", "fife")
if os.path.isdir(fife_path) and fife_path not in sys.path:
    sys.path.insert(0, fife_path)

from fife import fife

print("Using the FIFE python module found here: ", os.path.dirname(fife.__file__))

from fife.extensions.fife_settings import Setting

from scripts.fife_test import FifeTestApplication

TDS = Setting(app_name="fife_test", settings_file="./settings.xml")


def main():
    app = FifeTestApplication(TDS)
    app.run()


if __name__ == "__main__":
    if TDS.get("FIFE", "ProfilingOn"):
        import hotshot
        import hotshot.stats

        print("Starting profiler")
        prof = hotshot.Profile("fife.prof")
        prof.runcall(main)
        prof.close()
        print("analysing profiling results")
        stats = hotshot.stats.load("fife.prof")
        stats.strip_dirs()
        stats.sort_stats("time", "calls")
        stats.print_stats(20)
    else:
        main()
