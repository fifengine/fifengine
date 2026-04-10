# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestBarGraph:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.bargraph import BarGraph

        assert BarGraph is not None


class TestCurveGraph:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.curvegraph import CurveGraph

        assert CurveGraph is not None


class TestLineGraph:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.linegraph import LineGraph

        assert LineGraph is not None


class TestPieGraph:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.piegraph import PieGraph

        assert PieGraph is not None


class TestPointGraph:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.pointgraph import PointGraph

        assert PointGraph is not None


class TestPercentageBar:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.percentagebar import PercentageBar

        assert PercentageBar is not None


class TestIconProgressBar:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.iconprogressbar import IconProgressBar

        assert IconProgressBar is not None


class TestImageProgressBar:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.imageprogressbar import ImageProgressBar

        assert ImageProgressBar is not None


class TestAnimationIcon:
    def test_class_exists(self):
        from fife.extensions.pychan.widgets.animationicon import AnimationIcon

        assert AnimationIcon is not None
