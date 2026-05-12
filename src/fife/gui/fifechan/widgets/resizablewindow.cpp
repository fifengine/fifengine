// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "resizablewindow.h"

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <string>

// 3rd party library includes

// FIFE includes
#include "gui/fifechan/base/gui_font.h"
#include "gui/fifechan/fifechanmanager.h"
#include "gui/fifechan/widgets/resizablewindow.h"
#include "util/base/exception.h"

namespace fcn
{
    ResizableWindow::ResizableWindow() :

        m_cursor(FIFE::FifechanManager::instance()->getCursor()),
        m_borderDistance(5),
        m_resizable(true),
        m_resizableTop(true),
        m_resizableRight(true),
        m_resizableBottom(true),
        m_resizableLeft(true),
        m_resizing(false),
        m_resizeTop(false),
        m_resizeRight(false),
        m_resizeBottom(false),
        m_resizeLeft(false),
        m_shove(false)
    {

        addKeyListener(this);
        addFocusListener(this);
        initCursors();
    }

    ResizableWindow::ResizableWindow(bool resizable) :

        m_cursor(FIFE::FifechanManager::instance()->getCursor()),
        m_borderDistance(5),
        m_resizable(resizable),
        m_resizableTop(true),
        m_resizableRight(true),
        m_resizableBottom(true),
        m_resizableLeft(true),
        m_resizing(false),
        m_resizeTop(false),
        m_resizeRight(false),
        m_resizeBottom(false),
        m_resizeLeft(false),
        m_shove(false)
    {

        addKeyListener(this);
        addFocusListener(this);
        initCursors();
    }

    ResizableWindow::ResizableWindow(std::string const & caption, bool resizable) :

        m_cursor(FIFE::FifechanManager::instance()->getCursor()),
        m_borderDistance(5),
        m_resizable(resizable),
        m_resizableTop(true),
        m_resizableRight(true),
        m_resizableBottom(true),
        m_resizableLeft(true),
        m_resizing(false),
        m_resizeTop(false),
        m_resizeRight(false),
        m_resizeBottom(false),
        m_resizeLeft(false),
        m_shove(false)
    {

        setCaption(caption);
        addKeyListener(this);
        addFocusListener(this);
        initCursors();
    }

    ResizableWindow::~ResizableWindow() = default;

    void ResizableWindow::initCursors()
    {
        // defines default cursors
        // don't change the sequence!
        CursorState state;
        state.cursorId   = FIFE::NC_ARROW;
        state.cursorType = FIFE::CURSOR_NONE;
        m_saved          = state;
        state.cursorType = FIFE::CURSOR_NATIVE;
        // left
        state.cursorId = FIFE::NC_RESIZEWE;
        m_cursors.push_back(state);
        // right
        state.cursorId = FIFE::NC_RESIZEWE;
        m_cursors.push_back(state);
        // top
        state.cursorId = FIFE::NC_RESIZENS;
        m_cursors.push_back(state);
        // left and top
        state.cursorId = FIFE::NC_RESIZENWSE;
        m_cursors.push_back(state);
        // right and top
        state.cursorId = FIFE::NC_RESIZENESW;
        m_cursors.push_back(state);
        // bottom
        state.cursorId = FIFE::NC_RESIZENS;
        m_cursors.push_back(state);
        // left and bottom
        state.cursorId = FIFE::NC_RESIZENESW;
        m_cursors.push_back(state);
        // right and bottom
        state.cursorId = FIFE::NC_RESIZENWSE;
        m_cursors.push_back(state);
    }

    void ResizableWindow::restoreCursor()
    {
        FIFE::MouseCursorType const type = m_saved.cursorType;
        switch (type) {
        case FIFE::CURSOR_NATIVE:
            m_cursor->set(m_saved.cursorId);
            break;
        case FIFE::CURSOR_IMAGE:
            m_cursor->set(m_saved.cursorImage);
            break;
        case FIFE::CURSOR_ANIMATION:
            m_cursor->set(m_saved.cursorAnimation);
            break;
        case FIFE::CURSOR_NONE:
            return;
        default:
            return;
        }
    }

    void ResizableWindow::saveCursor()
    {
        FIFE::MouseCursorType const type = m_cursor->getType();
        switch (type) {
        case FIFE::CURSOR_NATIVE:
            m_saved.cursorType = type;
            m_saved.cursorId   = m_cursor->getId();
            m_saved.cursorImage.reset();
            m_saved.cursorAnimation.reset();
            break;
        case FIFE::CURSOR_IMAGE:
            m_saved.cursorType = type;
            m_saved.cursorId   = FIFE::NC_ARROW;
            ;
            m_saved.cursorImage = m_cursor->getImage();
            m_saved.cursorAnimation.reset();
            break;
        case FIFE::CURSOR_ANIMATION:
            m_saved.cursorType = type;
            m_saved.cursorId   = FIFE::NC_ARROW;
            ;
            m_saved.cursorImage.reset();
            m_saved.cursorAnimation = m_cursor->getAnimation();
            break;
        case FIFE::CURSOR_NONE:
            m_saved.cursorType = FIFE::CURSOR_NONE;
            break;
        default:
            m_saved.cursorType = FIFE::CURSOR_NONE;
        }
    }

    void ResizableWindow::setResizableBorderDistance(int32_t border)
    {
        m_borderDistance = border;
    }

    int32_t ResizableWindow::getResizableBorderDistance() const
    {
        return m_borderDistance;
    }

    void ResizableWindow::setResizable(bool resizable)
    {
        m_resizable = resizable;
    }

    bool ResizableWindow::isResizable() const
    {
        return m_resizable;
    }

    void ResizableWindow::setTopResizable(bool resizable)
    {
        m_resizableTop = resizable;
    }

    bool ResizableWindow::isTopResizable() const
    {
        return m_resizableTop;
    }

    void ResizableWindow::setRightResizable(bool resizable)
    {
        m_resizableRight = resizable;
    }

    bool ResizableWindow::isRightResizable() const
    {
        return m_resizableRight;
    }

    void ResizableWindow::setBottomResizable(bool resizable)
    {
        m_resizableBottom = resizable;
    }

    bool ResizableWindow::isBottomResizable() const
    {
        return m_resizableBottom;
    }

    void ResizableWindow::setLeftResizable(bool resizable)
    {
        m_resizableLeft = resizable;
    }

    bool ResizableWindow::isLeftResizable() const
    {
        return m_resizableLeft;
    }

    void ResizableWindow::setShove(bool shove)
    {
        m_shove = shove;
    }

    bool ResizableWindow::getShove() const
    {
        return m_shove;
    }

    void ResizableWindow::set(CursorDirections direction, uint32_t cursor_id)
    {
        CursorState& state = m_cursors[direction];
        state.cursorType   = FIFE::CURSOR_NATIVE;
        state.cursorId     = cursor_id;
        state.cursorImage.reset();
        state.cursorAnimation.reset();
    }

    void ResizableWindow::set(CursorDirections direction, FIFE::ImagePtr const & image)
    {
        CursorState& state = m_cursors[direction];
        state.cursorType   = FIFE::CURSOR_IMAGE;
        state.cursorId     = FIFE::NC_ARROW;
        state.cursorImage  = image;
        state.cursorAnimation.reset();
    }

    void ResizableWindow::set(CursorDirections direction, FIFE::AnimationPtr const & anim)
    {
        CursorState& state = m_cursors[direction];
        state.cursorType   = FIFE::CURSOR_ANIMATION;
        state.cursorId     = FIFE::NC_ARROW;
        state.cursorImage.reset();
        state.cursorAnimation = anim;
    }

    FIFE::MouseCursorType ResizableWindow::getType(CursorDirections direction) const
    {
        return m_cursors[direction].cursorType;
    }

    uint32_t ResizableWindow::getId(CursorDirections direction) const
    {
        return m_cursors[direction].cursorId;
    }

    FIFE::ImagePtr ResizableWindow::getImage(CursorDirections direction)
    {
        return m_cursors[direction].cursorImage;
    }

    FIFE::AnimationPtr ResizableWindow::getAnimation(CursorDirections direction)
    {
        return m_cursors[direction].cursorAnimation;
    }

    void ResizableWindow::resizeToContent(bool recursiv)
    {
        // we keep the size in case we want it resizable
        if (m_resizable) {
            int const w = getWidth();
            int const h = getHeight();
            Window::resizeToContent(recursiv);
            setSize(w, h);
        } else {
            Window::resizeToContent(recursiv);
        }
    }

    void ResizableWindow::mouseEntered(MouseEvent& mouseEvent)
    {
        if (m_resizable && !m_resizing) {
            saveCursor();
        }
        Window::mouseEntered(mouseEvent);
    }

    void ResizableWindow::mouseExited(MouseEvent& mouseEvent)
    {
        if (m_resizable && !m_resizing && m_saved.cursorType != FIFE::CURSOR_NONE) {
            restoreCursor();
        } else if (m_resizable && m_resizing) {
            mouseEvent.consume();
        }
        Window::mouseExited(mouseEvent);
    }

    void ResizableWindow::mousePressed(MouseEvent& mouseEvent)
    {
        if (m_resizable && mouseEvent.getButton() == MouseEvent::Button::Left) {
            m_resizeLeft   = m_resizableLeft && mouseEvent.getX() < m_borderDistance;
            m_resizeRight  = m_resizableRight && mouseEvent.getX() > getWidth() - m_borderDistance;
            m_resizeTop    = m_resizableTop && mouseEvent.getY() < m_borderDistance;
            m_resizeBottom = m_resizableBottom && mouseEvent.getY() > getHeight() - m_borderDistance;
            if (m_resizeTop || m_resizeBottom || m_resizeLeft || m_resizeRight) {
                m_resizing = true;
                mouseEvent.consume();
            }
        }
        Window::mousePressed(mouseEvent);
    }

    void ResizableWindow::mouseReleased(MouseEvent& mouseEvent)
    {
        if (m_resizing) {
            adaptLayout();
            m_resizing = false;
            if (mouseEvent.getX() <= 0 || mouseEvent.getX() >= getWidth() || mouseEvent.getY() <= 0 ||
                mouseEvent.getY() >= getHeight()) {
                mouseExited(mouseEvent);
            }
            mouseEvent.consume();
        } else {
            Window::mouseReleased(mouseEvent);
        }
    }

    void ResizableWindow::mouseMoved(MouseEvent& mouseEvent)
    {
        if (m_resizable && !m_resizing && m_saved.cursorType != FIFE::CURSOR_NONE) {
            // use the CursorDirections directly
            uint32_t index = 0;
            index += (m_resizableLeft && mouseEvent.getX() < m_borderDistance) ? 1 : 0;
            index += (m_resizableRight && mouseEvent.getX() > getWidth() - m_borderDistance) ? 2 : 0;
            index += (m_resizableTop && mouseEvent.getY() < m_borderDistance) ? 3 : 0;
            index += (m_resizableBottom && mouseEvent.getY() > getHeight() - m_borderDistance) ? 6 : 0;

            if (index > 0) {
                // to get the real index value
                index -= 1;
                CursorState const state          = m_cursors[index];
                FIFE::MouseCursorType const type = state.cursorType;
                switch (type) {
                case FIFE::CURSOR_NATIVE:
                    m_cursor->set(state.cursorId);
                    break;
                case FIFE::CURSOR_IMAGE:
                    m_cursor->set(state.cursorImage);
                    break;
                case FIFE::CURSOR_ANIMATION:
                    m_cursor->set(state.cursorAnimation);
                    break;
                case FIFE::CURSOR_NONE:
                    break;
                default:
                    break;
                }
                mouseEvent.consume();
            } else {
                restoreCursor();
            }
        }
        Window::mouseMoved(mouseEvent);
    }

    void ResizableWindow::mouseDragged(MouseEvent& mouseEvent)
    {
        if (m_resizable && m_resizing) {
            int const x = mouseEvent.getX();
            int const y = mouseEvent.getY();
            if (m_resizeLeft) {
                int const oldW = getWidth();
                int newW       = std::max(0, oldW - x);
                setWidth(newW);
                if (m_shove) {
                    setX(getX() + x);
                } else {
                    newW           = getWidth();
                    int const newX = oldW - newW;
                    setX(getX() + newX);
                }
            } else if (m_resizeRight) {
                setWidth(x);
                if (x != getWidth() && m_shove) {
                    setX(getX() + (x - getWidth()));
                }
            }

            if (m_resizeTop) {
                int const oldH = getHeight();
                int newH       = std::max(0, oldH - y);
                setHeight(newH);
                if (m_shove) {
                    setY(getY() + y);
                } else {
                    newH           = getHeight();
                    int const newY = oldH - newH;
                    setY(getY() + newY);
                }
            } else if (m_resizeBottom) {
                setHeight(y);
                if (y != getHeight() && m_shove) {
                    setY(getY() + (y - getHeight()));
                }
            }
            mouseEvent.consume();
        } else {
            Window::mouseDragged(mouseEvent);
        }
    }

    void ResizableWindow::focusLost([[maybe_unused]] Event const & event)
    {
        m_resizing = false;
        restoreCursor();
    }
} // namespace fcn
