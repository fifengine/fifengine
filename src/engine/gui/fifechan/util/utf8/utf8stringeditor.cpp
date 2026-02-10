/***************************************************************************
 *   Copyright (C) 2009 by the FIFE team                                   *
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
#include <iostream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/utf8/utf8.h"


#include "utf8stringeditor.h"

namespace fcn {

	int UTF8StringEditor::nextChar(const std::string & text, int byteOffset)
	{
		std::string::const_iterator c, e;

		c = text.begin() + byteOffset;
		e = text.end();

		utf8::next(c, e);
		return std::string(text.begin(), c).size();
	}

	int UTF8StringEditor::prevChar(const std::string & text, int byteOffset)
	{
		std::string::const_iterator c, b;

		c = text.begin() + byteOffset;
		b = text.begin();

		utf8::prior(c, b);
		return std::string(b, c).size();
	}

	int UTF8StringEditor::eraseChar(std::string & text, int byteOffset)
	{
		std::string::iterator begin, cur;
		begin = text.begin() + byteOffset;
		cur = begin;
		utf8::next(cur, text.end());

		text = std::string(text.begin(), begin) + std::string(cur, text.end());
		return byteOffset; // this shouldn't change!
	}

	int UTF8StringEditor::insertChar(std::string & text, int byteOffset, int ch)
	{
		std::string newText;
		std::string::iterator cut;
		int newOffset;

		// make a temp string from left part of the caret (+6 extra chars)
		newText = text.substr(0, byteOffset) + "        ";
		// append character
		utf8::append(ch, newText.begin() + byteOffset);
		// calculate newText real length
		cut = newText.begin() + byteOffset;
		utf8::next(cut, newText.end());
		// cut the string to real length
		newText = std::string(newText.begin(), cut);
		newOffset = newText.size();
		// make new text
		text = newText + text.substr(byteOffset);

		return newOffset;
	}

	int UTF8StringEditor::countChars(const std::string & text, int byteOffset)
	{
		return utf8::distance(text.begin(), text.begin() + byteOffset);
	}

	int UTF8StringEditor::getOffset(const std::string & text, int charIndex)
	{
		std::string::const_iterator cur, end;
		int bytes = 0, i;

		if (charIndex < 0) return 0;

		cur = text.begin();
		end = text.end();

		for(i = 0; i < charIndex && cur != end; i++) {
			utf8::next(cur, end);
		}

		return std::string(text.begin(), cur).size();
	}
};






