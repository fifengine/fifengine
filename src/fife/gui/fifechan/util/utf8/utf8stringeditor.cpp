// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "utf8stringeditor.h"

// Standard C++ library includes
#include <cassert>
#include <iostream>
#include <limits>
#include <string>

// 3rd party library includes
#include <utf8cpp/utf8.h>

// FIFE includes

namespace fcn
{

    int UTF8StringEditor::nextChar(std::string const & text, int byteOffset)
    {
        std::string::const_iterator c;
        std::string::const_iterator e;

        c = text.begin() + byteOffset;
        e = text.end();

        utf8::next(c, e);
        size_t const nextOffset = std::string(text.begin(), c).size();
        assert(nextOffset <= static_cast<size_t>(std::numeric_limits<int>::max()));
        return static_cast<int>(nextOffset);
    }

    int UTF8StringEditor::prevChar(std::string const & text, int byteOffset)
    {
        std::string::const_iterator c;
        std::string::const_iterator b;

        c = text.begin() + byteOffset;
        b = text.begin();

        utf8::prior(c, b);
        size_t const previousOffset = std::string(b, c).size();
        assert(previousOffset <= static_cast<size_t>(std::numeric_limits<int>::max()));
        return static_cast<int>(previousOffset);
    }

    int UTF8StringEditor::eraseChar(std::string& text, int byteOffset)
    {
        std::string::iterator begin;
        std::string::iterator cur;
        begin = text.begin() + byteOffset;
        cur   = begin;
        utf8::next(cur, text.end());

        text = std::string(text.begin(), begin) + std::string(cur, text.end());
        return byteOffset; // this shouldn't change!
    }

    int UTF8StringEditor::insertChar(std::string& text, int byteOffset, int ch)
    {
        std::string newText;
        std::string::iterator cut = std::string::iterator();
        int newOffset             = 0;

        // make a temp string from left part of the caret (+6 extra chars)
        newText = text.substr(0, byteOffset) + "        ";
        // append character
        utf8::append(ch, newText.begin() + byteOffset);
        // calculate newText real length
        cut = newText.begin() + byteOffset;
        utf8::next(cut, newText.end());
        // cut the string to real length
        newText.assign(newText.begin(), cut);
        assert(newText.size() <= static_cast<size_t>(std::numeric_limits<int>::max()));
        newOffset = static_cast<int>(newText.size());
        // make new text
        text = newText + text.substr(byteOffset);

        return newOffset;
    }

    int UTF8StringEditor::countChars(std::string const & text, int byteOffset)
    {
        return utf8::distance(text.begin(), text.begin() + byteOffset);
    }

    int UTF8StringEditor::getOffset(std::string const & text, int charIndex)
    {
        std::string::const_iterator cur = std::string::const_iterator();
        std::string::const_iterator end = std::string::const_iterator();
        int i                           = 0;

        if (charIndex < 0) {
            return 0;
        }

        cur = text.begin();
        end = text.end();

        for (i = 0; i < charIndex && cur != end; i++) {
            utf8::next(cur, end);
        }

        size_t const offset = std::string(text.begin(), cur).size();
        assert(offset <= static_cast<size_t>(std::numeric_limits<int>::max()));
        return static_cast<int>(offset);
    }
}; // namespace fcn
