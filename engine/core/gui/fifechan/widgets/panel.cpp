/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE::                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

// Standard C++ library includes
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "gui/fifechan/base/gui_font.h"
#include "gui/fifechan/fifechanmanager.h"
#include "util/base/exception.h"

#include "dockarea.h"
#include "panel.h"

namespace fcn {
	Panel::Panel():
		m_dockable(true),
		m_docked(false),
		m_expanded(true),
		m_foundDockArea(NULL),
		m_dockedArea(NULL),
		m_origParent(NULL) {
	}

	Panel::Panel(bool dockable):
		m_dockable(dockable),
		m_docked(false),
		m_expanded(true),
		m_foundDockArea(NULL),
		m_dockedArea(NULL),
		m_origParent(NULL) {
	}

	Panel::~Panel() {
	}
	

	void Panel::setDockable(bool dockable) {
		m_dockable = dockable;
	}

	bool Panel::isDockable() const {
		return m_dockable;
	}

	void Panel::setDocked(bool docked) {
		if (isDockable()) {
			if (docked && !isDocked()) {
				// remove from Parent and save settings
				//m_state.dimension = getDimension();
				//m_state.innerBorder = getInnerBorderSize();
				m_state.resizable = isResizable();
				m_state.movable = isMovable();
				setResizable(false);
				m_origParent = static_cast<Container*>(getParent());
				m_origParent->remove(this);
				// add to DockArea
				m_foundDockArea->dockWidget(this);
				m_foundDockArea->setHighlighted(false);
				m_docked = true;
				m_dockedArea = m_foundDockArea;
				m_foundDockArea = NULL;
				m_docked = docked;
			} else if (!docked && isDocked()) {
				// remove from DockArea
				m_dockedArea->undockWidget(this);
				m_docked = false;
				m_dockedArea = NULL;
				// add to old Parent and restore settings
				m_origParent->add(this);
				m_origParent = NULL;
				//setDimension(m_state.dimension);
				//setInnerBorderSize(m_state.innerBorder);
				Rectangle childrenRec = getParent()->getChildrenArea();
				int32_t x = childrenRec.x + childrenRec.width / 2 - getWidth() / 2;
				int32_t y = childrenRec.y + childrenRec.height / 2 - getHeight() / 2;
				setPosition(x, y);
				setResizable(m_state.resizable);
				setMovable(m_state.movable);
				m_foundDockArea = NULL;
			}
		}
	}

	bool Panel::isDocked() const {
		return m_docked;
	}

	void Panel::expand(bool expand) {
		if (m_expanded != expand) {
			if (m_expanded) {
				// save old size
				m_state.dimension = getDimension();
				m_state.innerBorder = getInnerBorderSize();
				// shrink to titlebar height
				setHeight(2*getBorderSize() + getTitleBarHeight());
				setInnerBorderSize(0);
				m_expanded = false;
			} else {
				// expand to old height
				//setSize(m_state.dimension.width, m_state.dimension.height);
				setHeight(m_state.dimension.height);
				setInnerBorderSize(m_state.innerBorder);
				m_expanded = true;
			}
			if (m_docked) {
				m_dockedArea->adaptLayout(false);
			}
		}
	}

	void Panel::changeLayout() {
		// change Layout if possible
		if (getLayout() == Container::Vertical) {
			setLayout(Container::Horizontal);
			Window::resizeToContent(true);
			Window::expandContent(true);
		} else if (getLayout() == Container::Horizontal) {
			setLayout(Container::Vertical);
			//adaptLayout();
			Window::resizeToContent(true);
			Window::expandContent(true);
		}
	}

	void Panel::resetPrepared() {
		m_preDock = false;
		m_preUndock = false;
		m_preExpand = false;
		m_preLayout = false;
	}

	void Panel::findDockArea() {
		if (!isDocked() && isDockable()) {
			if (getParent()) {
				DockArea* newDockArea = NULL;
				std::list<Widget*> widgets = getParent()->getWidgetsIn(getDimension(), this);
				if (widgets.size() > 0) {
					std::list<Widget*>::iterator it = widgets.begin();
					for (; it != widgets.end(); ++it) {
						DockArea* tmp = dynamic_cast<DockArea*>(*it);
						if (!tmp) {
							continue;
						}
						newDockArea = tmp;
						break;
					}
				}
				if (newDockArea != m_foundDockArea) {
					if (m_foundDockArea) {
						m_foundDockArea->setHighlighted(false);
					}
					m_foundDockArea = newDockArea;
					if (m_foundDockArea) {
						m_foundDockArea->setHighlighted(true);
					}
				}
			}
		}
	}

	void Panel::resizeToContent(bool recursiv) {
		if (!m_docked) {
			ResizableWindow::resizeToContent(recursiv);
		} else {
			Window::resizeToContent(recursiv);
		}
		if (!m_expanded) {
			// shrink to titlebar height
			setHeight(2*getBorderSize() + getTitleBarHeight());
			setInnerBorderSize(0);
		}
	}

	void Panel::mouseEntered(MouseEvent& mouseEvent) {
		ResizableWindow::mouseEntered(mouseEvent);
	}

	void Panel::mouseExited(MouseEvent& mouseEvent) {
		resetPrepared();
		ResizableWindow::mouseExited(mouseEvent);
	}

	void Panel::mousePressed(MouseEvent& mouseEvent) {
		resetPrepared();
		//int height = getBorderSize() + getPaddingTop() + getTitleBarHeight();
		int32_t height = getBorderSize() + getTitleBarHeight();
		bool barPressed = mouseEvent.getY() <= height && mouseEvent.getY() > getResizableBorderDistance();
		if (mouseEvent.getButton() == MouseEvent::Left && barPressed) {
			// prepare move or expand
			m_preExpand= true;
		} else if (mouseEvent.getButton() == MouseEvent::Right && barPressed) {
			if (m_docked) {
				// prepare undock
				m_preUndock = true;
			} else {
				// prepare change layout
				m_preLayout = true;
			}
		}

		if (!m_docked) {
			ResizableWindow::mousePressed(mouseEvent);
        } else {
            mDragOffsetX = mouseEvent.getX();
            mDragOffsetY = mouseEvent.getY();

            int height = getBorderSize() + getPaddingTop() + getTitleBarHeight();
            mMoved = mouseEvent.getY() <= height;
        }
	}

	void Panel::mouseReleased(MouseEvent& mouseEvent) {
		if (mouseEvent.getButton() == MouseEvent::Left) {
			// expand or shrink it to titlebar
			if (m_preExpand) {
				expand(!m_expanded);
				m_preExpand = false;
			// dock it to the founded dock area
			} else if (!isDocked() && m_foundDockArea) {
				setDocked(true);
			// move it inside the dock area
			} else if (m_docked && isMovable() && mMoved) {
				m_dockedArea->repositionWidget(this);
				m_dockedArea->adaptLayout(false);
			}
		} else if (mouseEvent.getButton() == MouseEvent::Right) {
			// remove it from the dock area and add it back to the old parent
			if (m_dockedArea) {
				setDocked(false);
				// additional we should change the position, depend on the dockarea typ
				// and mouse event position
			// change layout
			} else if (m_preLayout) {
				changeLayout();
				m_preLayout = false;
			}
		}
		ResizableWindow::mouseReleased(mouseEvent);
	}

	void Panel::mouseMoved(MouseEvent& mouseEvent) {
		ResizableWindow::mouseMoved(mouseEvent);
	}

	void Panel::mouseDragged(MouseEvent& mouseEvent) {
		findDockArea();
		m_preDock =  m_foundDockArea != NULL;
		m_preExpand = false;
		m_preLayout = false;
		if (m_docked) {
			if (isMovable() && mMoved) {
				Rectangle rec = m_dockedArea->getChildrenArea();
				int x = mouseEvent.getX() - mDragOffsetX + getX();
				int y = mouseEvent.getY() - mDragOffsetY + getY();
				if (x < 0) {
					x = 0;
				} else if (x + getWidth() > rec.width) {
					x = rec.width - getWidth();
				}
				if (y < 0) {
					y = 0;
				} else if (y + getHeight() > rec.height) {
					y = rec.height - getHeight();
				}
				setPosition(x, y);
				m_dockedArea->repositionWidget(this);
				// move to top on dragged instead of pressed
				m_dockedArea->moveToTop(this);
				// restore "old" position, layouting can change it
				setPosition(x, y);
			}

			mouseEvent.consume();
		} else {
			ResizableWindow::mouseDragged(mouseEvent);
		}
	}

	void Panel::focusLost(const Event& event) {
		resetPrepared();
	}
}
