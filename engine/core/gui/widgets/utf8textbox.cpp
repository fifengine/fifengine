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

		if (key.getValue() == Key::LEFT)
		{
			if (mCaretColumn == 0)
			{
				if (mCaretRow > 0)
				{
					mCaretRow--;
					mCaretColumn = mTextRows[mCaretRow].size();
				}
			}
			else
			{
				mCaretColumn = mStringEditor->prevChar(mTextRows[mCaretRow], mCaretColumn);
			}
		}
		else if (key.getValue() == Key::RIGHT)
		{
			if (mCaretColumn < mTextRows[mCaretRow].size())
			{
				mCaretColumn = mStringEditor->nextChar(mTextRows[mCaretRow], mCaretColumn);
			}
			else
			{
				if (mCaretRow < mTextRows.size() - 1)
				{
					mCaretRow++;
					mCaretColumn = 0;
				}
			}
		}
		else if (key.getValue() == Key::DOWN)
		{
			setCaretRowUTF8(mCaretRow + 1);
		}

		else if (key.getValue() == Key::UP)
		{
			setCaretRowUTF8(mCaretRow - 1);
		}

		else if (key.getValue() == Key::HOME)
		{
			mCaretColumn = 0;
		}

		else if (key.getValue() == Key::END)
		{
			mCaretColumn = mTextRows[mCaretRow].size();
		}

		else if (key.getValue() == Key::ENTER && mEditable)
		{
			mTextRows.insert(mTextRows.begin() + mCaretRow + 1,
								mTextRows[mCaretRow].substr(mCaretColumn, mTextRows[mCaretRow].size() - mCaretColumn));
			mTextRows[mCaretRow].resize(mCaretColumn);
			++mCaretRow;
			mCaretColumn = 0;
		}

		else if (key.getValue() == Key::BACKSPACE
					&& mCaretColumn != 0
					&& mEditable)
		{
			mCaretColumn = mStringEditor->prevChar(mTextRows[mCaretRow], mCaretColumn);
			mCaretColumn = mStringEditor->eraseChar(mTextRows[mCaretRow], mCaretColumn);
		}

		else if (key.getValue() == Key::BACKSPACE
					&& mCaretColumn == 0
					&& mCaretRow != 0
					&& mEditable)
		{
			mCaretColumn = mTextRows[mCaretRow - 1].size();
			mTextRows[mCaretRow - 1] += mTextRows[mCaretRow];
			mTextRows.erase(mTextRows.begin() + mCaretRow);
			mCaretRow--;
		}

		else if (key.getValue() == Key::DELETE
					&& mCaretColumn < (int)mTextRows[mCaretRow].size()
					&& mEditable)
		{
			mCaretColumn = mStringEditor->eraseChar(mTextRows[mCaretRow], mCaretColumn);
		}

		else if (key.getValue() == Key::DELETE
					&& mCaretColumn == (int)mTextRows[mCaretRow].size()
					&& mCaretRow < ((int)mTextRows.size() - 1)
					&& mEditable)
		{
			mTextRows[mCaretRow] += mTextRows[mCaretRow + 1];
			mTextRows.erase(mTextRows.begin() + mCaretRow + 1);
		}

		else if(key.getValue() == Key::PAGE_UP)
		{
			Widget* par = getParent();

			if (par != NULL)
			{
				int rowsPerPage = par->getChildrenArea().height / getFont()->getHeight();
				int chars = mStringEditor->countChars(mTextRows[mCaretRow], mCaretColumn);
				mCaretRow -= rowsPerPage;

				if (mCaretRow < 0)
				{
					mCaretRow = 0;
				}
				mCaretColumn = mStringEditor->getOffset(mTextRows[mCaretRow], chars);
			}
		}

		else if(key.getValue() == Key::PAGE_DOWN)
		{
			Widget* par = getParent();

			if (par != NULL)
			{
				int rowsPerPage = par->getChildrenArea().height / getFont()->getHeight();
				int chars = mStringEditor->countChars(mTextRows[mCaretRow], mCaretColumn);
				mCaretRow += rowsPerPage;

				if (mCaretRow >= (int)mTextRows.size())
				{
					mCaretRow = mTextRows.size() - 1;
				}

				mCaretColumn = mStringEditor->getOffset(mTextRows[mCaretRow], chars);
			}
		}

		else if(key.getValue() == Key::TAB
				&& mEditable)
		{
			// FIXME: jump X spaces, so mCaretColumn % TAB_SIZE = 0 and X <= TAB_SIZE
			mTextRows[mCaretRow].insert(mCaretColumn,std::string("    "));
			mCaretColumn += 4;
		}

		else if ((key.isCharacter() || key.getValue() > 255)
					&& mEditable)
		{
			mCaretColumn = mStringEditor->insertChar(mTextRows[mCaretRow], mCaretColumn, key.getValue());
		}

		adjustSize();
		scrollToCaret();
		assert( utf8::is_valid(mTextRows[mCaretRow].begin(),mTextRows[mCaretRow].end()) );
		assert( utf8::is_valid(mTextRows[mCaretRow].begin(),mTextRows[mCaretRow].begin() + mCaretColumn) );
		keyEvent.consume();
	}


	void UTF8TextBox::setCaretColumnUTF8(int column)
	{
		// no need to clip the column, mStringEditor handles it automaticly
		mCaretColumn = mStringEditor->getOffset(mTextRows[mCaretRow], column);
	}

	void UTF8TextBox::setCaretRowUTF8(int row)
	{
		int chars = mStringEditor->countChars(mTextRows[mCaretRow], mCaretColumn);
		if (row < 0) {
			row = 0;
		} else if (row >= mTextRows.size()) {
			row = mTextRows.size() - 1;
		}
		mCaretRow = row;
		mCaretColumn = mStringEditor->getOffset(mTextRows[mCaretRow], chars);
	}

}


