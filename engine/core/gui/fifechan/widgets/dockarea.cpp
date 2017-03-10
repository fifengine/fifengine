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
#include <algorithm>
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "gui/fifechan/base/gui_font.h"
#include "util/base/exception.h"

#include "dockarea.h"

namespace fcn {
	DockArea::DockArea():
		m_activeDockArea(true),
		m_topSide(false),
		m_rightSide(false),
		m_bottomSide(false),
		m_leftSide(false),
		m_highlighted(false),
		m_highlightColor(0xc80000)
	{
		setMovable(false);
		setMargin(0);
		setPadding(0);
		setTopResizable(false);
		setRightResizable(false);
		setBottomResizable(false);
		setLeftResizable(false);

	}

	DockArea::DockArea(bool active):
		m_activeDockArea(active),
		m_topSide(false),
		m_rightSide(false),
		m_bottomSide(false),
		m_leftSide(false),
		m_highlighted(false),
		m_highlightColor(0xc80000)
	{
		setMovable(false);
		setMargin(0);
		setPadding(0);
		setTopResizable(false);
		setRightResizable(false);
		setBottomResizable(false);
		setLeftResizable(false);

	}

	DockArea::~DockArea() {
	}

	void DockArea::setActiveDockArea(bool active) {
		m_activeDockArea = active;
	}

	bool DockArea::isActiveDockArea() const {
		return m_activeDockArea;
	}
	
	void DockArea::setTopSide(bool side) {
		m_topSide = side;
		setBottomResizable(m_topSide);
		if (side) {
			setLayout(Container::Horizontal);
		}
	}

	bool DockArea::isTopSide() const {
		return m_topSide;
	}

	void DockArea::setRightSide(bool side) {
		m_rightSide = side;
		setLeftResizable(m_rightSide);
		if (side) {
			setLayout(Container::Vertical);
		}
	}

	bool DockArea::isRightSide() const {
		return m_rightSide;
	}

	void DockArea::setBottomSide(bool side) {
		m_bottomSide = side;
		setTopResizable(m_bottomSide);
		if (side) {
			setLayout(Container::Horizontal);
		}
	}

	bool DockArea::isBottomSide() const {
		return m_bottomSide;
	}

	void DockArea::setLeftSide(bool side) {
		m_leftSide = side;
		setRightResizable(m_leftSide);
		if (side) {
			setLayout(Container::Vertical);
		}
	}

	bool DockArea::isLeftSide() const {
		return m_leftSide;
	}

	void DockArea::dockWidget(Widget* widget) {
		add(widget);
	}

	void DockArea::undockWidget(Widget* widget) {
		remove(widget);
	}

	void DockArea::setHighlighted(bool highlighted) {
		if (highlighted != m_highlighted) {
			if (highlighted) {
				m_savedColor = getBaseColor();
				setBaseColor(m_highlightColor);
			} else {
				setBaseColor(m_savedColor);
			}
			m_highlighted = highlighted;
		}
	}

	bool DockArea::isHighlighted() const {
		return m_highlighted;
	}

	void DockArea::setHighlightColor(const Color& color) {
		m_highlightColor = color;
	}

	const Color& DockArea::getHighlightColor() const {
		return m_highlightColor;
	}

	void DockArea::repositionWidget(Widget* widget) {
		Widget* placeBefore = NULL;
		Widget* placeAfter = NULL;

		Rectangle dim = widget->getDimension();
		widget->getAbsolutePosition(dim.x, dim.y);
		std::list<Widget*>::const_iterator currChild(mChildren.begin());
		std::list<Widget*>::const_iterator endChildren(mChildren.end());
		for(; currChild != endChildren; ++currChild) {
			if (!(*currChild)->isVisible() || (*currChild) == widget) {
				continue;
			}
			Rectangle childDim = (*currChild)->getDimension();
			(*currChild)->getAbsolutePosition(childDim.x, childDim.y);
			if (childDim.isIntersecting(dim)) {
				if (getLayout() == Container::Horizontal) {
					if (dim.x < childDim.x) {
						if (dim.x + dim.width <= childDim.x + childDim.width / 2) {
							placeBefore = *currChild;
						} else {
							placeAfter = *currChild;
						}
					} else if (dim.x >= childDim.x) {
						if (dim.x <= childDim.x + childDim.width / 2) {
							placeBefore = *currChild;
						} else {
							placeAfter = *currChild;
						}
					}
				}
				if (getLayout() == Container::Vertical) {
					if (dim.y < childDim.y) {
						if (dim.y + dim.height <= childDim.y + childDim.height / 2) {
							placeBefore = *currChild;
						} else {
							placeAfter = *currChild;
						}
					} else if (dim.y >= childDim.y) {
						if (dim.y <= childDim.y + childDim.height / 2) {
							placeBefore = *currChild;
						} else {
							placeAfter = *currChild;
						}
					}
				}
			}
		}
		if (placeBefore || placeAfter) {
			mChildren.remove(widget);
			if (placeBefore) {
				std::list<Widget*>::iterator it = std::find(mChildren.begin(), mChildren.end(), placeBefore);
				mChildren.insert(it, widget);
			} else {
				std::list<Widget*>::iterator it = std::find(mChildren.begin(), mChildren.end(), placeAfter);
				++it;
				mChildren.insert(it, widget);
			}
			adaptLayout(false);
		}
	}

	void DockArea::repositionDockAreas() {
		Widget* parent = getParent();
		if (parent) {
			DockArea* top = NULL;
			DockArea* right = NULL;
			DockArea* bottom = NULL;
			DockArea* left = NULL;
			std::list<Widget*> widgets = parent->getWidgetsIn(parent->getChildrenArea());
			std::list<Widget*>::iterator it = widgets.begin();
			for (; it != widgets.end(); ++it) {
				DockArea* tmp = dynamic_cast<DockArea*>(*it);
				if (!tmp) {
					continue;
				}
				tmp->keepInBounds();
				if (tmp->isTopSide()) {
					top = tmp;
				} else if (tmp->isRightSide()) {
					right = tmp;
				} else if (tmp->isBottomSide()) {
					bottom = tmp;
				} else if (tmp->isLeftSide()) {
					left = tmp;
				}
			}

			if (top) {
				if (right) {
					if (top->getY() + top->getHeight() >= right->getY() || top->getY() + top->getHeight() + 1 < right->getY()) {
						int32_t newY = top->getY() + top->getHeight() + 1;
						int32_t diff = newY - right->getY();
						right->setY(newY);
						right->setHeight(right->getHeight() + diff);
					}
				}
				if (left) {
					if (top->getY() + top->getHeight() >= left->getY() || top->getY() + top->getHeight() + 1 < left->getY()) {
						int32_t newY = top->getY() + top->getHeight() + 1;
						int32_t diff = newY - left->getY();
						left->setY(newY);
						left->setHeight(left->getHeight() + diff);
					}
				}
				if (bottom) {
					if (top->getY() + top->getHeight() >= bottom->getY()) {
						int32_t newY = top->getY() + top->getHeight() + 1;
						int32_t diff = newY - bottom->getY();
						bottom->setY(newY);
						bottom->setHeight(bottom->getHeight() + diff);
					}
				}
			}

			if (bottom) {
				if (right) {
					if (right->getY() + right->getHeight() >= bottom->getY() || right->getY() + right->getHeight() + 1 < bottom->getY()) {
						Size min = right->getMinSize();
						Size tmp;
						right->setMinSize(tmp);
						int32_t diff = bottom->getY() - (right->getY() + right->getHeight() + 1);
						right->setHeight(right->getHeight() + diff);
						right->setMinSize(min);
					}
				}
				if (left) {
					if (left->getY() + left->getHeight() >= bottom->getY() || left->getY() + left->getHeight() + 1 < bottom->getY()) {
						Size min = left->getMinSize();
						Size tmp;
						left->setMinSize(tmp);
						int32_t diff = bottom->getY() - (left->getY() + left->getHeight() + 1);
						left->setHeight(left->getHeight() + diff);
						left->setMinSize(min);
					}
				}
			}
		}
	}

	void DockArea::keepInBounds() {
		Widget* parent = getParent();
		if (!parent) {
			return;
		}
		Rectangle childArea = parent->getChildrenArea();
		Rectangle childDim = getDimension();
		if (childDim.x < childArea.x) {
			setX(childArea.x);
		}
		if (childDim.y < childArea.y) {
			setY(childArea.y);
		}
		
		if (m_topSide) {
			if ((childDim.y + childDim.height) > childArea.height) {
				setHeight(childArea.height - childDim.y);
			}
		} else if (m_rightSide) {
			if ((childDim.x + childDim.width) > childArea.width) {
				if (childDim.width > childArea.width) {
					setX(childArea.x);
					setWidth(childArea.width);
				} else {
					setX(childArea.width - childDim.width);
				}
			}
		} else if (m_bottomSide) {
			if ((childDim.y + childDim.height) > childArea.height) {
				if (childDim.height > childArea.height) {
					setY(childArea.y);
					setHeight(childArea.height);
				} else {
					setY(childArea.height - childDim.height);
				}
			}
		} else if (m_leftSide) {
			if ((childDim.x + childDim.width) > childArea.width) {
				setWidth(childArea.width - childDim.x);
			}
		}
	}

	void DockArea::add(Widget* widget) {
		ResizableWindow::add(widget);
		int32_t x = widget->getX();
		int32_t y = widget->getY();
		adaptLayout(true);
		widget->setPosition(x, y);
		repositionWidget(widget);
		requestMoveToTop();
	}

	void DockArea::remove(Widget* widget) {
		ResizableWindow::remove(widget);
		adaptLayout(false);
	}

	void DockArea::resizeToContent(bool recursiv) {
		Rectangle oldDimension = getDimension();
		if (m_resizing) {
			ResizableWindow::resizeToContent(recursiv);
		} else {
			Window::resizeToContent(recursiv);
		}
		if (isRightSide()) {
			int32_t wDiff = oldDimension.width - getWidth();
			setX(oldDimension.x + wDiff);
		} else if (isBottomSide()) {
			int32_t hDiff = oldDimension.height - getHeight();
			setY(oldDimension.y + hDiff);
		}
		repositionDockAreas();
	}

	void DockArea::expandContent(bool recursiv) {
		Rectangle oldDimension = getDimension();
		if (m_resizing) {
			ResizableWindow::expandContent(recursiv);
		} else {
			Window::expandContent(recursiv);
		}
		if (isRightSide()) {
			int32_t wDiff = oldDimension.width - getWidth();
			setX(oldDimension.x + wDiff);
		} else if (isBottomSide()) {
			int32_t hDiff = oldDimension.height - getHeight();
			setY(oldDimension.y + hDiff);
		}
		repositionDockAreas();
	}

	void DockArea::mouseEntered(MouseEvent& mouseEvent) {
		if (!m_highlighted) {
			ResizableWindow::mouseEntered(mouseEvent);
		}
	}

	void DockArea::mouseExited(MouseEvent& mouseEvent) {
		if (!m_highlighted) {
			ResizableWindow::mouseExited(mouseEvent);
		}
	}

	void DockArea::mousePressed(MouseEvent& mouseEvent) {
		if (!m_highlighted) {
			ResizableWindow::mousePressed(mouseEvent);
		}
	}

	void DockArea::mouseReleased(MouseEvent& mouseEvent) {
		if (!m_highlighted) {
			ResizableWindow::mouseReleased(mouseEvent);
			if (m_resizing) {
				repositionDockAreas();
			}
		}
	}

	void DockArea::mouseMoved(MouseEvent& mouseEvent) {
		if (!m_highlighted) {
			ResizableWindow::mouseMoved(mouseEvent);
		}
	}
	
	void DockArea::mouseDragged(MouseEvent& mouseEvent) {
		if (!m_highlighted) {
			ResizableWindow::mouseDragged(mouseEvent);
			if (m_resizing) {
				repositionDockAreas();
			}
		}
	}
}
