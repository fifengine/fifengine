/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_EVENTCHANNEL_KEY_H
#define FIFE_EVENTCHANNEL_KEY_H

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
	class Key {
	public:
		/** Constructor
		 * @param value value of the key
		 * @param repr string represntation of the key
		*/
		Key(int value, const std::string& repr=""): 
			m_value(value),
			m_repr(repr) {}

		/** Destructor.
		 */
		virtual ~Key() {}

		/** Checks whether a key is a character.
		 */
		bool isCharacter() const {
			return (m_value >= 32 && m_value <= 126)
				|| (m_value >= 162 && m_value <= 255)
				|| (m_value == 9);
		}

		/** Gets the value of the key. 
		 */
		bool isNumber() const  {
			return m_value >= 48 && m_value <= 57;
		}

		/** Checks whether a key is a letter. 
		 */
		bool isLetter() const  {
			return (((m_value >= 65 && m_value <= 90)
				|| (m_value >= 97 && m_value <= 122)
				|| (m_value >= 192 && m_value <= 255))
				&& (m_value != 215) && (m_value != 247));
		}
		
		/** Gets the value of the key. 
		 */
		int getValue() const {
			return m_value;
		}

		/** Gets the string representation of the key. 
		 */
		const std::string& getAsString() const {
			return m_repr;
		}

		/** Codes for different keys
		 */
		enum KeyType {
			INVALID_KEY        = -1,
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
			DELETE_KEY,
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

	private:
		int m_value;
		std::string m_repr;
	};

} //FIFE

#endif
