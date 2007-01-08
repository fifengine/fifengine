/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

// Standard C++ library includes

// 3rd party library includes
#include <QMessageBox>
#include <QPoint>
#include <QStyle>
#include <QTimer>

#include "fpane.h"


namespace FIFEDIT {
	FPane::FPane(QString &title, QWidget *parent, Qt::WFlags flags) : QDockWidget(title, parent, flags) {
		initialize();
	}
	
	FPane::FPane(QWidget *parent, Qt::WFlags flags) : QDockWidget(parent, flags) {
		initialize();
	}
	
	void FPane::initContextMenu() {

		//-- Actions --//
		m_collapseAction = new QAction(tr("Collapse"), this);
		m_collapseAction->setEnabled( !m_collapsed );
		connect(m_collapseAction, SIGNAL(triggered()), SLOT(collapse()));
        
		m_expandAction = new QAction(tr("Expand"), this);
		m_expandAction->setEnabled( m_collapsed );
		connect(m_expandAction, SIGNAL(triggered()), SLOT(expand()));
        
		m_autoCollapseAction = new QAction(tr("Auto-collapse"), this);
		m_autoCollapseAction->setCheckable(true);
		m_autoCollapseAction->setChecked(m_autoCollapse);
		connect(m_autoCollapseAction, SIGNAL(toggled(bool)), SLOT(setAutoCollapse(bool)));
		
		m_dockToLeftAction = new QAction(tr("Left"), this);
		connect(m_dockToLeftAction, SIGNAL(triggered(bool)), SLOT(dockToLeft()));

		m_dockToRightAction = new QAction(tr("Right"), this);
		connect(m_dockToRightAction, SIGNAL(triggered(bool)), SLOT(dockToRight()));

		m_dockToTopAction = new QAction(tr("Top"), this);
		connect(m_dockToTopAction, SIGNAL(triggered(bool)), SLOT(dockToTop()));

		m_dockToBottomAction = new QAction(tr("Bottom"), this);
		connect(m_dockToBottomAction, SIGNAL(triggered(bool)), SLOT(dockToBottom()));


		//-- Context menus --//
		m_dockAreaMenu = new QMenu(tr("Dock to ..."));
		m_dockAreaMenu->addAction(m_dockToLeftAction);
		m_dockAreaMenu->addAction(m_dockToRightAction);
		m_dockAreaMenu->addAction(m_dockToTopAction);
		m_dockAreaMenu->addAction(m_dockToBottomAction);
		
		m_menu = new QMenu("ContextMenu");
		m_menu->addAction(m_autoCollapseAction);
		m_menu->addAction(m_expandAction);
		m_menu->addAction(m_collapseAction);
		m_menu->addSeparator();
		m_menu->addMenu(m_dockAreaMenu);
	}
	
	void FPane::initialize() {
		setMinimumHeight(QStyle::PM_TitleBarHeight);
		m_collapsed = false;
		m_autoCollapse = false;
		m_autoCollapseDelay = 2000;
		m_dockArea = Qt::TopDockWidgetArea;
		m_inDesigner = false;
		
		// Context menu
		initContextMenu();

		// Mouse Events
		setMouseTracking(true);
		setEnabled(true);
		
		// Auto-collapse timer
		m_collapseTimer.setSingleShot(true);
		connect(&m_collapseTimer, SIGNAL(timeout()), this, SLOT(collapse()));

		setAutoCollapseDelay(m_autoCollapseDelay);
		setAutoCollapse(m_autoCollapse);

		setDockArea( m_dockArea );
	}

	void FPane::setAutoCollapseDelay(int delay) {
		m_autoCollapseDelay = delay;
		m_collapseTimer.setInterval(m_autoCollapseDelay);
	}

	int FPane::autoCollapseDelay() {
		return m_autoCollapseDelay;
	}

	bool FPane::autoCollapse() const {
		return m_autoCollapse;
	}

	void FPane::setAutoCollapse( bool enable ) {
		if ( m_inDesigner ) {
			return;
		}
		
		m_autoCollapse = enable;
		if (m_autoCollapse) {
			m_collapseTimer.start();
		} else {
			m_collapseTimer.stop();
		}
	}
	
	bool FPane::isCollapsed() const {
		return m_collapsed;
	}
	
	void FPane::collapse() {
		if (m_collapsed) {
			return;
		}
		if (mouseInWidget()) {
			m_collapseTimer.start();
			return;
		}

		if (widget()) {
			widget()->hide();
		}
		
		m_collapsed = true;
		m_lastSize = frameGeometry();
		setMaximumHeight(QStyle::PM_TitleBarHeight);
        
		m_collapseAction->setEnabled(false);
		m_expandAction->setEnabled(true);
		emit collapsed();
	}

	void FPane::setCollapsed( bool enable ) {
		if ( enable ) {
			collapse();
		} else {
			expand();
		}
	}
	
	void FPane::expand() {
		if (m_autoCollapse) {
			m_collapseTimer.start();
		}
		if (!m_collapsed) {
			return;
		}

		setMaximumHeight(5000);
		resize(m_lastSize.width(), m_lastSize.height());
		if (widget()) {
			widget()->show();
		}
		
		m_collapsed = false;
		m_collapseAction->setEnabled(true);
		m_expandAction->setEnabled(false);
		emit restored();
	}

	bool FPane::mouseInWidget(){
		QPoint mousePos = mapFromGlobal(QCursor::pos()) + frameGeometry().topLeft();
		if (frameGeometry().contains(mousePos)) {
			return true;
		}
		
		return false;
	}

	QMainWindow *FPane::getMainWindow() {
		for (QWidget *pWidget = this; ; pWidget = pWidget->parentWidget()) {
			if (!pWidget) return NULL;
			
			QMainWindow *mainWindow = qobject_cast<QMainWindow *>(pWidget);
			if (mainWindow) {
				return mainWindow;
			}
		}
		return NULL;
	}


	
	void FPane::setDockArea(const Qt::DockWidgetArea area) {
		if ( m_inDesigner ) {
			return;
		}
		
		m_dockArea = area;
		QMainWindow* mainwindow = getMainWindow();
		if (mainwindow) {
			mainwindow->addDockWidget(area, this);
		}
	}
	
	Qt::DockWidgetArea FPane::dockArea() const {
		return m_dockArea;
	}
	
	void FPane::dockToLeft() {
		setDockArea( Qt::LeftDockWidgetArea );
	}
	
	void FPane::dockToRight() {
		setDockArea( Qt::RightDockWidgetArea );
	}
	
	void FPane::dockToTop() {
		setDockArea( Qt::TopDockWidgetArea );
	}
		
	void FPane::dockToBottom() {
		setDockArea( Qt::BottomDockWidgetArea );
	}
	

	
	void FPane::contextMenuEvent(QContextMenuEvent *event) {
		event->accept();
		m_menu->exec(event->globalPos());
	}
	
	void FPane::enterEvent(QEvent *) {
		if( m_autoCollapse && m_collapsed ) {
			expand();
		}
	}

	void FPane::leaveEvent(QEvent *) {
		if (m_autoCollapse) {
			m_collapseTimer.start();
		}
	}

	
	void FPane::setInDesigner(bool enable) {
		m_inDesigner = enable;
	}
}
