# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import fife.extensions.pychan.compat as compat


class TestPychanCompat:
    def test_module_docstring(self):
        doc = compat.__doc__
        assert "compatibility" in doc.lower()

    def test_in_fife_variable_exists(self):
        assert compat.in_fife is not None

    def test_point_class(self):
        from fife.extensions.pychan.compat import _point

        p = _point(10, 20)
        assert p.x == 0
        assert p.y == 0
