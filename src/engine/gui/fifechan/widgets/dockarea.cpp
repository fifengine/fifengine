// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "dockarea.h"

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <list>

// 3rd party library includes

// FIFE includes
#include "gui/fifechan/base/gui_font.h"
#include "util/base/exception.h"

namespace fcn
{
    DockArea::DockArea() :
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

    DockArea::DockArea(bool active) :
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

    DockArea::~DockArea() = default;

    void DockArea::setActiveDockArea(bool active)
    {
        m_activeDockArea = active;
    }

    bool DockArea::isActiveDockArea() const
    {
        return m_activeDockArea;
    }

    void DockArea::setTopSide(bool side)
    {
        m_topSide = side;
        setBottomResizable(m_topSide);
        if (side) {
            setLayout(Container::LayoutPolicy::Horizontal);
        }
    }

    bool DockArea::isTopSide() const
    {
        return m_topSide;
    }

    void DockArea::setRightSide(bool side)
    {
        m_rightSide = side;
        setLeftResizable(m_rightSide);
        if (side) {
            setLayout(Container::LayoutPolicy::Vertical);
        }
    }

    bool DockArea::isRightSide() const
    {
        return m_rightSide;
    }

    void DockArea::setBottomSide(bool side)
    {
        m_bottomSide = side;
        setTopResizable(m_bottomSide);
        if (side) {
            setLayout(Container::LayoutPolicy::Horizontal);
        }
    }

    bool DockArea::isBottomSide() const
    {
        return m_bottomSide;
    }

    void DockArea::setLeftSide(bool side)
    {
        m_leftSide = side;
        setRightResizable(m_leftSide);
        if (side) {
            setLayout(Container::LayoutPolicy::Vertical);
        }
    }

    bool DockArea::isLeftSide() const
    {
        return m_leftSide;
    }

    void DockArea::dockWidget(Widget* widget)
    {
        add(widget);
    }

    void DockArea::undockWidget(Widget* widget)
    {
        remove(widget);
    }

    void DockArea::setHighlighted(bool highlighted)
    {
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

    bool DockArea::isHighlighted() const
    {
        return m_highlighted;
    }

    void DockArea::setHighlightColor(Color const & color)
    {
        m_highlightColor = color;
    }

    Color const & DockArea::getHighlightColor() const
    {
        return m_highlightColor;
    }

    void DockArea::repositionWidget(Widget* widget)
    {
        Widget* placeBefore = nullptr;
        Widget* placeAfter  = nullptr;

        Rectangle dim = widget->getDimension();
        widget->getAbsolutePosition(dim.x, dim.y);
        std::list<Widget*>::const_iterator currChild(mChildren.begin());
        std::list<Widget*>::const_iterator const endChildren(mChildren.end());
        for (; currChild != endChildren; ++currChild) {
            if (!(*currChild)->isVisible() || (*currChild) == widget) {
                continue;
            }
            Rectangle childDim = (*currChild)->getDimension();
            (*currChild)->getAbsolutePosition(childDim.x, childDim.y);
            if (childDim.isIntersecting(dim)) {
                if (getLayout() == Container::LayoutPolicy::Horizontal) {
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
                if (getLayout() == Container::LayoutPolicy::Vertical) {
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
        if ((placeBefore != nullptr) || (placeAfter != nullptr)) {
            mChildren.remove(widget);
            if (placeBefore != nullptr) {
                auto it = std::ranges::find(mChildren, placeBefore);
                mChildren.insert(it, widget);
            } else {
                auto it = std::ranges::find(mChildren, placeAfter);
                ++it;
                mChildren.insert(it, widget);
            }
            adaptLayout(false);
        }
    }

    void DockArea::repositionDockAreas()
    {
        Widget* parent = getParent();
        if (parent != nullptr) {
            DockArea* top              = nullptr;
            DockArea* right            = nullptr;
            DockArea* bottom           = nullptr;
            DockArea* left             = nullptr;
            std::list<Widget*> widgets = parent->getWidgetsIn(parent->getChildrenArea());
            auto it                    = widgets.begin();
            for (; it != widgets.end(); ++it) {
                auto* tmp = dynamic_cast<DockArea*>(*it);
                if (tmp == nullptr) {
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

            if (top != nullptr) {
                if (right != nullptr) {
                    if (top->getY() + top->getHeight() >= right->getY() ||
                        top->getY() + top->getHeight() + 1 < right->getY()) {
                        int32_t const newY = top->getY() + top->getHeight() + 1;
                        int32_t const diff = newY - right->getY();
                        right->setY(newY);
                        right->setHeight(right->getHeight() + diff);
                    }
                }
                if (left != nullptr) {
                    if (top->getY() + top->getHeight() >= left->getY() ||
                        top->getY() + top->getHeight() + 1 < left->getY()) {
                        int32_t const newY = top->getY() + top->getHeight() + 1;
                        int32_t const diff = newY - left->getY();
                        left->setY(newY);
                        left->setHeight(left->getHeight() + diff);
                    }
                }
                if (bottom != nullptr) {
                    if (top->getY() + top->getHeight() >= bottom->getY()) {
                        int32_t const newY = top->getY() + top->getHeight() + 1;
                        int32_t const diff = newY - bottom->getY();
                        bottom->setY(newY);
                        bottom->setHeight(bottom->getHeight() + diff);
                    }
                }
            }

            if (bottom != nullptr) {
                if (right != nullptr) {
                    if (right->getY() + right->getHeight() >= bottom->getY() ||
                        right->getY() + right->getHeight() + 1 < bottom->getY()) {
                        Size const min = right->getMinSize();
                        Size const tmp;
                        right->setMinSize(tmp);
                        int32_t const diff = bottom->getY() - (right->getY() + right->getHeight() + 1);
                        right->setHeight(right->getHeight() + diff);
                        right->setMinSize(min);
                    }
                }
                if (left != nullptr) {
                    if (left->getY() + left->getHeight() >= bottom->getY() ||
                        left->getY() + left->getHeight() + 1 < bottom->getY()) {
                        Size const min = left->getMinSize();
                        Size const tmp;
                        left->setMinSize(tmp);
                        int32_t const diff = bottom->getY() - (left->getY() + left->getHeight() + 1);
                        left->setHeight(left->getHeight() + diff);
                        left->setMinSize(min);
                    }
                }
            }
        }
    }

    void DockArea::keepInBounds()
    {
        Widget* parent = getParent();
        if (parent == nullptr) {
            return;
        }
        Rectangle const childArea = parent->getChildrenArea();
        Rectangle const childDim  = getDimension();
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

    void DockArea::add(Widget* widget)
    {
        ResizableWindow::add(widget);
        int32_t const x = widget->getX();
        int32_t const y = widget->getY();
        adaptLayout(true);
        widget->setPosition(x, y);
        repositionWidget(widget);
        requestMoveToTop();
    }

    void DockArea::remove(Widget* widget)
    {
        ResizableWindow::remove(widget);
        adaptLayout(false);
    }

    void DockArea::resizeToContent(bool recursiv)
    {
        Rectangle const oldDimension = getDimension();
        ResizableWindow::resizeToContent(recursiv);
        if (isRightSide()) {
            int32_t const wDiff = oldDimension.width - getWidth();
            setX(oldDimension.x + wDiff);
        } else if (isBottomSide()) {
            int32_t const hDiff = oldDimension.height - getHeight();
            setY(oldDimension.y + hDiff);
        }
        repositionDockAreas();
    }

    void DockArea::expandContent(bool recursiv)
    {
        Rectangle const oldDimension = getDimension();
        if (m_resizing) {
            ResizableWindow::expandContent(recursiv);
        } else {
            Window::expandContent(recursiv);
        }
        if (isRightSide()) {
            int32_t const wDiff = oldDimension.width - getWidth();
            setX(oldDimension.x + wDiff);
        } else if (isBottomSide()) {
            int32_t const hDiff = oldDimension.height - getHeight();
            setY(oldDimension.y + hDiff);
        }
        repositionDockAreas();
    }

    void DockArea::mouseEntered(MouseEvent& mouseEvent)
    {
        if (!m_highlighted) {
            ResizableWindow::mouseEntered(mouseEvent);
        }
    }

    void DockArea::mouseExited(MouseEvent& mouseEvent)
    {
        if (!m_highlighted) {
            ResizableWindow::mouseExited(mouseEvent);
        }
    }

    void DockArea::mousePressed(MouseEvent& mouseEvent)
    {
        if (!m_highlighted) {
            ResizableWindow::mousePressed(mouseEvent);
        }
    }

    void DockArea::mouseReleased(MouseEvent& mouseEvent)
    {
        if (!m_highlighted) {
            ResizableWindow::mouseReleased(mouseEvent);
            if (m_resizing) {
                repositionDockAreas();
            }
        }
    }

    void DockArea::mouseMoved(MouseEvent& mouseEvent)
    {
        if (!m_highlighted) {
            ResizableWindow::mouseMoved(mouseEvent);
        }
    }

    void DockArea::mouseDragged(MouseEvent& mouseEvent)
    {
        if (!m_highlighted) {
            ResizableWindow::mouseDragged(mouseEvent);
            if (m_resizing) {
                repositionDockAreas();
            }
        }
    }
} // namespace fcn
