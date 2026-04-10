# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestPythonize:
    def test_module_docstring(self):
        from fife.extensions import pythonize

        assert "pythonic" in pythonize.__doc__.lower()

    def test_all_is_empty(self):
        from fife.extensions import pythonize

        assert pythonize.__all__ == ()

    def test_exception_str_patched(self):
        from fife import fife

        assert hasattr(fife.Exception, "__str__")
