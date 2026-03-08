// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GUI_WIDGETS_PANEL_H
#define FIFE_GUI_WIDGETS_PANEL_H

// Standard C++ library includes

// 3rd party library includes
#include <fifechan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "resizablewindow.h"

namespace fcn
{
    class DockArea;
    class Panel : public ResizableWindow
    {
    public:
        Panel();
        Panel(bool dockable);

        virtual ~Panel();

        /** Sets if the widget is dockable.
         * @param dockable True if the widget should be dockable, otherwise false.
         */
        void setDockable(bool dockable);

        /** Gets if the widget is dockable.
         * @return True if the widget should be dockable, otherwise false.
         */
        bool isDockable() const;

        /** Sets if the widget is docked.
         * @param docked True if the widget is docked, otherwise false.
         */
        void setDocked(bool docked);

        /** Gets if the widget is docked.
         * @return True if the widget is docked, otherwise false.
         */
        bool isDocked() const;

        virtual DockArea* findDockArea();

        // Inherited from ResizableWindow

        virtual void resizeToContent(bool recursiv = true);
        virtual void expandContent(bool recursiv = true);

        // Inherited from ResizableWindow / MouseListener

        virtual void mouseEntered(MouseEvent& mouseEvent);

        virtual void mouseExited(MouseEvent& mouseEvent);

        virtual void mousePressed(MouseEvent& mouseEvent);

        virtual void mouseReleased(MouseEvent& mouseEvent);

        virtual void mouseMoved(MouseEvent& mouseEvent);

        virtual void mouseDragged(MouseEvent& mouseEvent);

    protected:
        DockArea* getDockedArea();

        // is dockable
        bool m_dockable;
        // is docked
        bool m_docked;

        struct SavedState
        {
            Rectangle dimension;
            uint32_t innerBorder;
            bool resizable;
            bool movable;
        };
        SavedState m_state;
    };
} // namespace fcn

#endif