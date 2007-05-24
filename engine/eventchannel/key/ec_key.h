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
#include "ec_ikey.h"

namespace FIFE {

	/** Represents a key or a character.
	 */
	class Key: public IKey {
	public:
        /** Constructor
		 * @param value value of the key
         */
		Key(int value): m_value(value) {}

        /** Destructor.
         */
		virtual ~Key() {}

		bool isCharacter() const {
			return (m_value >= 32 && m_value <= 126)
				|| (m_value >= 162 && m_value <= 255)
				|| (m_value == 9);
		}

		bool isNumber() const  {
			return m_value >= 48 && m_value <= 57;
		}

		bool isLetter() const  {
			return (((m_value >= 65 && m_value <= 90)
				|| (m_value >= 97 && m_value <= 122)
				|| (m_value >= 192 && m_value <= 255))
				&& (m_value != 215) && (m_value != 247));
		}
		int getValue() const {
			return m_value;
		}

	private:
		int m_value;
	};

} //FIFE

#endif
