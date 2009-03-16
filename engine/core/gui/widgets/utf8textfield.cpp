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


