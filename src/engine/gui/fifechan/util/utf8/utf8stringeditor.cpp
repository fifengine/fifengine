// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cassert>
#include <iostream>
#include <limits>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/utf8/utf8.h"

#include "utf8stringeditor.h"

namespace fcn
{

    int UTF8StringEditor::nextChar(const std::string& text, int byteOffset)
    {
        std::string::const_iterator c;
        std::string::const_iterator e;

        c = text.begin() + byteOffset;
        e = text.end();

        utf8::next(c, e);
        const size_t nextOffset = std::string(text.begin(), c).size();
        assert(nextOffset <= static_cast<size_t>(std::numeric_limits<int>::max()));
        return static_cast<int>(nextOffset);
    }

    int UTF8StringEditor::prevChar(const std::string& text, int byteOffset)
    {
        std::string::const_iterator c;
        std::string::const_iterator b;

        c = text.begin() + byteOffset;
        b = text.begin();

        utf8::prior(c, b);
        const size_t previousOffset = std::string(b, c).size();
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

    int UTF8StringEditor::countChars(const std::string& text, int byteOffset)
    {
        return utf8::distance(text.begin(), text.begin() + byteOffset);
    }

    int UTF8StringEditor::getOffset(const std::string& text, int charIndex)
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

        const size_t offset = std::string(text.begin(), cur).size();
        assert(offset <= static_cast<size_t>(std::numeric_limits<int>::max()));
        return static_cast<int>(offset);
    }
}; // namespace fcn
