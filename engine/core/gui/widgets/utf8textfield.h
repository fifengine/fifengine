#ifndef GCN_UTF8TEXTFIELD_HPP
#define GCN_UTF8TEXTFIELD_HPP

#include <guichan/widgets/textfield.hpp>
#include "util/utf8/utf8stringeditor.h"

namespace gcn {

	/**
	 * UTF-8 aware version of the TextField class.
	 *
	 * Using UTF8StringEditor, it is able to correctly edit UTF-8 text line.
	 *
	 * You will also need an UTF-8 aware font to be able to correctly
	 * display such text (so gcn::ImageFont can not be used).
	 * 
	 * @author Przemyslaw Grzywacz
	 */
	class UTF8TextField: public TextField
	{
	public:
		/**
		 * UTF8TextField constructor.
		 * @param text Initial text.
		 */
		UTF8TextField(const std::string& text = "");

		/**
		 * Destructor.
		 */
		virtual ~UTF8TextField();

		/**
		 * Key pressed handler.
		 *
		 * Overides gcn::TextField to handle UTF-8 character codes.
		 *
		 * @param keyEvent Keyboard event.
		 */
		virtual void keyPressed(KeyEvent& keyEvent);

	protected:
		/**
		 * UTF-8 string editor suppor.
		 *
		 * It allows the UTF8TextField to easly traverse UTF-8 strings
		 * as well as inserting and deleting characters.
		 */
		UTF8StringEditor* mStringEditor;
	};


}

#endif // !GCN_UTF8TEXTFIELD_HPP
