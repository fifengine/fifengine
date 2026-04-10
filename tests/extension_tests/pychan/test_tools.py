# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import pytest
from fife.extensions.pychan import tools
from fife.extensions.pychan.exceptions import PrivateFunctionalityError


class TestPychanTools:
    def test_apply_only_suitable_filters_kwargs(self):
        def func(a, b=0):
            return a + b

        result = tools.applyOnlySuitable(func, 1, c=3, b=2)
        assert result == 3

    def test_apply_only_suitable_with_no_matching_kwargs(self):
        def func(a, b=0):
            return a + b

        result = tools.applyOnlySuitable(func, 1, c=3, d=4)
        assert result == 1

    def test_apply_only_suitable_variadic(self):
        def func(*args):
            return sum(args)

        result = tools.applyOnlySuitable(func, 1, 2, 3, 4)
        assert result == 10

    def test_callback_with_arguments(self):
        calls = []

        def callback(a, b):
            calls.append((a, b))

        cb = tools.callbackWithArguments(callback, 1, 2)
        cb()
        assert calls == [(1, 2)]

    def test_callback_with_kwargs(self):
        calls = []

        def callback(a, b=None):
            calls.append((a, b))

        cb = tools.callbackWithArguments(callback, 1, b="test")
        cb()
        assert calls == [(1, "test")]

    def test_attr_set_callback(self):
        class MockWidget:
            pass

        widget = MockWidget()
        widget.value = None

        cb = tools.attrSetCallback(value="test_value", text="hello")
        cb(widget=widget)
        assert widget.value == "test_value"
        assert widget.text == "hello"

    def test_attr_set_callback_with_do_methods(self):
        class MockWidget:
            def update(self):
                self.updated = True

        widget = MockWidget()
        widget.value = None

        cb = tools.attrSetCallback(value="test", do__update=True)
        cb(widget=widget)
        assert widget.value == "test"
        assert widget.updated is True

    def test_attr_set_callback_private_raises(self):
        with pytest.raises(PrivateFunctionalityError):
            tools.attrSetCallback(_private="test")

    def test_chain_callbacks(self):
        calls = []

        def cb1(event=0, widget=0):
            calls.append("cb1")

        def cb2(event=0, widget=0):
            calls.append("cb2")

        chain = tools.chainCallbacks(cb1, cb2)
        chain()
        assert calls == ["cb1", "cb2"]

    def test_repeat_alot(self):
        counter = {"value": 0}

        @tools.repeatALot(n=5)
        def func():
            counter["value"] += 1

        result = func()
        assert counter["value"] == 6
        assert result is None

    def test_this_is_deprecated(self, capsys):
        @tools.this_is_deprecated
        def old_func():
            return "result"

        result = old_func()
        assert result == "result"
        captured = capsys.readouterr()
        assert "deprecated" in captured.out.lower()
