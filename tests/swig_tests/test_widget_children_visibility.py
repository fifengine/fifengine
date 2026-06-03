# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Regression tests for child widget visibility in containers with chrome.

This tests the fix for the `Widget::_draw` intersection check
in fifechan's `widget.cpp`. The original code compared the child's
raw position against the children area, but for widgets with chrome
(e.g. Window with title bar), the children area has a non-zero origin.
A child at (0,0) was wrongly culled because its raw position didn't
overlap with the offset children area, even though the clip stack
would correctly render it at the content area's top-left.

The fix offsets the child's position by the children area origin
before checking intersection, matching what the clip stack produces.

These tests verify the rendered intersection logic directly.
"""

from fife import fifechan


def compute_children_area_origin(win):
    """Return (x, y) origin of the Window's children area in parent space."""
    b = win.getBorderSize()
    pl = win.getPaddingLeft()
    pt = win.getPaddingTop()
    title = win.getTitleBarHeight()
    inner = 1
    return b + pl + inner, b + pt + inner + title


def make_widget(w=50, h=16):
    """Create a simple Container widget for testing visibility."""
    wgt = fifechan.Container()
    wgt.setSize(w, h)
    return wgt


def rects_intersect(r1, r2):
    """Check if two (x, y, w, h) rectangles intersect.

    Mirrors fifechan's ``Rectangle::isIntersecting`` logic.
    """
    x1, y1, w1, h1 = r1
    x2, y2, w2, h2 = r2

    x1_ = x1
    y1_ = y1
    w1_ = w1
    h1_ = h1

    x1_ -= x2
    y1_ -= y2

    if x1_ < 0:
        w1_ += x1_
    elif x1_ + w1_ > w2:
        w1_ = w2 - x1_

    if y1_ < 0:
        h1_ += y1_
    elif y1_ + h1_ > h2:
        h1_ = h2 - y1_

    return w1_ > 0 and h1_ > 0


class TestWidgetChildrenVisibility:
    """Tests that child widgets at (0,0) in a Window are correctly
    considered visible inside the children area."""

    def test_child_at_origin_is_visible(self):
        """A child at (0,0) in a Window should be visible.

        The rendered position is (childrenArea.x + 0, childrenArea.y + 0),
        which always intersects with the children area.
        """
        win = fifechan.Window()
        win.setSize(200, 200)
        wgt = make_widget()
        win.add(wgt)

        ca_x, ca_y = compute_children_area_origin(win)
        children_area = (ca_x, ca_y, win.getWidth(), win.getHeight())

        rendered = (ca_x + wgt.getX(), ca_y + wgt.getY(),
                    wgt.getWidth(), wgt.getHeight())

        assert rects_intersect(children_area, rendered), (
            f"child at ({wgt.getX()}, {wgt.getY()}) should be visible "
            f"in children area origin ({ca_x}, {ca_y}), "
            f"rendered={rendered} children_area={children_area}"
        )

    def test_child_scrolled_out_above_is_hidden(self):
        """A child scrolled far above the visible area should be hidden."""
        win = fifechan.Window()
        win.setSize(200, 200)
        wgt = make_widget()
        wgt.setPosition(0, -100)
        win.add(wgt)

        ca_x, ca_y = compute_children_area_origin(win)
        children_area = (ca_x, ca_y, win.getWidth(), win.getHeight())

        rendered = (ca_x + wgt.getX(), ca_y + wgt.getY(),
                    wgt.getWidth(), wgt.getHeight())

        assert not rects_intersect(children_area, rendered), (
            f"child at ({wgt.getX()}, {wgt.getY()}) should be hidden "
            f"above children area, rendered={rendered}"
        )

    def test_child_scrolled_out_below_is_hidden(self):
        """A child positioned below the visible area should be hidden."""
        win = fifechan.Window()
        win.setSize(200, 200)
        wgt = make_widget()
        wgt.setPosition(0, 1000)
        win.add(wgt)

        ca_x, ca_y = compute_children_area_origin(win)
        children_area = (ca_x, ca_y, win.getWidth(), win.getHeight())

        rendered = (ca_x + wgt.getX(), ca_y + wgt.getY(),
                    wgt.getWidth(), wgt.getHeight())

        assert not rects_intersect(children_area, rendered), (
            f"child at ({wgt.getX()}, {wgt.getY()}) should be hidden "
            f"below children area, rendered={rendered}"
        )

    def test_child_at_content_origin_is_visible(self):
        """A child explicitly positioned at the children area origin
        should be visible (has some intersection)."""
        win = fifechan.Window()
        win.setSize(200, 200)
        wgt = make_widget()
        ca_x, ca_y = compute_children_area_origin(win)
        wgt.setPosition(ca_x, ca_y)
        win.add(wgt)

        children_area = (ca_x, ca_y, win.getWidth(), win.getHeight())
        rendered = (ca_x + wgt.getX(), ca_y + wgt.getY(),
                    wgt.getWidth(), wgt.getHeight())

        assert rects_intersect(children_area, rendered), (
            f"child explicitly at children area origin ({ca_x}, {ca_y}) "
            f"should be visible, rendered={rendered}"
        )

    def test_child_partially_visible_is_visible(self):
        """A child partially inside the children area should be visible."""
        win = fifechan.Window()
        win.setSize(200, 200)
        wgt = make_widget(w=10, h=20)
        ca_x, ca_y = compute_children_area_origin(win)
        wgt.setPosition(ca_x, ca_y - 10)
        win.add(wgt)

        children_area = (ca_x, ca_y, win.getWidth(), win.getHeight())
        rendered = (ca_x + wgt.getX(), ca_y + wgt.getY(),
                    wgt.getWidth(), wgt.getHeight())

        assert rects_intersect(children_area, rendered), (
            f"partially visible child should intersect, rendered={rendered}"
        )

    def test_zero_size_child_not_visible(self):
        """A child with zero width or height should not be visible."""
        win = fifechan.Window()
        win.setSize(200, 200)
        wgt = make_widget(w=0, h=16)
        win.add(wgt)

        ca_x, ca_y = compute_children_area_origin(win)
        children_area = (ca_x, ca_y, win.getWidth(), win.getHeight())
        rendered = (ca_x + wgt.getX(), ca_y + wgt.getY(),
                    wgt.getWidth(), wgt.getHeight())

        assert not rects_intersect(children_area, rendered), (
            f"zero-width child should not be visible, rendered={rendered}"
        )

    def test_child_right_of_content_area_is_hidden(self):
        """A child completely to the right of the content area
        should be hidden."""
        win = fifechan.Window()
        win.setSize(200, 200)
        wgt = make_widget()
        win.add(wgt)

        ca_x, ca_y = compute_children_area_origin(win)
        wgt.setPosition(win.getWidth() + 10, ca_y)

        children_area = (ca_x, ca_y, win.getWidth(), win.getHeight())
        rendered = (ca_x + wgt.getX(), ca_y + wgt.getY(),
                    wgt.getWidth(), wgt.getHeight())

        assert not rects_intersect(children_area, rendered), (
            f"child right of content should be hidden, rendered={rendered}"
        )

    def test_child_in_vbox_in_window_is_visible(self):
        """A child inside a VBox inside a Window should be visible."""
        win = fifechan.Window()
        win.setSize(200, 200)
        vbox = fifechan.Container()
        vbox.setLayout(fifechan.Container.LayoutPolicy_Vertical)
        win.add(vbox)

        child = make_widget()
        vbox.add(child)
        vbox.resizeToContent(True)

        ca_x, ca_y = compute_children_area_origin(win)
        children_area = (ca_x, ca_y, win.getWidth(), win.getHeight())

        vbox_rendered = (ca_x + vbox.getX(), ca_y + vbox.getY(),
                         vbox.getWidth(), vbox.getHeight())

        assert rects_intersect(children_area, vbox_rendered), (
            f"VBox in Window should be visible, rendered={vbox_rendered}"
        )

        vbox_ca_x = 0
        vbox_ca_y = 0
        child_rendered = (vbox_rendered[0] + vbox_ca_x + child.getX(),
                          vbox_rendered[1] + vbox_ca_y + child.getY(),
                          child.getWidth(), child.getHeight())

        assert rects_intersect(children_area, child_rendered), (
            f"nested child in VBox should be visible, "
            f"vbox_rendered={vbox_rendered} child_rendered={child_rendered}"
        )
