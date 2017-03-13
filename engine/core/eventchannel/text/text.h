/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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

#ifndef FIFE_EVENTCHANNEL_TEXT_H
#define FIFE_EVENTCHANNEL_TEXT_H

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

	/** Represents a text.
	 */
	class Text {
	public:

		/** Constructor
		 * @param text pointer to the UTF-8 text
		 * @param start point of the text edit event
		 * @param length length of the text edit event
		*/
		Text(const char* text = 0, int32_t start = 0, int32_t length = 0):
			m_text(text ? text : ""),
			m_start(start),
			m_length(length) {}

		/** Destructor
		 */
		virtual ~Text() {}

		/** Tells whether the Text can be represented as a string.
		 */
		bool isPrintable() const {
			if (!m_text.empty()) {
				return true;
			}
			return false;
		}

		/** Gets the string, encoded as UTF-8.
		 */
		std::string getAsString() const {
			return m_text;
		}

		/** Gets the start point of the text edit event, cursor position.
		 */
		int32_t getStart() const {
			return m_start;
		}

		/** Gets the length of the text edit event,
		 * the number of characters to edit from the start point.
		 */
		int32_t getLength() const {
			return m_length;
		}

	private:
		std::string m_text;
		int32_t m_start;
		int32_t m_length;
	};

} //FIFE

#endif
