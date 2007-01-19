/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/
 
#ifndef FIFEDIT_FPANE_H
#define FIFEDIT_FPANE_H

// Standard C++ library includes

// 3rd party library includes
#include <QContextMenuEvent>
#include <QDockWidget>
#include <QMainWindow>
#include <QMenu>
#include <QMouseEvent>
#include <QTimer>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFEDIT {
	/** \brief The FPane class provides an extension of the QDockWidget.
	  *
	  * The extended functionality is the ability to collapse (hide) the pane,
	  * expand (show) the pane and the auto-hide ability. Auto-hide will make the
	  * pane visible when the mouse is hovered over the tab if it is set to true.
	  * Included in this is the need to emit signals for the changing of these states.
	  */
	class FPane : public QDockWidget {
		Q_OBJECT

		// Properties for the Qt designer
		Q_PROPERTY(Qt::DockWidgetArea dockArea READ dockArea WRITE setDockArea DESIGNABLE true)
		Q_PROPERTY(bool collapsed READ isCollapsed WRITE setCollapsed DESIGNABLE true)
		Q_PROPERTY(bool autoCollapse READ autoCollapse WRITE setAutoCollapse DESIGNABLE true)
		Q_PROPERTY(int autoCollapseDelay READ autoCollapseDelay WRITE setAutoCollapseDelay DESIGNABLE true)
		
		public:
			/** \brief Constructor
			  *
			  * @see setAutoCollapse
			  *
			  * @param title Pane title
			  * @param parent Parent. If parent is 0, the new widget becomes a window. If parent is another widget, this widget becomes a child window inside parent. The new widget is deleted when its parent is deleted.
			  * @param flags Window flags. can be set to customize the frame of a window (i.e. parent must be 0).
			  */
			FPane(QString &title, QWidget *parent = 0, Qt::WFlags flags = 0);

			/** \brief Constructor
			  * @param parent Parent. If parent is 0, the new widget becomes a window. If parent is another widget, this widget becomes a child window inside parent. The new widget is deleted when its parent is deleted.
			  * @param flags Window flags. can be set to customize the frame of a window (i.e. parent must be 0).
			  */
			FPane(QWidget *parent = 0, Qt::WFlags flags = 0);

			/** \brief Is auto-collapse enabled?
			  *
			  * @return Returns true if auto-collapse
			  **/
			bool autoCollapse() const;
			
			/** \brief Is pane collapsed/hidden?
			  *
			  * @return Returns true if pane is collapsed.
			  */
			bool isCollapsed() const;

			/** \brief Return current dock area
			  * Note that this function only returns the dockarea set with setDockArea
			  * @return current dock area
			  */
			Qt::DockWidgetArea dockArea() const;

			/** \brief Return the amount of milliseconds before the pane is auto-collapsed
			 * @return Return the amount of milliseconds before the pane is auto-collapsed
			  */
			int autoCollapseDelay();

			/** \brief Set whether this pane is to be used in the Qt designer or not
			  * xssx
			  * This function is only supposed to be used by fpaneplugin.cpp.
			  * It disables some functions which cause problems when used in the designer.
			  * Among these are docking and auto-collapse.
			  * @param inDesigner if the pane is to be used in Qt designer, set to true
			  */
			void setInDesigner(bool inDesigner);
			
		public slots:
			/** \brief Collapse/hide the pane
			  */
			void collapse();
			
			/** \brief Expand the pane
			  */
			void expand();

			/** \brief Collapse or expand the pane
			  */
			void setCollapsed(bool collapse);

			/** \brief Enable/disable auto-collapse.
			  *
			  * If auto-collapse is enabled the pane will be minimized a second
			  * after the cursor leaves the pane.
			  *
			  * @see FMainWindow::collapsePanes
			  * @param enable Whether auto-collapse should be enabled or not.
			  */
			void setAutoCollapse(bool enable);

			/** \brief Set amount of milliseconds before the pane is auto-collapsed
			  * @param delay amount of millisecond before the pane is auto-collapsed
			  */
			void setAutoCollapseDelay(int delay);

			/** \brief Dock pane to specified dockarea
			  * @param are the dockarea the pane is to be docked to
			  */
			void setDockArea(Qt::DockWidgetArea area);

			/** \brief Dock pane to left dockarea
			  */
			void dockToLeft();

			/** \brief Dock pane to right dockarea
			 */
			void dockToRight();

			/** \brief Dock pane to top dockarea
			 */
			void dockToTop();

			/** \brief Dock pane to bottom dockarea
			 */
			void dockToBottom();
			
		protected:
			/** \brief Sets attributes and initializes timers. */
			void initialize();
			
			/** \brief Initializes the context menu */
			void initContextMenu();
			
			
			/** \brief Loops through parent widgets and returns Main window if found
			  * @return main window if found, NULL otherwise
			  */
			QMainWindow *getMainWindow();

			/** \brief Compares the mouse location to the area covered by the FPane.
			 *  @return true if the mouse is in the Widget, false otherwise
			 */
			bool mouseInWidget();

			/** \brief Event created when the mouse enters the widget.  Expands the pane if
			 *  auto collapse is enabled.
			 *  @param event event
			 */
			virtual void enterEvent(QEvent *event);

			/** \brief Event created when the mouse leaves the widget. Restarts the timer if
			 *  auto collapse is enabled.
			 *  @param event event
			 */
			virtual void leaveEvent(QEvent *event);

			/** \brief Displays the contextmenu
			 * @param event contextmenu event
			 */
			virtual void contextMenuEvent(QContextMenuEvent *event);

		signals:
			/** \brief This signal gets emitted when the fpane collapses
			  *
			  * @see collapse
			  * @see expand
			  * @see expanded
			  */
			void collapsed();
			
			/** \brief This signal gets emitted when the fpane is expanded
			  *
			  * @see expand
			  * @see collapse
			  * @see collapsed
			  */
			void restored();
			
			
		private:
			QTimer m_collapseTimer;
			bool m_autoCollapse;
			bool m_collapsed;

			// Milliseconds before pane collapses
			int m_autoCollapseDelay;
			
			QRect m_lastSize;
			
			QMenu* m_menu;
			QMenu* m_dockAreaMenu;
			
			Qt::DockWidgetArea m_dockArea;
			
			// Actions
			QAction* m_expandAction;
			QAction* m_collapseAction;
			QAction* m_autoCollapseAction;
			QAction* m_dockToLeftAction;
			QAction* m_dockToRightAction;
			QAction* m_dockToTopAction;
			QAction* m_dockToBottomAction;

			// Whether the widget is being used in the Qt Designer
			bool m_inDesigner;
	};
}
#endif
