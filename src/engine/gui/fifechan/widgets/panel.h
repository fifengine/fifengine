// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GUI_WIDGETS_PANEL_H
#define FIFE_GUI_WIDGETS_PANEL_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
// 3rd party library includes
#include <fifechan.hpp>

// FIFE includes
#include "resizablewindow.h"

namespace fcn
{
    class DockArea;
    class /*FIFE_API*/ Panel : public ResizableWindow
    {
    public:
        Panel();
        explicit Panel(bool dockable);

        ~Panel() override;

        Panel(const Panel&)            = delete;
        Panel& operator=(const Panel&) = delete;

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

        void resizeToContent(bool recursiv = true) override;
        void expandContent(bool recursiv = true) override;

        // Inherited from ResizableWindow / MouseListener

        void mouseEntered(MouseEvent& mouseEvent) override;

        void mouseExited(MouseEvent& mouseEvent) override;

        void mousePressed(MouseEvent& mouseEvent) override;

        void mouseReleased(MouseEvent& mouseEvent) override;

        void mouseMoved(MouseEvent& mouseEvent) override;

        void mouseDragged(MouseEvent& mouseEvent) override;

    protected:
        DockArea* getDockedArea();

        // is dockable
        bool m_dockable;
        // is docked
        bool m_docked;

        struct FIFE_API SavedState
        {
            Rectangle dimension;
            uint32_t innerBorder{0};
            bool resizable{false};
            bool movable{false};
        };
        SavedState m_state;
    };
} // namespace fcn

#endif
