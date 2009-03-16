#ifndef RAVERON_UTF8TEXTBOX_HPP
#define RAVERON_UTF8TEXTBOX_HPP

#include <guichan/widgets/textbox.hpp>
#include "util/utf8/utf8stringeditor.h"

namespace gcn {

	/**
	 * UTF-8 aware version of the TextBox class.
	 *
	 * Using UTF8StringEditor, it is able to correctly edit UTF-8 multiline texts.
	 *
	 * You will also need an UTF-8 aware font to be able to correctly
	 * display such text (so gcn::ImageFont can not be used).
	 *
	 * TextBox::setCaretColumn(), TextBox::setCaretRow(),
	 * TextBox::setCaretRowColumn(), TextBox::getCaretColumn()
	 * and TextBox::getCaretRow() addresses byte offsets of the text.
	 * Use character versions of those methods with UTF8 suffix, they
	 * will correctly access real character positions in UTF-8 texts.
	 * 
	 * @author Przemyslaw Grzywacz
	 */
	class UTF8TextBox: public TextBox {
	public:
		/**
		 * Constructor.
		 * @param text Initial text.
		 */
		UTF8TextBox(const std::string& text = "");

		/**
		 * Destructor.
		 */
		virtual ~UTF8TextBox();

		/**
		 * Key pressed handler.
		 *
		 * Overides gcn::TextField to handle UTF-8 character codes.
		 *
		 * @param keyEvent Keyboard event.
		 */
		virtual void keyPressed(KeyEvent& keyEvent);

		/**
		 * Sets caret column (UTF-8 aware).
		 * @param column Caret column.
		 */
		void setCaretColumnUTF8(int column);

		/**
		 * Sets caret row  (UTF-8 aware).
		 * @param row Caret row
		 */
		void setCaretRowUTF8(int row);

		/**
		 * Sets caret row and column  (UTF-8 aware).
		 * @param row Caret row.
		 * @param column Caret column.
		 */
		void setCaretRowColumnUTF8(int row, int column);

		/**
		 * Gets caret column (UTF-8 aware).
		 * @return Caret column.
		 */
		int getCaretColumnUTF8();

		/**
		 * Gets caret column (UTF-8 aware).
		 * @return Caret row.
		 */
		int getCaretRowUTF8();

	protected:

		/**
		 * UTF-8 string editor suppor.
		 *
		 * It allows the UTF8TextBox to easly traverse UTF-8 strings
		 * as well as inserting and deleting characters.
		 */
		UTF8StringEditor* mStringEditor;
	};

};

#endif // !RAVERON_UTF8TEXTBOX_HPP
