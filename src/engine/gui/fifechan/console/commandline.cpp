// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/time/timeevent.h"
#include "util/time/timemanager.h"

#include "commandline.h"

namespace FIFE
{
    using namespace fcn;

    CommandLine::CommandLine() : fcn::TextField(), m_history_position(0)
    {

        m_blinkTimer.setInterval(500);
        m_blinkTimer.setCallback(std::bind(&CommandLine::toggleCaretVisible, this));
        m_blinkTimer.start();

        m_suppressBlinkTimer.setInterval(2000);
        m_suppressBlinkTimer.setCallback(std::bind(&CommandLine::startBlinking, this));
    }

    CommandLine::~CommandLine() = default;

    void CommandLine::toggleCaretVisible()
    {
        m_caretVisible = !m_caretVisible;
    }

    void CommandLine::stopBlinking()
    {
        m_suppressBlinkTimer.start();
        m_blinkTimer.stop();
        m_caretVisible = true;
    }

    void CommandLine::startBlinking()
    {
        m_suppressBlinkTimer.stop();
        m_blinkTimer.start();
    }

    void CommandLine::keyPressed(fcn::KeyEvent& keyEvent)
    {
        fcn::Key key    = keyEvent.getKey();
        int32_t keyType = key.getValue();

        if (keyType == Key::Left && getCaretPosition() > 0) {
            TextField::keyPressed(keyEvent);
        } else if (keyType == Key::Right && getCaretPosition() < getText().size()) {
            TextField::keyPressed(keyEvent);
        } else if (keyType == Key::Down && !m_history.empty()) {
            if (m_history_position < m_history.size()) {

                if (++m_history_position == m_history.size()) {
                    setText(m_cmdline);
                } else {
                    setText(m_history[m_history_position]);
                }
            }
        } else if (keyType == Key::Up && !m_history.empty()) {
            if (m_history_position > 0) {
                if (m_history_position == m_history.size()) {
                    m_cmdline = getText();
                }
                --m_history_position;
                setText(m_history[m_history_position]);
            }
        } else if (keyType == Key::Delete && getCaretPosition() < getText().size()) {
            TextField::keyPressed(keyEvent);
        } else if (keyType == Key::Backspace && getCaretPosition() > 0) {
            TextField::keyPressed(keyEvent);
        } else if (keyType == Key::Enter) {
            if (!getText().empty()) {
                if (m_callback) {
                    m_callback(getText());
                }
                m_history.push_back(getText());
                m_history_position = m_history.size();
                setText("");
            }
        } else if (keyType == Key::Home) {
            setCaretPosition(0);
        } else if (keyType == Key::End) {
            setCaretPosition(getText().size());
        } else if (key.isCharacter() || static_cast<uint32_t>(key.getValue()) > 255) {
            TextField::keyPressed(keyEvent);
        }
        stopBlinking();
        fixScroll();
    }

    void CommandLine::drawCaret(fcn::Graphics* graphics, int32_t x)
    {
        if (!m_caretVisible) {
            return;
        }

        graphics->setColor(getForegroundColor());
        graphics->drawLine(x, getHeight() - 2, x, 1);
        graphics->drawLine(x + 1, getHeight() - 2, x + 1, 1);
    }

    void CommandLine::setCallback(const type_callback& cb)
    {
        m_callback = cb;
    }

} // namespace FIFE
