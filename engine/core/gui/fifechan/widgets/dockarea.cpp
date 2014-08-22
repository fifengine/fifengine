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
		repositionWidget(widget);
		Rectangle oldDimension = getDimension();
		adaptLayout(false);
		if (isRightSide()) {
			int32_t wDiff = oldDimension.width - getWidth();
			setX(oldDimension.x + wDiff);
		} else if (isBottomSide()) {
			int32_t hDiff = oldDimension.height - getHeight();
			setY(oldDimension.y + hDiff);
		}
		repositionDockAreas();
		requestMoveToTop();
	}

	void DockArea::undockWidget(Widget* widget) {
		remove(widget);
		Rectangle oldDimension = getDimension();
		adaptLayout(false);
		if (isRightSide()) {
			int32_t wDiff = oldDimension.width - getWidth();
			setX(oldDimension.x + wDiff);
		} else if (isBottomSide()) {
			int32_t hDiff = oldDimension.height - getHeight();
			setY(oldDimension.y + hDiff);
		}
		repositionDockAreas();
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
		//Widget* placeIn = NULL;

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

	void DockArea::resizeToContent(bool recursiv) {
		if (m_resizing) {
			ResizableWindow::resizeToContent(recursiv);
		} else {
			Window::resizeToContent(recursiv);
		}
	}

	void DockArea::expandContent(bool recursiv) {
		if (m_resizing) {
			ResizableWindow::expandContent(recursiv);
		} else {
			Window::expandContent(recursiv);
		}
	}

	void DockArea::mouseEntered(MouseEvent& mouseEvent) {
		ResizableWindow::mouseEntered(mouseEvent);
	}

	void DockArea::mouseExited(MouseEvent& mouseEvent) {
		ResizableWindow::mouseExited(mouseEvent);
	}

	void DockArea::mousePressed(MouseEvent& mouseEvent) {
		m_oldDimension = getDimension();
		ResizableWindow::mousePressed(mouseEvent);
	}

	void DockArea::mouseReleased(MouseEvent& mouseEvent) {
		if (m_resizing) {
			if (m_rightSide || m_bottomSide) {
				ResizableWindow::mouseReleased(mouseEvent);
				if (m_rightSide) {
					setX(m_oldDimension.x);
				} else {
					setY(m_oldDimension.y);
				}
				return;
			}
		}
		ResizableWindow::mouseReleased(mouseEvent);
	}

	void DockArea::mouseMoved(MouseEvent& mouseEvent) {
		ResizableWindow::mouseMoved(mouseEvent);
	}
	
	void DockArea::mouseDragged(MouseEvent& mouseEvent) {
		ResizableWindow::mouseDragged(mouseEvent);
	}

	void DockArea::focusLost(const Event& event) {
	}
}
