/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
	using namespace gcn;


	CommandLine::CommandLine() : gcn::UTF8TextField(), m_history_position(0) {

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

	void CommandLine::keyPressed(gcn::KeyEvent &keyEvent) {
		gcn::Key key = keyEvent.getKey();
		int keyType = key.getValue();
		
		if (keyType == Key::LEFT && mCaretPosition > 0)
		{
			UTF8TextField::keyPressed(keyEvent);
		}
		else if (keyType == Key::RIGHT && mCaretPosition < mText.size())
		{
			UTF8TextField::keyPressed(keyEvent);
		}
		else if (keyType == Key::DOWN && !m_history.empty())
		{
			if( m_history_position < m_history.size() ) {
				
				if( ++m_history_position == m_history.size() ) {
					setText( m_cmdline ); 
				} else {
					setText( m_history[m_history_position] ); 
				}
			};
		}
		else if (keyType == Key::UP && !m_history.empty())
		{
			if( m_history_position > 0 ) {
				if( m_history_position == m_history.size() ) {
					m_cmdline = mText; 
				}
				--m_history_position;
				setText( m_history[m_history_position] ); 
			};
		}
		else if (keyType == Key::DELETE && mCaretPosition < mText.size())
		{
			UTF8TextField::keyPressed(keyEvent);
		}
		else if (keyType  == Key::BACKSPACE && mCaretPosition > 0)
		{
			UTF8TextField::keyPressed(keyEvent);
		}
		else if (keyType == Key::ENTER)
		{
			if( mText != "" ) {
				if(m_callback) {
					m_callback( mText );
				}
				m_history.push_back( mText ); 
				m_history_position = m_history.size();
				setText("");
			}
		}
		else if (keyType == Key::HOME)
		{
			mCaretPosition = 0;
		}    
		else if (keyType == Key::END)
		{
			mCaretPosition = mText.size();
		}    
		else if (key.isCharacter())
		{
			UTF8TextField::keyPressed(keyEvent);
		}
		stopBlinking();
		fixScroll();
	}

	void CommandLine::drawCaret(gcn::Graphics * graphics, int x) {
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
