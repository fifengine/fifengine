# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestFifeUtilsModule:
    def test_all_exports(self):
        from fife.extensions import fife_utils

        expected = ["is_fife_exc", "getUserDataDirectory"]
        for name in expected:
            assert name in fife_utils.__all__

    def test_module_docstring(self):
        from fife.extensions import fife_utils

        assert (
            "utility" in fife_utils.__doc__.lower()
            or "function" in fife_utils.__doc__.lower()
        )
