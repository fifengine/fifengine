// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "gui/fifechan/base/gui_font.h"
#include "util/base/exception.h"
#include "video/image.h"

#include "clicklabel.h"

namespace fcn
{
    ClickLabel::ClickLabel()
    {
        mGuiFont = static_cast<FIFE::GuiFont*>(getFont());
        setAlignment(Graphics::Alignment::Left);
        setOpaque(true);
        mTextWrapping = false;
        setBorderSize(0);
        mHasMouse = false, mKeyPressed = false, mMousePressed = false;

        addMouseListener(this);
        addKeyListener(this);
        addFocusListener(this);
        addWidgetListener(this);
    }

    ClickLabel::ClickLabel(const std::string& caption)
    {
        mGuiFont = static_cast<FIFE::GuiFont*>(getFont());
        setCaption(caption);
        setAlignment(Graphics::Alignment::Left);
        setOpaque(true);
        mTextWrapping = false;
        setBorderSize(0);
        mHasMouse = false, mKeyPressed = false, mMousePressed = false;

        adjustSize();
        addMouseListener(this);
        addKeyListener(this);
        addFocusListener(this);
        addWidgetListener(this);
    }

    ClickLabel::~ClickLabel() = default;

    void ClickLabel::setCaption(const std::string& caption)
    {
        mCaption = caption;
        mGuiFont = static_cast<FIFE::GuiFont*>(getFont());
        wrapText();
    }

    const std::string& ClickLabel::getCaption() const
    {
        return mCaption;
    }

    void ClickLabel::setAlignment(Graphics::Alignment alignment)
    {
        mAlignment = alignment;
    }

    Graphics::Alignment ClickLabel::getAlignment() const
    {
        return mAlignment;
    }

    void ClickLabel::setOpaque(bool opaque)
    {
        mOpaque = opaque;
    }

    bool ClickLabel::isOpaque() const
    {
        return mOpaque;
    }

    void ClickLabel::setTextWrapping(bool textWrapping)
    {
        bool wrappingEnabled = !mTextWrapping && textWrapping;
        mTextWrapping        = textWrapping;
        if (wrappingEnabled) {
            wrapText();
        }
    }

    bool ClickLabel::isTextWrapping() const
    {
        return mTextWrapping;
    }

    void ClickLabel::wrapText()
    {
        if (isTextWrapping() && (mGuiFont != nullptr)) {
            int32_t w    = getWidth() - 2 * getBorderSize() - getPaddingLeft() - getPaddingRight();
            mWrappedText = mGuiFont->splitTextToWidth(mCaption, w);
        }
    }

    void ClickLabel::setDimension(const Rectangle& dimension)
    {
        int32_t w = getWidth();
        Widget::setDimension(dimension);
        if (getWidth() != w && isTextWrapping()) {
            wrapText();
        }
    }

    void ClickLabel::resizeToContent(bool recursiv)
    {
        adjustSize();
    }

    void ClickLabel::adjustSize()
    {
        if (mGuiFont != nullptr) {
            int32_t w = getWidth();
            int32_t h = 0;
            if (isTextWrapping()) {
                if (getParent() != nullptr) {
                    w = getParent()->getChildrenArea().width;
                }
                int32_t textW = w - 2 * getBorderSize() - getPaddingLeft() - getPaddingRight();
                int32_t maxW  = isFixedSize() ? getFixedSize().getWidth() : getMaxSize().getWidth();
                if (textW < 1) {
                    w     = maxW;
                    textW = w - 2 * getBorderSize() - getPaddingLeft() - getPaddingRight();
                } else if (w > maxW) {
                    w     = std::min(w, maxW);
                    textW = w - 2 * getBorderSize() - getPaddingLeft() - getPaddingRight();
                }
                mWrappedText = mGuiFont->splitTextToWidth(mCaption, textW);
            } else {
                FIFE::Image* image = mGuiFont->getAsImageMultiline(mCaption);
                w                  = image->getWidth() + 2 * getBorderSize() + getPaddingLeft() + getPaddingRight();
            }
            const std::string& text = isTextWrapping() ? mWrappedText : mCaption;
            FIFE::Image* image      = mGuiFont->getAsImageMultiline(text);
            h                       = 2 * getBorderSize() + getPaddingTop() + getPaddingBottom() + image->getHeight();
            setSize(w, h);
        }
    }

    void ClickLabel::draw(Graphics* graphics)
    {
        bool active = isFocused();
        Rectangle offsetRec(getBorderSize(), getBorderSize(), 2 * getBorderSize(), 2 * getBorderSize());

        if (isOpaque()) {
            Color faceColor          = getBackgroundColor();
            const auto selectionMode = static_cast<uint8_t>(getSelectionMode());
            if (active && ((selectionMode & static_cast<uint8_t>(Widget::SelectionMode::Background)) != 0)) {
                faceColor = getSelectionColor();
            }
            graphics->setColor(faceColor);
            graphics->fillRectangle(
                offsetRec.x, offsetRec.y, getWidth() - offsetRec.width, getHeight() - offsetRec.height);
        }

        if (getBorderSize() > 0) {
            const auto selectionMode = static_cast<uint8_t>(getSelectionMode());
            if (active && ((selectionMode & static_cast<uint8_t>(Widget::SelectionMode::Border)) != 0)) {
                drawSelectionFrame(graphics);
            } else {
                drawBorder(graphics);
            }
        }

        if (mGuiFont != nullptr) {
            graphics->setColor(getForegroundColor());
            const std::string& text = isTextWrapping() ? mWrappedText : mCaption;
            FIFE::Image* image      = mGuiFont->getAsImageMultiline(text);

            int32_t textX = 0;
            int32_t textY =
                offsetRec.y + getPaddingTop() +
                (getHeight() - offsetRec.height - getPaddingTop() - getPaddingBottom() - image->getHeight()) / 2;

            switch (getAlignment()) {
            case Graphics::Alignment::Left:
                textX = offsetRec.x + getPaddingLeft();
                break;
            case Graphics::Alignment::Center:
                textX = offsetRec.x + getPaddingLeft() +
                        (getWidth() - offsetRec.width - getPaddingLeft() - getPaddingRight() - image->getWidth()) / 2;
                break;
            case Graphics::Alignment::Right:
                textX = getWidth() - offsetRec.x - getPaddingRight() - image->getWidth();
                break;
            default:
                fcn::throwException("Unknown alignment.");
            }

            mGuiFont->drawMultiLineString(graphics, text, textX, textY);
        }
    }

    void ClickLabel::fontChanged()
    {
        mGuiFont = static_cast<FIFE::GuiFont*>(getFont());
        wrapText();
        adjustSize();
    }

    void ClickLabel::mousePressed(MouseEvent& mouseEvent)
    {
        if (mouseEvent.getButton() == MouseEvent::Button::Left) {
            mMousePressed = true;
            mouseEvent.consume();
        }
    }

    void ClickLabel::mouseExited(MouseEvent& mouseEvent)
    {
        mHasMouse = false;
    }

    void ClickLabel::mouseEntered(MouseEvent& mouseEvent)
    {
        mHasMouse = true;
    }

    void ClickLabel::mouseReleased(MouseEvent& mouseEvent)
    {
        if (mouseEvent.getButton() == MouseEvent::Button::Left && mMousePressed && mHasMouse) {
            mMousePressed = false;
            distributeActionEvent();
            mouseEvent.consume();
        } else if (mouseEvent.getButton() == MouseEvent::Button::Left) {
            mMousePressed = false;
            mouseEvent.consume();
        }
    }

    void ClickLabel::mouseDragged(MouseEvent& mouseEvent)
    {
        mouseEvent.consume();
    }

    void ClickLabel::keyPressed(KeyEvent& keyEvent)
    {
        Key key = keyEvent.getKey();

        if (key.getValue() == Key::Enter || key.getValue() == Key::Space) {
            mKeyPressed = true;
            keyEvent.consume();
        }
    }

    void ClickLabel::keyReleased(KeyEvent& keyEvent)
    {
        Key key = keyEvent.getKey();

        if ((key.getValue() == Key::Enter || key.getValue() == Key::Space) && mKeyPressed) {
            mKeyPressed = false;
            distributeActionEvent();
            keyEvent.consume();
        }
    }

    void ClickLabel::focusLost(const Event& event)
    {
        mMousePressed = false;
        mKeyPressed   = false;
        mHasMouse     = false;
    }

    void ClickLabel::ancestorHidden(const Event& event)
    {
        mMousePressed = false;
        mKeyPressed   = false;
        mHasMouse     = false;
    }
} // namespace fcn
