/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
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
#include <boost/bind.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/time/timeevent.h"
#include "util/time/timemanager.h"

#include "commandline.h"

namespace FIFE {
	using namespace fcn;


	CommandLine::CommandLine() : fcn::TextField(), m_history_position(0) {

		m_blinkTimer.setInterval(500);
		m_blinkTimer.setCallback(boost::bind(&CommandLine::toggleCaretVisible,this));
		m_blinkTimer.start();

		m_suppressBlinkTimer.setInterval(2000);
		m_suppressBlinkTimer
			.setCallback(boost::bind(&CommandLine::startBlinking,this));
	}

	CommandLine::~CommandLine() {
	}

	void CommandLine::toggleCaretVisible() {
		m_caretVisible = !m_caretVisible;
	}

	void CommandLine::stopBlinking() {
		m_suppressBlinkTimer.start();
		m_blinkTimer.stop();
		m_caretVisible = true;
	}

	void CommandLine::startBlinking() {
		m_suppressBlinkTimer.stop();
		m_blinkTimer.start();
	}

	void CommandLine::keyPressed(fcn::KeyEvent &keyEvent) {
		fcn::Key key = keyEvent.getKey();
		int32_t keyType = key.getValue();

		if (keyType == Key::Left && getCaretPosition() > 0)
		{
			TextField::keyPressed(keyEvent);
		}
		else if (keyType == Key::Right && getCaretPosition() < getText().size())
		{
			TextField::keyPressed(keyEvent);
		}
		else if (keyType == Key::Down && !m_history.empty())
		{
			if( m_history_position < m_history.size() ) {

				if( ++m_history_position == m_history.size() ) {
					setText( m_cmdline );
				} else {
					setText( m_history[m_history_position] );
				}
			};
		}
		else if (keyType == Key::Up && !m_history.empty())
		{
			if( m_history_position > 0 ) {
				if( m_history_position == m_history.size() ) {
					m_cmdline = getText();
				}
				--m_history_position;
				setText( m_history[m_history_position] );
			};
		}
		else if (keyType == Key::Delete && getCaretPosition() < getText().size())
		{
			TextField::keyPressed(keyEvent);
		}
		else if (keyType  == Key::Backspace && getCaretPosition() > 0)
		{
			TextField::keyPressed(keyEvent);
		}
		else if (keyType == Key::Enter)
		{
			if( getText() != "" ) {
				if(m_callback) {
					m_callback( getText() );
				}
				m_history.push_back( getText() );
				m_history_position = m_history.size();
				setText("");
			}
		}
		else if (keyType == Key::Home)
		{
			setCaretPosition(0);
		}
		else if (keyType == Key::End)
		{
			setCaretPosition(getText().size());
		}
		else if (key.isCharacter() || static_cast<uint32_t>(key.getValue()) > 255)
		{
			TextField::keyPressed(keyEvent);
		}
		stopBlinking();
		fixScroll();
	}

	void CommandLine::drawCaret(fcn::Graphics * graphics, int32_t x) {
		if( !m_caretVisible )
			return;

		graphics->setColor(getForegroundColor());
		graphics->drawLine(x, getHeight() - 2, x, 1);
		graphics->drawLine(x+1, getHeight() - 2, x+1, 1);
	}


	void CommandLine::setCallback(const type_callback& cb) {
		m_callback = cb;
	}

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
