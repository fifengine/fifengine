# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestFifeCompat:
    def test_deprecated_function(self):
        from fife.extensions.fife_compat import deprecated

        deprecated("2008.1", "test message")

    def testing_annotation_this_is_deprecated(self):
        from fife.extensions.fife_compat import this_is_deprecated

        @this_is_deprecated
        def old_func():
            return "result"

        result = old_func()
        assert result == "result"

    def test_compat_nonconsumable_keys_exists(self):
        from fife.extensions.fife_compat import _compat_NonConsumableKeys

        assert _compat_NonConsumableKeys is not None

    def test_module_docstring(self):
        import fife.extensions.fife_compat

        assert "compatibility" in fife.extensions.fife_compat.__doc__.lower()
