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

#ifndef FCN_UTF8STRINGEDITOR_HPP
#define FCN_UTF8STRINGEDITOR_HPP

// Standard C++ library includes

// 3rd party library includes
#include <fifechan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace fcn {

	/**
	 * UTF-8 string editor.
	 * 
	 * This is a helper class which allows to use UTF-8 strings in
	 * your application.
	 * 
	 * @author Przemyslaw Grzywacz
	 */
	class UTF8StringEditor {
	public:
		/**
		 * Returns byte offset of the next character.
		 * 
		 * @param text UTF-8 text to navigate.
		 * @param byteOffset Byte offset of current character.
		 * @return Byte offset of the next character.
		 */
		static int nextChar(const std::string& text, int byteOffset);

		/**
		 * Returns byte offset of the previous character.
		 * 
		 * @param text UTF-8 text to navigate.
		 * @param byteOffset Byte offset of current character.
		 * @return Byte offset of the previous character.
		 */
		static int prevChar(const std::string& text, int byteOffset);

		/**
		 * Erase character at specified byte offset.
		 * 
		 * @param text UTF-8 text to modify.
		 * @param byteOffset Byte offset of the character to erase.
		 * @return New byte offset (is equal to byteOffset).
		 */
		static int eraseChar(std::string& text, int byteOffset);

		/**
		 * Insert a character at specified byte offset.
		 * 
		 * @param text UTF-8 text to modify.
		 * @param byteOffset Byte offset where character will be inserted.
		 * @param ch Unicode character to insert.
		 * @return New byte offset (after the new character).
		 */
		static int insertChar(std::string& text, int byteOffset, int ch);

		/**
		 * Counts characters up to byteOffset.
		 *
		 * @param text UTF-8 text to navigate.
		 * @param byteOffset Byte offset inside the text.
		 * @return Number of characters.
		 */
		static int countChars(const std::string& text, int byteOffset);

		/**
		 * Gets byte offset for character index.
		 *
		 * This method automaticly clips charIndex to be inside
		 * the string + EOF
		 *
		 * @param text UTF-8 text to navigate.
		 * @param charIndex Character index to move to.
		 * @return Byte offset of character at charIndex.
		 */
		static int getOffset(const std::string& text, int charIndex);
	};

};

#endif // !FCN_UTF8STRINGEDITOR_HPP
