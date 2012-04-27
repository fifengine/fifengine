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
#include <guichan/text.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/utf8/utf8.h"

#include "utf8textbox.h"

namespace gcn {

	UTF8TextBox::UTF8TextBox(const std::string & text)
		:TextBox(text), mStringEditor(new UTF8StringEditor())
	{
	}

	UTF8TextBox::~ UTF8TextBox()
	{
		delete mStringEditor;
	}

	void UTF8TextBox::keyPressed(KeyEvent& keyEvent)
	{
		Key key = keyEvent.getKey();

		if (key.getValue() == Key::Left)
		{
			if (getCaretColumn() == 0)
			{
				if (getCaretRow() > 0)
				{
					setCaretRow(getCaretRow()-1);
					setCaretColumn(getTextRow(getCaretRow()).size());
				}
			}
			else
			{
				setCaretColumn(mStringEditor->prevChar(getTextRow(getCaretRow()), getCaretColumn()));
			}
		}
		else if (key.getValue() == Key::Right)
		{
			if (getCaretColumn() < getTextRow(getCaretRow()).size())
			{
				setCaretColumn(mStringEditor->nextChar(getTextRow(getCaretRow()), getCaretColumn()));
			}
			else
			{
				if (getCaretRow() < getNumberOfRows() - 1)
				{
					setCaretRow(getCaretRow()+1);
					setCaretColumn(0);
				}
			}
		}
		else if (key.getValue() == Key::Down)
		{
			setCaretRowUTF8(getCaretRow() + 1);
		}

		else if (key.getValue() == Key::Up)
		{
			setCaretRowUTF8(getCaretRow() - 1);
		}

		else if (key.getValue() == Key::Home)
		{
			setCaretColumn(0);
		}

		else if (key.getValue() == Key::End)
		{
			setCaretColumn(getTextRow(getCaretRow()).size());
		}
		else if (key.getValue() == Key::Enter && mEditable)
		{
			mText->insertRow(getTextRow(getCaretRow()).substr(getCaretColumn(), getTextRow(getCaretRow()).size() - getCaretColumn())
                            ,getCaretRow() + 1);
            
			mText->getRow(getCaretRow()).resize(getCaretColumn());
			setCaretRow(getCaretRow() + 1);
			setCaretColumn(0);
		}
        else if (key.getValue() == Key::Backspace
					&& getCaretColumn() != 0
					&& mEditable)
		{
            std::string& currRow = mText->getRow(getCaretRow());
			setCaretColumn(mStringEditor->prevChar(currRow, static_cast<int>(getCaretColumn())));
			setCaretColumn(mStringEditor->eraseChar(currRow, static_cast<int>(getCaretColumn())));
		}
		else if (key.getValue() == Key::Backspace
					&& getCaretColumn() == 0
					&& getCaretRow() != 0
					&& mEditable)
		{
			setCaretColumn(getTextRow(getCaretRow() - 1).size());
			mText->getRow(getCaretRow() - 1) += getTextRow(getCaretRow());
			mText->eraseRow(getCaretRow());
			setCaretRow(getCaretRow() - 1);
		}
		else if (key.getValue() == Key::Delete
					&& getCaretColumn() < (int32_t)getTextRow(getCaretRow()).size()
					&& mEditable)
        {
			setCaretColumn(mStringEditor->eraseChar(mText->getRow(getCaretRow()), getCaretColumn()));
		}
		else if (key.getValue() == Key::Delete
					&& getCaretColumn() == (int32_t)getTextRow(getCaretRow()).size()
					&& getCaretRow() < ((int32_t)getNumberOfRows() - 1)
					&& mEditable)
		{
			mText->getRow(getCaretRow()) += getTextRow((getCaretRow() + 1));
			mText->eraseRow(getCaretRow() + 1);
		}
     	else if(key.getValue() == Key::PageUp)
		{
			Widget* par = getParent();

			if (par != NULL)
			{
				int32_t rowsPerPage = par->getChildrenArea().height / getFont()->getHeight();
				int32_t chars = mStringEditor->countChars(getTextRow(getCaretRow()), getCaretColumn());
				setCaretRow(getCaretRow() - rowsPerPage);

				if (getCaretRow() < 0)
				{
					setCaretRow(0);
				}
				setCaretColumn(mStringEditor->getOffset(getTextRow(getCaretRow()), chars));
			}
		}
		else if(key.getValue() == Key::PageDown)
		{
			Widget* par = getParent();

			if (par != NULL)
			{
				int32_t rowsPerPage = par->getChildrenArea().height / getFont()->getHeight();
				int32_t chars = mStringEditor->countChars(getTextRow(getCaretRow()), getCaretColumn());
				setCaretRow(getCaretRow() + rowsPerPage);

				if (getCaretRow() >= (int32_t)getNumberOfRows())
				{
					setCaretRow(getNumberOfRows() - 1);
				}

				setCaretColumn(mStringEditor->getOffset(getTextRow(getCaretRow()), chars));
			}
		}
		else if(key.getValue() == Key::Tab
				&& mEditable)
		{
			// FIXME: jump X spaces, so getCaretColumn() % TAB_SIZE = 0 and X <= TAB_SIZE
			mText->getRow(getCaretRow()).insert(getCaretColumn(),std::string("    "));
			setCaretColumn(getCaretColumn() + 4);
		}
		else if ((key.isCharacter() || key.getValue() > 255)
					&& mEditable)
		{
			setCaretColumn(mStringEditor->insertChar(mText->getRow(getCaretRow()), getCaretColumn(), key.getValue()));
		}

		adjustSize();
		scrollToCaret();
		assert( utf8::is_valid(getTextRow(getCaretRow()).begin(),getTextRow(getCaretRow()).end()) );
		assert( utf8::is_valid(getTextRow(getCaretRow()).begin(),getTextRow(getCaretRow()).begin() + getCaretColumn()) );
		keyEvent.consume();
	}


	void UTF8TextBox::setCaretColumnUTF8(int32_t column)
	{
		// no need to clip the column, mStringEditor handles it automaticly
		setCaretColumn(mStringEditor->getOffset(getTextRow(getCaretRow()), column));
	}

	void UTF8TextBox::setCaretRowUTF8(int32_t row)
	{
		int32_t chars = mStringEditor->countChars(getTextRow(getCaretRow()), getCaretColumn());
		if (row < 0) {
			row = 0;
		} else if (row >= getNumberOfRows()) {
			row = getNumberOfRows() - 1;
		}
		setCaretRow(row);
		setCaretColumn(mStringEditor->getOffset(getTextRow(getCaretRow()), chars));
	}

}


