# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

class TestPychanLayout:
    def test_layout_constants(self):
        from fife.extensions.pychan.widgets.layout import (
            AlignBottom,
            AlignCenter,
            AlignLeft,
            AlignRight,
            AlignTop,
        )

        assert AlignTop == 0
        assert AlignBottom == 1
        assert AlignLeft == 2
        assert AlignRight == 3
        assert AlignCenter == 4
