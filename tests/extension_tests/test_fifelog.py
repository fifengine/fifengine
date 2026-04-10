# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import fife.extensions.fifelog


class TestFifelog:
    def test_module_docstring(self):
        doc = fife.extensions.fifelog.__doc__
        assert "log" in doc.lower()

    def test_log_manager_exists(self):
        from fife.extensions.fifelog import LogManager

        assert LogManager is not None

    def test_log_manager_init(self):
        from fife.extensions.fifelog import LogManager

        assert hasattr(LogManager, "__init__")
