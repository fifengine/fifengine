# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestFifeTimer:
    def test_timer_class_exists(self):
        from fife.extensions.fife_timer import Timer

        assert Timer is not None
