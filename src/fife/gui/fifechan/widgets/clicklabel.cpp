// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "clicklabel.h"

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <string>

// 3rd party library includes

// FIFE includes
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "video/fonts/fontinstanceifontadapter.h"
#include "video/image.h"

namespace fcn
{
    namespace
    {
        FIFE::Logger& _log()
        {
            static FIFE::Logger log(LM_GUI);
            return log;
        }

        bool shouldLogCaption(std::string const & caption)
        {
            return caption == "Text 1" || caption == "Text 2" || caption == "Text 3" || caption == "Credits";
        }
    } // namespace

    ClickLabel::ClickLabel() :
        mGuiFont(nullptr), mAlignment(Graphics::Alignment::Left), mOpaque(true), mTextWrapping(false)
    {
        ClickLabel::setAlignment(Graphics::Alignment::Left);
        ClickLabel::setOpaque(true);
        setBorderSize(0);
        mHasMouse = false, mKeyPressed = false, mMousePressed = false;

        addMouseListener(this);
        addKeyListener(this);
        addFocusListener(this);
        addWidgetListener(this);
    }

    ClickLabel::ClickLabel(std::string const & caption) :
        mGuiFont(nullptr), mAlignment(Graphics::Alignment::Left), mOpaque(true), mTextWrapping(false)
    {
        ClickLabel::setCaption(caption);
        ClickLabel::setAlignment(Graphics::Alignment::Left);
        ClickLabel::setOpaque(true);
        setBorderSize(0);
        mHasMouse = false, mKeyPressed = false, mMousePressed = false;

        ClickLabel::adjustSize();
        addMouseListener(this);
        addKeyListener(this);
        addFocusListener(this);
        addWidgetListener(this);
    }

    ClickLabel::~ClickLabel() = default;

    void ClickLabel::setCaption(std::string const & caption)
    {
        mCaption = caption;
        mGuiFont = dynamic_cast<FIFE::FontInstanceIFontAdapter*>(getFont());
        wrapText();
    }

    std::string const & ClickLabel::getCaption() const
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
        bool const wrappingEnabled = !mTextWrapping && textWrapping;
        mTextWrapping              = textWrapping;
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
            int32_t const w = getWidth() - static_cast<int32_t>(2 * getBorderSize()) -
                              static_cast<int32_t>(getPaddingLeft()) - static_cast<int32_t>(getPaddingRight());
            mWrappedText    = mGuiFont->splitTextToWidth(mCaption, w);
        }
    }

    void ClickLabel::setDimension(Rectangle const & dimension)
    {
        int32_t const w = getWidth();
        Widget::setDimension(dimension);
        if (getWidth() != w && isTextWrapping()) {
            wrapText();
        }
    }

    void ClickLabel::resizeToContent([[maybe_unused]] bool recursiv)
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
                int32_t textW = w - static_cast<int32_t>(2 * getBorderSize()) - static_cast<int32_t>(getPaddingLeft()) -
                                static_cast<int32_t>(getPaddingRight());
                int32_t const maxW = isFixedSize() ? getFixedSize().getWidth() : getMaxSize().getWidth();
                if (textW < 1) {
                    w     = maxW;
                    textW = (w - static_cast<int32_t>(2 * getBorderSize())) - static_cast<int32_t>(getPaddingLeft()) -
                            static_cast<int32_t>(getPaddingRight());
                } else if (w > maxW) {
                    w     = std::min(w, maxW);
                    textW = (w - static_cast<int32_t>(2 * getBorderSize())) - static_cast<int32_t>(getPaddingLeft()) -
                            static_cast<int32_t>(getPaddingRight());
                }
                mWrappedText = mGuiFont->splitTextToWidth(mCaption, textW);
            } else {
                FIFE::Image* image = mGuiFont->getAsImageMultiline(mCaption);
                w                  = static_cast<int32_t>(
                    image->getWidth() + (2 * getBorderSize()) + getPaddingLeft() + getPaddingRight());
            }
            std::string const & text = isTextWrapping() ? mWrappedText : mCaption;
            FIFE::Image* image       = mGuiFont->getAsImageMultiline(text);
            h = static_cast<int32_t>((2 * getBorderSize()) + getPaddingTop() + getPaddingBottom() + image->getHeight());
            setSize(w, h);
        }
    }

    void ClickLabel::draw(Graphics* graphics)
    {
        bool const active = isFocused();
        Rectangle const offsetRec(
            static_cast<int>(getBorderSize()),
            static_cast<int>(getBorderSize()),
            static_cast<int>(2 * getBorderSize()),
            static_cast<int>(2 * getBorderSize()));

        if (isOpaque()) {
            Color faceColor          = getBackgroundColor();
            auto const selectionMode = static_cast<uint8_t>(getSelectionMode());
            if (active && ((selectionMode & static_cast<uint8_t>(Widget::SelectionMode::Background)) != 0)) {
                faceColor = getSelectionColor();
            }
            graphics->setColor(faceColor);
            graphics->fillRectangle(
                offsetRec.x, offsetRec.y, getWidth() - offsetRec.width, getHeight() - offsetRec.height);
        }

        if (getBorderSize() > 0) {
            auto const selectionMode = static_cast<uint8_t>(getSelectionMode());
            if (active && ((selectionMode & static_cast<uint8_t>(Widget::SelectionMode::Border)) != 0)) {
                drawSelectionFrame(graphics);
            } else {
                drawBorder(graphics);
            }
        }

        if (mGuiFont != nullptr) {
            graphics->setColor(getForegroundColor());
            std::string const & text = isTextWrapping() ? mWrappedText : mCaption;
            FIFE::Image* image       = mGuiFont->getAsImageMultiline(text);

            int32_t textX = 0;
            int32_t const textY =
                offsetRec.y + static_cast<int32_t>(getPaddingTop()) +
                ((getHeight() - offsetRec.height - static_cast<int32_t>(getPaddingTop()) -
                  static_cast<int32_t>(getPaddingBottom()) - static_cast<int32_t>(image->getHeight())) /
                 2);

            switch (getAlignment()) {
            case Graphics::Alignment::Left:
                textX = offsetRec.x + static_cast<int32_t>(getPaddingLeft());
                break;
            case Graphics::Alignment::Center:
                textX = offsetRec.x + static_cast<int32_t>(getPaddingLeft()) +
                        ((getWidth() - offsetRec.width - static_cast<int32_t>(getPaddingLeft()) -
                          static_cast<int32_t>(getPaddingRight()) - static_cast<int32_t>(image->getWidth())) /
                         2);
                break;
            case Graphics::Alignment::Right:
                textX = getWidth() - offsetRec.x - static_cast<int32_t>(getPaddingRight()) -
                        static_cast<int32_t>(image->getWidth());
                break;
            default:
                fcn::throwException("Unknown alignment.");
            }

            if (shouldLogCaption(mCaption)) {
                Rectangle const childrenArea = getChildrenArea();
                FIFE::FL_LOG(
                    _log(),
                    std::format(
                        "ClickLabel::draw caption='{}' widget=({},{} {}x{}) children=({},{} {}x{}) textPos=({}, {}) "
                        "image={}x{} padding=({}, {}, {}, {}) border={} opaque={} visible={}",
                        mCaption,
                        getX(),
                        getY(),
                        getWidth(),
                        getHeight(),
                        childrenArea.x,
                        childrenArea.y,
                        childrenArea.width,
                        childrenArea.height,
                        textX,
                        textY,
                        image->getWidth(),
                        image->getHeight(),
                        getPaddingTop(),
                        getPaddingRight(),
                        getPaddingBottom(),
                        getPaddingLeft(),
                        getBorderSize(),
                        isOpaque(),
                        isVisible()));
            }

            mGuiFont->drawMultiLineString(graphics, text, textX, textY);
        }
    }

    void ClickLabel::fontChanged()
    {
        mGuiFont = dynamic_cast<FIFE::FontInstanceIFontAdapter*>(getFont());
        wrapText();
    }

    void ClickLabel::mousePressed(MouseEvent& mouseEvent)
    {
        if (mouseEvent.getButton() == MouseEvent::Button::Left) {
            mMousePressed = true;
            mouseEvent.consume();
        }
    }

    void ClickLabel::mouseExited([[maybe_unused]] MouseEvent& mouseEvent)
    {
        mHasMouse = false;
    }

    void ClickLabel::mouseEntered([[maybe_unused]] MouseEvent& mouseEvent)
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
        Key const key = keyEvent.getKey();

        if (key.getValue() == Key::KEY_RETURN || key.getValue() == Key::SPACE) {
            mKeyPressed = true;
            keyEvent.consume();
        }
    }

    void ClickLabel::keyReleased(KeyEvent& keyEvent)
    {
        Key const key = keyEvent.getKey();

        if ((key.getValue() == Key::KEY_RETURN || key.getValue() == Key::SPACE) && mKeyPressed) {
            mKeyPressed = false;
            distributeActionEvent();
            keyEvent.consume();
        }
    }

    void ClickLabel::focusLost([[maybe_unused]] Event const & event)
    {
        mMousePressed = false;
        mKeyPressed   = false;
        mHasMouse     = false;
    }

    void ClickLabel::ancestorHidden([[maybe_unused]] Event const & event)
    {
        mMousePressed = false;
        mKeyPressed   = false;
        mHasMouse     = false;
    }
} // namespace fcn
