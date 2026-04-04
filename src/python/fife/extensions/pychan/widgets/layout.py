# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

AlignTop, AlignBottom, AlignLeft, AlignRight, AlignCenter = list(range(5))


def isLayouted(widget):
    return widget.real_widget.isLayouted()
