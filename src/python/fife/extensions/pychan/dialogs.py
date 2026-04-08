# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""
Dialog utilities for creating common dialog boxes.

This module provides functions for displaying message boxes, yes/no dialogs,
selection dialogs, and exception handling dialogs.
"""

from io import StringIO

from fife.extensions import pychan
from fife.extensions.pychan import loadXML

from .internal import screen_height, screen_width

OK, YES, NO, CANCEL = True, True, False, None


def print_event(**kwargs):
    """
    Debug function to print event keyword arguments.

    Parameters
    ----------
    **kwargs : dict
        Keyword arguments containing event data.
    """
    print(kwargs)


class XMLDialog:
    """
    Base class for dialogs that load and execute XML-defined GUIs.

    Parameters
    ----------
    xml : str or file-like
        The XML definition of the dialog.
    ok_field : str, optional
        Name of the field to collect data from when OK is pressed.
    cancel_field : str, optional
        Name of the field to collect data from when Cancel is pressed.
    initial_data : dict, optional
        Initial data to distribute to widgets.
    data : dict, optional
        Data to distribute to widgets.
    """

    def __init__(self, xml, ok_field=None, cancel_field=None, initial_data={}, data={}):
        self.gui = loadXML(xml)
        self.ok_field = ok_field
        self.cancel_field = cancel_field
        self.initial_data = initial_data
        self.data = data
        self.max_size = None
        self.min_size = None

    # 		self.gui.capture(print_event,"mouseEntered")

    def execute(self):
        """
        Execute the dialog synchronously.

        Returns
        -------
        object
            The result of the dialog, typically True/False or collected data.
        """
        self.gui.distributeInitialData(self.initial_data)
        self.gui.distributeData(self.data)

        screen_w, screen_h = screen_width(), screen_height()
        if self.max_size is None:
            self.max_size = screen_w // 2, screen_h // 3
        if self.min_size is None:
            self.min_size = screen_w // 2, screen_h // 4
        self.gui.max_size = self.max_size
        self.gui.min_size = self.min_size

        resultMap = {}
        if self.gui.findChild(name="okButton"):
            resultMap["okButton"] = OK

        if self.gui.findChild(name="cancelButton"):
            resultMap["cancelButton"] = CANCEL

        if self.gui.findChild(name="yesButton"):
            resultMap["noButton"] = NO

        if self.gui.findChild(name="yesButton"):
            resultMap["yesButton"] = YES

        ok = self.gui.execute(resultMap)
        if ok:
            return self.getOkResult()
        return self.getCancelResult()

    def getOkResult(self):
        """
        Get the result when OK/Yes is selected.

        Returns
        -------
        object
            The collected data or True.
        """
        if self.ok_field:
            return self.gui.collectData(self.ok_field)
        return True

    def getCancelResult(self):
        """
        Get the result when Cancel/No is selected.

        Returns
        -------
        object
            The collected data or False.
        """
        if self.cancel_field:
            return self.gui.collectData(self.cancel_field)
        return False


MESSAGE_BOX_XML = """\
<Window name="window" title="Message">
<ScrollArea>
<Label wrap_text="1" text="$MESSAGE" name="message" vexpand="True"/>
</ScrollArea>
<HBox>
<Spacer/><Button min_size="50,0" name="okButton" text="OK"/>
</HBox>
</Window>
"""

YESNO_BOX_XML = """\
<Window name="window" title="Question">
<ScrollArea>
<Label wrap_text="1" text="$MESSAGE" name="message" vexpand="True"/>
</ScrollArea>
<HBox>
<Spacer/>
<Button min_size="50,0" name="yesButton" text="Yes"/>
<Button min_size="50,0" name="noButton" text="No"/>
</HBox>
</Window>
"""

YESNOCANCEL_BOX_XML = """\
<Window name="window" title="Question">
<ScrollArea>
<Label wrap_text="1" text="$MESSAGE" name="message" vexpand="True"/>
</ScrollArea>
<HBox>
<Spacer/>
<Button min_width="50" name="yesButton" text="Yes"/>
<Button min_width="50" name="noButton" text="No"/>
<Button min_width="50" name="cancelButton" text="Cancel"/>
</HBox>
</Window>
"""

SELECT_BOX_XML = """\
<Window name="window" title="Select">
<Label wrap_text="1" text="$MESSAGE" name="message"/>
<ScrollArea>
<ListBox name="selection">
</ListBox>
</ScrollArea>
<HBox>
<Spacer/>
<Button min_width="50" name="okButton" text="Select"/>
<Button min_width="50" name="cancelButton" text="Cancel"/>
</HBox>
</Window>
"""

EXCEPTION_CATCHER_XML = """\
<Window name="window" title="An exception occurred - what now?">
  <VBox hexpand="True">
    <Label wrap_text="1" max_size="400,90000" text="$MESSAGE" name="message"/>
    <ScrollArea>
    <Label text="$MESSAGE" name="traceback"/>
    </ScrollArea>
  </VBox>
  <HBox>
    <Spacer/>
    <Button name="yesButton" text="Retry"/>
    <Button name="noButton" text="Ignore"/>
    <Button name="cancelButton" text="Reraise"/>
  </HBox>
</Window>
"""


def _make_text(message):
    if callable(message):
        message = message()
    if hasattr(message, "read"):
        message = message.read()
    return message


def message(message="", caption="Message"):
    """
    Display a simple message dialog with an OK button.

    Parameters
    ----------
    message : str or callable
        The message to display. Can be a string or a callable returning a string.
    caption : str
        The dialog window title.
    """
    text = _make_text(message)
    dialog = XMLDialog(StringIO(MESSAGE_BOX_XML))
    dialog.gui.findChild(name="message").max_width = screen_width() // 2 - 50
    dialog.gui.findChild(name="message").text = text
    dialog.gui.findChild(name="window").title = caption

    dialog.execute()


def yesNo(message="", caption="Message"):
    """
    Display a Yes/No dialog.

    Parameters
    ----------
    message : str or callable
        The message to display. Can be a string or a callable returning a string.
    caption : str
        The dialog window title.

    Returns
    -------
    bool
        True if Yes was selected, False if No was selected.
    """
    text = _make_text(message)
    dialog = XMLDialog(StringIO(YESNO_BOX_XML))
    dialog.gui.findChild(name="message").max_width = screen_width() // 2 - 50
    dialog.gui.findChild(name="message").text = text
    dialog.gui.findChild(name="window").title = caption

    return dialog.execute()


def yesNoCancel(message="", caption="Message"):
    """
    Display a Yes/No/Cancel dialog.

    Parameters
    ----------
    message : str or callable
        The message to display. Can be a string or a callable returning a string.
    caption : str
        The dialog window title.

    Returns
    -------
    bool or None
        True if Yes was selected, False if No was selected, None if Cancel was selected.
    """
    text = _make_text(message)
    dialog = XMLDialog(StringIO(YESNOCANCEL_BOX_XML))
    dialog.gui.findChild(name="message").max_width = screen_width() // 2 - 50
    dialog.gui.findChild(name="message").text = text
    dialog.gui.findChild(name="window").title = caption

    return dialog.execute()


def select(message="", options=[], caption="Message"):
    """
    Display a selection dialog with a list of options.

    Parameters
    ----------
    message : str or callable
        The message to display. Can be a string or a callable returning a string.
    options : list
        List of options to display in the selection box.
    caption : str
        The dialog window title.

    Returns
    -------
    object or None
        The selected item, or None if cancelled.
    """
    text = _make_text(message)
    dialog = XMLDialog(StringIO(SELECT_BOX_XML))
    dialog.size = screen_width() // 3, (2 * screen_height()) // 3
    dialog.gui.findChild(name="message").max_width = screen_width() // 2 - 50
    dialog.gui.findChild(name="message").text = text
    dialog.gui.findChild(name="window").title = caption

    listbox = dialog.gui.findChild(name="selection")
    listbox.items = options
    if dialog.execute():
        return listbox.selected_item
    return None


def trace(f):
    """
    Catch exceptions and display a dialog.

    Allows the user to retry, ignore, or reraise the exception.

    Parameters
    ----------
    f : callable
        The function to wrap.

    Returns
    -------
    callable
        The wrapped function.
    """
    import sys
    import traceback

    def new_f(*args, **kwargs):
        try:
            return pychan.tools.applyOnlySuitable(f, *args, **kwargs)

        except Exception as e:
            dialog = XMLDialog(StringIO(EXCEPTION_CATCHER_XML))
            dialog.gui.findChild(name="message").text = str(e)

            tb = traceback.format_exception(
                sys.exc_info()[0], sys.exc_info()[1], sys.exc_info()[2]
            )
            dialog.gui.findChild(name="traceback").text = "".join(tb)
            dialog.min_size = screen_width() // 2, (3 * screen_height()) // 4
            dialog.max_size = screen_width() // 2, (3 * screen_height()) // 4
            result = dialog.execute()
            if result == YES:
                return new_f(*args, **kwargs)
            elif result == NO:
                return
            raise

    return new_f
