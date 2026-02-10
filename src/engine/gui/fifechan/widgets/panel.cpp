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
		m_docked(false) {
	}

	Panel::Panel(bool dockable):
		m_dockable(dockable),
		m_docked(false) {
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
				// save settings
				m_state.dimension = getDimension();
				//m_state.innerBorder = getInnerBorderSize();
				m_state.resizable = isResizable();
				m_state.movable = isMovable();
				setResizable(false);
				m_docked = true;
				getDockedArea()->adaptLayout(false);
				//resizeToContent(true);
				//expandContent(true);
			} else if (!docked && isDocked()) {
				m_docked = false;
				// restore settings
				//setDimension(m_state.dimension);
				setSize(m_state.dimension.width, m_state.dimension.height);
				//setInnerBorderSize(m_state.innerBorder);
				// center it
				Rectangle childrenRec = getParent()->getChildrenArea();
				int32_t x = childrenRec.x + childrenRec.width / 2 - getWidth() / 2;
				int32_t y = childrenRec.y + childrenRec.height / 2 - getHeight() / 2;
				setPosition(x, y);
				setResizable(m_state.resizable);
				setMovable(m_state.movable);
			}
		}
	}

	bool Panel::isDocked() const {
		return m_docked;
	}

	DockArea* Panel::getDockedArea() {
		DockArea* dockedArea = NULL;
		if (isDocked()) {
			dockedArea = dynamic_cast<DockArea*>(getParent());
		}
		return dockedArea;
	}

	DockArea* Panel::findDockArea() {
		DockArea* dockArea = NULL;
		if (!isDocked()) {
			if (getParent()) {
				DockArea* newDockArea = NULL;
				std::list<Widget*> widgets = getParent()->getWidgetsIn(getDimension(), this);
				if (widgets.size() > 0) {
					std::list<Widget*>::iterator it = widgets.begin();
					for (; it != widgets.end(); ++it) {
						// check if the Widget is a DockArea
						DockArea* tmp = dynamic_cast<DockArea*>(*it);
						if (!tmp) {
							// check if the Widget contains a DockArea
							std::list<Widget*> deepWidgets = (*it)->getWidgetsIn((*it)->getChildrenArea());
							if (deepWidgets.size() > 0) {
								Rectangle dim = getDimension();
								getAbsolutePosition(dim.x, dim.y);
								std::list<Widget*>::iterator dit = deepWidgets.begin();
								for (; dit != deepWidgets.end(); ++dit) {
									tmp = dynamic_cast<DockArea*>(*dit);
									if (tmp) {
										Rectangle tdim = (*dit)->getDimension();
										(*dit)->getAbsolutePosition(tdim.x, tdim.y);
										if (dim.isIntersecting(tdim)) {
											break;
										}
										tmp = NULL;
									}
								}
							}
							if (!tmp) {
								continue;
							}
						}
						newDockArea = tmp;
						break;
					}
				}
				dockArea = newDockArea;
			}
		}
		return dockArea;
	}

	void Panel::resizeToContent(bool recursiv) {
		if (!m_docked) {
			ResizableWindow::resizeToContent(recursiv);
		} else {
			Window::resizeToContent(recursiv);
		}
	}

	void Panel::expandContent(bool recursiv) {
		ResizableWindow::resizeToContent(recursiv);
	}

	void Panel::mouseEntered(MouseEvent& mouseEvent) {
		ResizableWindow::mouseEntered(mouseEvent);
	}

	void Panel::mouseExited(MouseEvent& mouseEvent) {
		ResizableWindow::mouseExited(mouseEvent);
	}

	void Panel::mousePressed(MouseEvent& mouseEvent) {
		if (!m_docked) {
			ResizableWindow::mousePressed(mouseEvent);
        } else {
			int32_t height = getBorderSize() + getPaddingTop() + getTitleBarHeight();
            mDragOffsetX = mouseEvent.getX();
            mDragOffsetY = mouseEvent.getY();

            mMoved = mouseEvent.getY() <= height;
        }
	}

	void Panel::mouseReleased(MouseEvent& mouseEvent) {
		if (mouseEvent.getButton() == MouseEvent::Left) {
			// move it inside the dock area
			if (m_docked && isMovable() && mMoved) {
				getDockedArea()->repositionWidget(this);
				getDockedArea()->adaptLayout(false);
			}
		} else if (mouseEvent.getButton() == MouseEvent::Right) {
			if (getDockedArea()) {
				restoreCursor();
			}
		}
		ResizableWindow::mouseReleased(mouseEvent);
	}

	void Panel::mouseMoved(MouseEvent& mouseEvent) {
		ResizableWindow::mouseMoved(mouseEvent);
	}

	void Panel::mouseDragged(MouseEvent& mouseEvent) {
		if (m_docked) {
			if (isMovable() && mMoved) {
				Rectangle rec = getDockedArea()->getChildrenArea();
				int32_t x = mouseEvent.getX() - mDragOffsetX + getX();
				int32_t y = mouseEvent.getY() - mDragOffsetY + getY();
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
				getDockedArea()->repositionWidget(this);
				// move to top on dragged instead of pressed
				getDockedArea()->moveToTop(this);
				// restore "old" position, layouting can change it
				setPosition(x, y);
			}

			mouseEvent.consume();
		} else {
			ResizableWindow::mouseDragged(mouseEvent);
		}
	}
}
