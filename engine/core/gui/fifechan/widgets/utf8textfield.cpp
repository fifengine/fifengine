/***************************************************************************
 *   Copyright (C) 2009 by the FIFE team                                   *
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
#include <fifechan/text.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "utf8textfield.h"

namespace fcn {

	UTF8TextField::UTF8TextField(const std::string & text)
		: TextField(text)
	{
		mStringEditor = new UTF8StringEditor();
	}

	UTF8TextField::~UTF8TextField()
	{
		delete mStringEditor;
	}

	void UTF8TextField::keyPressed(KeyEvent & keyEvent)
	{
		
		Key key = keyEvent.getKey();

		if (key.getValue() == Key::Left && getCaretPosition() > 0)
		{
			setCaretPosition(mStringEditor->prevChar(getText(), static_cast<int>(getCaretPosition())));
		}
		else if (key.getValue() == Key::Right && getCaretPosition() < getText().size())
		{
			setCaretPosition(mStringEditor->nextChar(getText(), static_cast<int>(getCaretPosition())));
		}
		else if (key.getValue() == Key::Delete && getCaretPosition() < getText().size())
		{
			std::string textfieldContents(getText());
		  
			setCaretPosition(mStringEditor->eraseChar(textfieldContents, static_cast<int>(getCaretPosition())));
			setText(textfieldContents);
		}
		else if (key.getValue() == Key::Backspace && getCaretPosition() > 0 && mText->getNumberOfRows() > 0)
		{
			setCaretPosition(mStringEditor->prevChar(mText->getRow(0), static_cast<int>(getCaretPosition())));
			setCaretPosition(mStringEditor->eraseChar(mText->getRow(0), static_cast<int>(getCaretPosition())));
		}
		else if (key.getValue() == Key::Enter)
		{
			distributeActionEvent();
		}
		else if (key.getValue() == Key::Home)
		{
			setCaretPosition(0);
		}

		else if (key.getValue() == Key::End)
		{
			setCaretPosition(getText().size());
		}

		// Add character to text, if key is realy a ASCII character
		// or is greater than 8bits long and the character is not
		// the tab key.

		else if ((key.isCharacter() || key.getValue() > 255 && mText->getNumberOfRows() > 0)
			&& key.getValue() != Key::Tab)
		{
			setCaretPosition(mStringEditor->insertChar(mText->getRow(0), getCaretPosition(), key.getValue()));
		}

		if (key.getValue() != Key::Tab)
		{
			// consume all characters except TAB which is needed
			// for traversing through widgets in a container.
			keyEvent.consume();
		}

		fixScroll();
	}

};


