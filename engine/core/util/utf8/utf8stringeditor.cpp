#include <iostream>
#include "utf8stringeditor.h"
#include "utf8.h"

namespace gcn {

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






