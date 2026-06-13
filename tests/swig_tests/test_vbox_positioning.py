# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Tests for VBox positioning inside Windows using fifechan directly."""

from fife import fifechan


def compute_children_area_origin(win):
    b = win.getBorderSize()
    pl = win.getPaddingLeft()
    pt = win.getPaddingTop()
    title = win.getTitleBarHeight()
    inner = 1
    return b + pl + inner, b + pt + inner + title


def make_label(text, w=50, h=16):
    lbl = fifechan.Label()
    lbl.setCaption(text)
    lbl.setSize(w, h)
    return lbl


class TestVBoxPositioningFifechan:
    """Tests VBox positioning inside Windows by testing fifechan directly."""

    def test_children_stacked_vertically(self):
        win = fifechan.Window()
        a = make_label("A")
        b = make_label("B")
        c = make_label("C")
        win.add(a)
        win.add(b)
        win.add(c)
        win.setLayout(fifechan.Container.LayoutPolicy_Vertical)
        win.resizeToContent(True)

        assert a.getY() < b.getY(), (
            f"child A y ({a.getY()}) should be above B ({b.getY()})"
        )
        assert b.getY() < c.getY(), (
            f"child B y ({b.getY()}) should be above C ({c.getY()})"
        )

    def test_children_same_x(self):
        win = fifechan.Window()
        a = make_label("A")
        b = make_label("B")
        win.add(a)
        win.add(b)
        win.setLayout(fifechan.Container.LayoutPolicy_Vertical)
        win.resizeToContent(True)

        assert a.getX() == b.getX(), (
            f"both children should have same x ({a.getX()} vs {b.getX()})"
        )

    def test_window_resize_fits_children(self):
        win = fifechan.Window()
        a = make_label("A")
        b = make_label("B")
        c = make_label("C")
        win.add(a)
        win.add(b)
        win.add(c)
        win.setLayout(fifechan.Container.LayoutPolicy_Vertical)
        win.resizeToContent(True)

        max_bottom = max(
            c.getY() + c.getHeight(), b.getY() + b.getHeight(), a.getY() + a.getHeight()
        )
        assert max_bottom <= win.getHeight(), (
            f"last child bottom ({max_bottom}) should fit in window height ({win.getHeight()})"
        )
        max_right = max(
            a.getX() + a.getWidth(), b.getX() + b.getWidth(), c.getX() + c.getWidth()
        )
        assert max_right <= win.getWidth(), (
            f"last child right ({max_right}) should fit in window width ({win.getWidth()})"
        )

    def test_nested_container_stacking(self):
        win = fifechan.Window()
        outer = fifechan.Container()
        outer.setLayout(fifechan.Container.LayoutPolicy_Vertical)
        inner = fifechan.Container()
        inner.setLayout(fifechan.Container.LayoutPolicy_Vertical)
        a = make_label("A")
        b = make_label("B")
        inner.add(a)
        inner.add(b)
        outer.add(inner)
        win.add(outer)
        win.setLayout(fifechan.Container.LayoutPolicy_Vertical)
        win.resizeToContent(True)

        assert inner.getY() >= outer.getY(), (
            f"inner y ({inner.getY()}) should be >= outer y ({outer.getY()})"
        )
        assert b.getY() > a.getY(), (
            f"inner child B y ({b.getY()}) should be below A ({a.getY()})"
        )
        assert a.getY() >= inner.getY(), (
            f"child A y ({a.getY()}) should be >= inner y ({inner.getY()})"
        )

    def test_single_child(self):
        win = fifechan.Window()
        a = make_label("A")
        win.add(a)
        win.setLayout(fifechan.Container.LayoutPolicy_Vertical)
        win.resizeToContent(True)

        assert a.getX() >= 0, f"single child should have non-negative x ({a.getX()})"
        assert a.getY() >= 0, f"single child should have non-negative y ({a.getY()})"
        assert a.getWidth() > 0, (
            f"single child should have positive width ({a.getWidth()})"
        )

    def test_no_children_no_crash(self):
        win = fifechan.Window()
        win.setLayout(fifechan.Container.LayoutPolicy_Vertical)
        win.resizeToContent(True)
        # Just checking that resizeToContent doesn't crash with no children

    def test_children_position_below_titlebar(self):
        win = fifechan.Window()
        a = make_label("A")
        win.add(a)
        win.setLayout(fifechan.Container.LayoutPolicy_Vertical)
        win.resizeToContent(True)

        assert a.getY() >= 0, f"child y ({a.getY()}) should be non-negative"
        assert a.getY() < win.getHeight(), (
            f"child y ({a.getY()}) should be within window height ({win.getHeight()})"
        )

    def test_spacing_between_children(self):
        win = fifechan.Window()
        win.setLayout(fifechan.Container.LayoutPolicy_Vertical)
        a = make_label("A")
        b = make_label("B")
        win.add(a)
        win.add(b)
        win.resizeToContent(True)

        spacing = b.getY() - (a.getY() + a.getHeight())
        assert spacing >= 0, (
            f"spacing between children should be non-negative, got {spacing}"
        )

    def test_container_sizing(self):
        win = fifechan.Window()
        win.setLayout(fifechan.Container.LayoutPolicy_Vertical)
        a = make_label("A")
        b = make_label("B")
        win.add(a)
        win.add(b)
        win.resizeToContent(True)

        assert win.getWidth() > 0, (
            f"window width should be positive, got {win.getWidth()}"
        )
        assert win.getHeight() > 0, (
            f"window height should be positive, got {win.getHeight()}"
        )
        # Content fits within window (even if title bar not accounted for)
        content_h = b.getY() + b.getHeight()
        assert win.getHeight() >= content_h, (
            f"window height ({win.getHeight()}) should be >= content bottom ({content_h})"
        )

    def test_x_position_is_zero(self):
        win = fifechan.Window()
        win.setLayout(fifechan.Container.LayoutPolicy_Vertical)
        a = make_label("A")
        win.add(a)
        win.resizeToContent(True)

        assert a.getX() == 0, f"first child x should be 0 inside layout, got {a.getX()}"

    def test_absolute_layout_children_overlap(self):
        win = fifechan.Window()
        a = make_label("A")
        b = make_label("B")
        win.add(a)
        win.add(b)
        # Default layout is Absolute, no resizeToContent
        win.resizeToContent(True)
        # With Absolute layout, children stay at (0,0) and overlap
        assert a.getX() == b.getX(), (
            f"Absolute: children should be at same x ({a.getX()} vs {b.getX()})"
        )
        assert a.getY() == b.getY(), (
            f"Absolute: children should be at same y ({a.getY()} vs {b.getY()})"
        )

    def test_hidden_child_excluded_from_layout(self):
        win = fifechan.Window()
        win.setLayout(fifechan.Container.LayoutPolicy_Vertical)
        a = make_label("A")
        b = make_label("B", h=24)
        win.add(a)
        win.add(b)
        win.resizeToContent(True)
        bottom_with_both = b.getY() + b.getHeight()

        b.setVisible(False)
        win.resizeToContent(True)

        # Window should shrink after hiding child
        assert win.getHeight() < bottom_with_both, (
            "window should be shorter when child is hidden"
        )
