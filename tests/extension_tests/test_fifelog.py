# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import fife.extensions.fifelog


class TestFifelog:
    def test_module_docstring(self):
        doc = fife.extensions.fifelog.__doc__
        assert "log" in doc.lower()
