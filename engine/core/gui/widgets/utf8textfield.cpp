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

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "utf8textfield.h"

namespace gcn {

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

		if (key.getValue() == Key::LEFT && mCaretPosition > 0)
		{
			mCaretPosition = mStringEditor->prevChar(mText, mCaretPosition);
		}
		else if (key.getValue() == Key::RIGHT && mCaretPosition < mText.size())
		{
			mCaretPosition = mStringEditor->nextChar(mText, mCaretPosition);
		}
		else if (key.getValue() == Key::DELETE && mCaretPosition < mText.size())
		{
			mCaretPosition = mStringEditor->eraseChar(mText, mCaretPosition);
		}
		else if (key.getValue() == Key::BACKSPACE && mCaretPosition > 0)
		{
			mCaretPosition = mStringEditor->prevChar(mText, mCaretPosition);
			mCaretPosition = mStringEditor->eraseChar(mText, mCaretPosition);
		}
		else if (key.getValue() == Key::ENTER)
		{
			distributeActionEvent();
		}
		else if (key.getValue() == Key::HOME)
		{
			mCaretPosition = 0;
		}

		else if (key.getValue() == Key::END)
		{
			mCaretPosition = mText.size();
		}

		// Add character to text, if key is realy a ASCII character
		// or is greater than 8bits long and the character is not
		// the tab key.
		else if ((key.isCharacter() || key.getValue() > 255)
			&& key.getValue() != Key::TAB)
		{
			mCaretPosition = mStringEditor->insertChar(mText, mCaretPosition, key.getValue());
		}

		if (key.getValue() != Key::TAB)
		{
			// consume all characters except TAB which is needed
			// for traversing through widgets in a container.
			keyEvent.consume();
		}

		fixScroll();
	}

};


