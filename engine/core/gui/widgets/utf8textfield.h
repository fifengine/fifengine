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

#ifndef GCN_UTF8TEXTFIELD_HPP
#define GCN_UTF8TEXTFIELD_HPP

// Standard C++ library includes

// 3rd party library includes
#include <guichan/widgets/textfield.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
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
