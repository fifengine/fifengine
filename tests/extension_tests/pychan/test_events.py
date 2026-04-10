# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestEvents:
    def test_module_docstring(self):
        from fife.extensions.pychan import events

        assert "event" in events.__doc__.lower()

    def test_event_mapper_exists(self):
        from fife.extensions.pychan.events import EventMapper

        assert EventMapper is not None

    def test_event_listener_base_exists(self):
        from fife.extensions.pychan.events import EventListenerBase

        assert EventListenerBase is not None
