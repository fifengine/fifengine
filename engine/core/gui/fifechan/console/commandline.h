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

#ifndef FIFE_FIFECHAN_ADDON_COMMANDLINE_H
#define FIFE_FIFECHAN_ADDON_COMMANDLINE_H

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes
#include <boost/function.hpp>
#include <fifechan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/time/timer.h"

namespace FIFE {

	/** A Command line widget
	 */
	class CommandLine : public fcn::TextField {
		public:
			typedef boost::function1<void,std::string> type_callback;

			/** Constructor
			 */
			CommandLine();

			/** Destructor
			 */
			~CommandLine();

			void keyPressed(fcn::KeyEvent& keyEvent);
			virtual void drawCaret(fcn::Graphics * graphics, int32_t x);

			/** Set callback on pressing the ENTER key
			 */
			void setCallback(const type_callback& cb);

			/** Toggle the caret visibility
			 */
			void toggleCaretVisible();

			/** Start blinking the caret
			 */
			void startBlinking();

			/** Stop blinking the caret for a few seconds
			 */
			void stopBlinking();
		private:
			type_callback m_callback;
			std::vector<std::string> m_history;
			size_t m_history_position;
			std::string m_cmdline;
			bool m_caretVisible;
			Timer m_blinkTimer;
			Timer m_suppressBlinkTimer;
	};
}
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
