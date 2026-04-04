# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife.extensions import pychan
from pychan_demo import PyChanExample


class TabbedAreaExample(PyChanExample):
    def __init__(self):
        super().__init__("gui/tabbedarea.xml")

    def start(self):
        self.labelCount = 4
        self.widget = pychan.loadXML(self.xmlFile)
        self.tabarea = self.widget.findChild(name="tabarea")
        self.widget.mapEvents(
            {
                "closeButton": self.stop,
            }
        )
        # self.tabarea.layout = 'vertical'

        label = self.tabarea.findChild(name="label1")
        label.tab.mapEvents(
            {
                "close1": self.closeTab,
            }
        )
        label = self.tabarea.findChild(name="label2")
        label.tab.mapEvents(
            {
                "close2": self.closeTab,
            }
        )
        label = self.tabarea.findChild(name="label3")
        label.tab.mapEvents(
            {
                "close3": self.closeTab,
            }
        )
        label = self.tabarea.findChild(name="label4")
        label.tab.mapEvents(
            {
                "create": self.createTab,
            }
        )
        self.widget.show()

    def closeTab(self):
        widget = self.tabarea.select_tab
        self.tabarea.removeChild(widget)
        self.tabarea.resizeToContent()

    def createTab(self):
        tmp = self.tabarea.select_tab
        self.tabarea.removeChild(tmp)

        widget = pychan.widgets.Label()
        number = str(self.labelCount)
        widget.text = str("The " + number + " new Tab.")
        widget.tab = pychan.widgets.Tab()
        content = pychan.widgets.Label()
        content.text = str("Tab " + number)
        widget.tab.addChild(content)
        content = pychan.widgets.Button(text="X", name="close" + number, border_size=0)
        widget.tab.addChild(content)
        widget.tab.mapEvents(
            {
                "close" + number: self.closeTab,
            }
        )
        self.tabarea.addChild(widget)
        self.tabarea.addChild(tmp)
        self.labelCount += 1
        self.tabarea.select_tab = widget
        self.tabarea.adaptLayout()
