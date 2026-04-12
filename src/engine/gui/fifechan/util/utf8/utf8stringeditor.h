// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FCN_UTF8STRINGEDITOR_HPP
#define FCN_UTF8STRINGEDITOR_HPP

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <fifechan.hpp>

// FIFE includes

namespace fcn
{

    /**
     * UTF-8 string editor.
     *
     * This is a helper class which allows to use UTF-8 strings in
     * your application.
     *
     * @author Przemyslaw Grzywacz
     */
    class /*FIFE_API*/ UTF8StringEditor
    {
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

}; // namespace fcn

#endif // !FCN_UTF8STRINGEDITOR_HPP
