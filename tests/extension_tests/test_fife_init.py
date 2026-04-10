# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestFifeModuleImports:
    def test_fife_import(self):
        from fife import fife

        assert fife is not None

    def test_fifechan_import(self):
        from fife import fifechan

        assert fifechan is not None
