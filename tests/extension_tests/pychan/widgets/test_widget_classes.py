# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestPychanWidgetsClasses:
    def test_widget_class(self):
        from fife.extensions.pychan.widgets.widget import Widget

        assert Widget is not None

    def test_container_class(self):
        from fife.extensions.pychan.widgets.containers import Container

        assert Container is not None

    def test_vbox_class(self):
        from fife.extensions.pychan.widgets.containers import VBox

        assert VBox is not None

    def test_hbox_class(self):
        from fife.extensions.pychan.widgets.containers import HBox

        assert HBox is not None

    def test_window_class(self):
        from fife.extensions.pychan.widgets.containers import Window

        assert Window is not None

    def test_label_class(self):
        from fife.extensions.pychan.widgets.label import Label

        assert Label is not None

    def test_button_class(self):
        from fife.extensions.pychan.widgets.buttons import Button

        assert Button is not None

    def test_checkbox_class(self):
        from fife.extensions.pychan.widgets.checkbox import CheckBox

        assert CheckBox is not None

    def test_textfield_class(self):
        from fife.extensions.pychan.widgets.textfield import TextField

        assert TextField is not None

    def test_slider_class(self):
        from fife.extensions.pychan.widgets.slider import Slider

        assert Slider is not None

    def test_scrollarea_class(self):
        from fife.extensions.pychan.widgets.scrollarea import ScrollArea

        assert ScrollArea is not None

    def test_listbox_class(self):
        from fife.extensions.pychan.widgets.listbox import ListBox

        assert ListBox is not None

    def test_dropdown_class(self):
        from fife.extensions.pychan.widgets.dropdown import DropDown

        assert DropDown is not None

    def test_icon_class(self):
        from fife.extensions.pychan.widgets.icon import Icon

        assert Icon is not None

    def test_spacer_class(self):
        from fife.extensions.pychan.widgets.spacer import Spacer

        assert Spacer is not None

    def test_textbox_class(self):
        from fife.extensions.pychan.widgets.textbox import TextBox

        assert TextBox is not None

    def test_password_field_class(self):
        from fife.extensions.pychan.widgets.passwordfield import PasswordField

        assert PasswordField is not None

    def test_radio_button_class(self):
        from fife.extensions.pychan.widgets.radiobutton import RadioButton

        assert RadioButton is not None

    def test_image_button_class(self):
        from fife.extensions.pychan.widgets.buttons import ImageButton

        assert ImageButton is not None

    def test_toggle_button_class(self):
        from fife.extensions.pychan.widgets.buttons import ToggleButton

        assert ToggleButton is not None

    def test_panel_class(self):
        from fife.extensions.pychan.widgets.panel import Panel

        assert Panel is not None

    def test_tab_class(self):
        from fife.extensions.pychan.widgets.tabbedarea import Tab

        assert Tab is not None

    def test_tabbed_area_class(self):
        from fife.extensions.pychan.widgets.tabbedarea import TabbedArea

        assert TabbedArea is not None

    def test_flow_container_class(self):
        from fife.extensions.pychan.widgets.flowcontainer import FlowContainer

        assert FlowContainer is not None

    def test_adjusting_container_class(self):
        from fife.extensions.pychan.widgets.adjustingcontainer import AdjustingContainer

        assert AdjustingContainer is not None

    def test_resizable_window_class(self):
        from fife.extensions.pychan.widgets.resizablewindow import ResizableWindow

        assert ResizableWindow is not None

    def test_dock_area_class(self):
        from fife.extensions.pychan.widgets.dockarea import DockArea

        assert DockArea is not None

    def test_bar_graph_class(self):
        from fife.extensions.pychan.widgets.bargraph import BarGraph

        assert BarGraph is not None

    def test_line_graph_class(self):
        from fife.extensions.pychan.widgets.linegraph import LineGraph

        assert LineGraph is not None

    def test_curve_graph_class(self):
        from fife.extensions.pychan.widgets.curvegraph import CurveGraph

        assert CurveGraph is not None

    def test_pie_graph_class(self):
        from fife.extensions.pychan.widgets.piegraph import PieGraph

        assert PieGraph is not None

    def test_point_graph_class(self):
        from fife.extensions.pychan.widgets.pointgraph import PointGraph

        assert PointGraph is not None

    def test_percentage_bar_class(self):
        from fife.extensions.pychan.widgets.percentagebar import PercentageBar

        assert PercentageBar is not None

    def test_icon_progress_bar_class(self):
        from fife.extensions.pychan.widgets.iconprogressbar import IconProgressBar

        assert IconProgressBar is not None

    def test_image_progress_bar_class(self):
        from fife.extensions.pychan.widgets.imageprogressbar import ImageProgressBar

        assert ImageProgressBar is not None

    def test_animation_icon_class(self):
        from fife.extensions.pychan.widgets.animationicon import AnimationIcon

        assert AnimationIcon is not None
