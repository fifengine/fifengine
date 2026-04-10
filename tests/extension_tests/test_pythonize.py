# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import fife.extensions.pythonize


class TestPythonize:
    def test_module_docstring(self):
        doc = fife.extensions.pythonize.__doc__
        assert "pythonic" in doc.lower()

    def test_all_is_empty(self):
        assert fife.extensions.pythonize.__all__ == ()
