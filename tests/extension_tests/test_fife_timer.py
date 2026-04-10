# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import fife.extensions.fife_timer


class TestFifeTimer:
    def test_module_docstring(self):
        doc = fife.extensions.fife_timer.__doc__
        assert "timer" in doc.lower()

    def test_manager_is_none_initially(self):
        from fife.extensions import fife_timer

        assert fife_timer._manager is None
