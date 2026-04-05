#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

# ------------------------------------------------------------------------------
# This is the RPG demo of Fifengine.
# ------------------------------------------------------------------------------

import cProfile
import os

from fife import fife  # noqa: E402
from fife.extensions.pychan.fife_pychansettings import FifePychanSettings  # noqa: E402
from scripts.rpg import RPGApplication  # noqa: E402

print("Using the FIFE python module found here: ", os.path.dirname(fife.__file__))

TDS = FifePychanSettings(app_name="rpg", settings_file="./settings.xml")


def main():
    app = RPGApplication(TDS)
    app.run()


if __name__ == "__main__":
    if TDS.get("FIFE", "ProfilingOn"):
        print("Starting profiler")
        prof = cProfile.Profile()
        prof.runcall(main)
        prof.dump_stats("fife.prof")
        print("analysing profiling results")
        import pstats

        stats = pstats.Stats("fife.prof")
        stats.strip_dirs()
        stats.sort_stats("time", "calls")
        stats.print_stats(20)
    else:
        main()
