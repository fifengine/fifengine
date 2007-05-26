/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#ifndef FIFE_EVENTCHANNEL_IKEY_H
#define FIFE_EVENTCHANNEL_IKEY_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//

namespace FIFE {

	/** Represents a key or a character.
	 */
	class IKey {
	public:
		/** Checks whether a key is a character.
		 */
		virtual bool isCharacter() const = 0;

		/** Gets the value of the key. 
		 */
		virtual bool isNumber() const = 0;

		/** Checks whether a key is a letter. 
		 */
		virtual bool isLetter() const = 0;

		/** Gets the value of the key. 
		 */
		virtual int getValue() const = 0;

		/** Gets the string representation of the key. 
		 */
		virtual std::string getAsString() const = 0;


		enum KeyType {
			INVALID_KEY		   = -1,
			SPACE              = ' ',
			TAB                = '\t',
			ENTER              = '\n',
			LEFT_ALT           = 1000,
			RIGHT_ALT,
			LEFT_SHIFT,
			RIGHT_SHIFT,
			LEFT_CONTROL,
			RIGHT_CONTROL,
			LEFT_META,
			RIGHT_META,
			LEFT_SUPER,
			RIGHT_SUPER,
			INSERT,
			HOME,
			PAGE_UP,
			DELETE,
			END,
			PAGE_DOWN,
			ESCAPE,
			CAPS_LOCK,
			BACKSPACE,
			F1,
			F2,
			F3,
			F4,
			F5,
			F6,
			F7,
			F8,
			F9,
			F10,
			F11,
			F12,
			F13,
			F14,
			F15,
			PRINT_SCREEN,
			SCROLL_LOCK,
			PAUSE,
			NUM_LOCK,
			ALT_GR,
			LEFT,
			RIGHT,
			UP,
			DOWN
		};
		virtual ~IKey() {}
	};

} //FIFE

#endif
