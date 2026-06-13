# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Tests for issue #656: tuple style keys not properly overriding default styles."""

import pytest


def _make_manager():
    """Create a Manager instance without invoking __init__ (which needs engine).

    Returns
    -------
    Manager
        A Manager instance with default style registered.
    """
    from fife.extensions.pychan.internal import DEFAULT_STYLE, Manager

    mgr = object.__new__(Manager)
    mgr.styles = {}
    mgr.addStyle("default", DEFAULT_STYLE)
    return mgr


class TestStyleIssue656:
    """Reproduce and verify fix for issue #656."""

    @pytest.fixture(autouse=True)
    def _register_tooltip(self):
        """Register TooltipLabel as a test widget class."""
        from fife.extensions.pychan.widgets import Widget, registerWidget

        class TooltipLabel(Widget):
            def __init__(self, **kwargs):
                pass

        registerWidget(TooltipLabel)

    def test_tuple_keys_are_flattened_in_style(self):
        """Tuple keys should be resolved into individual keys in the stored style."""
        mgr = _make_manager()
        mgr.addStyle("custom", {("Label", "TooltipLabel"): {"border_size": 5}})

        stored = mgr.styles["custom"]
        has_tuple = any(isinstance(k, tuple) for k in stored if k != "default")
        assert not has_tuple, "Tuple keys should be flattened into individual keys"

    def test_custom_tuple_overrides_default_label(self):
        """Custom style with (Label, TooltipLabel) should override Label's default."""
        from fife.extensions.pychan.widgets.label import Label

        mgr = _make_manager()
        mgr.addStyle("custom", {("Label", "TooltipLabel"): {"border_size": 5}})

        stored = mgr.styles["custom"]
        assert Label in stored
        assert stored[Label]["border_size"] == 5, (
            f"Expected border_size=5 from custom style, got {stored[Label]['border_size']}"
        )

    def test_mixed_tuple_and_single_keys_in_one_style(self):
        """A style with both tuple and single keys should work correctly."""
        from fife.extensions.pychan.widgets.containers import Container, HBox, VBox
        from fife.extensions.pychan.widgets.label import Label

        mgr = _make_manager()
        mgr.addStyle(
            "mixed",
            {
                "Label": {"border_size": 3},
                ("Container", "HBox", "VBox"): {"opaque": False, "padding": 10},
            },
        )

        stored = mgr.styles["mixed"]

        assert stored[Label]["border_size"] == 3
        assert stored[Container]["opaque"] is False
        assert stored[Container]["padding"] == 10
        assert stored[HBox]["opaque"] is False
        assert stored[HBox]["padding"] == 10
        assert stored[VBox]["opaque"] is False
        assert stored[VBox]["padding"] == 10

    def test_default_style_containers_also_flattened(self):
        """The DEFAULT_STYLE's (Container, HBox, VBox) tuple is now flattened."""
        from fife.extensions.pychan.widgets.containers import Container, HBox, VBox

        mgr = _make_manager()

        for cls in (Container, HBox, VBox):
            assert cls in mgr.styles["default"], (
                f"{cls.__name__} should have its own key (flattened from tuple)"
            )
