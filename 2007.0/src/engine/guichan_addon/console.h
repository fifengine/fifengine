/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#ifndef FIFE_GUICHAN_CONSOLE_H
#define FIFE_GUICHAN_CONSOLE_H

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <guichan.hpp>

// FIFE includes
#include "timer.h"

namespace FIFE {

	class CommandLine;
	class ScriptExecuter;

	/** Ingame Console
	 *
	 * Activated with F10.
	 */
	class Console : public gcn::Container {
		public:
			/** Constructor
			 */
			Console();

			/** Destructor
			 */
			~Console();

			/** Print one or more lines to the console output
			 */
			void println(const std::string & s);

			/** Show the console
			 * Adds the Console to the guichan toplevel container
			 * and pushes an input Context so that keys are not send to the
			 * rest of the game.
			 */
			void show();

			/** Hide the console
			 * Removes itself from the toplevel container
			 * and pops it's input context
			 */
			void hide();

			/** Clear the console output
			 */
			void clear();

			/** Toggle the console
			 * Toggles whether the Console is shown or not.
			 * Calls show() or hide() respectively.
			 */
			void toggleShowHide();

			/** Execute a command
			 * Normally just sends the command to runString()
			 * Checks whether the cmd is just one token
			 * and print it's value rather than throw an
			 * useless error.
			 *
			 * @todo generalize the generated command and the regexp used.
			 */
			void execute(std::string cmd);

			/** Update the FPS caption
			 *  @note Is a timer callback.
			 */
			void updateCaption();

			/** Update the scroll in/out animation.
			 *  @note Is a timer callback.
			 */
			void updateAnimation();
		private:

			bool m_isAttached;
			
			CommandLine*      m_textfield;
			gcn::TextBox*     m_output;
			gcn::ScrollArea*  m_scrollarea;
			gcn::Label*       m_label;
			gcn::Button*      m_button;
			static const unsigned m_maxOutputRows;

			ScriptExecuter* m_scriptexecuter;

			std::string m_prompt;
			std::string m_single_word_rx;


			int m_hiddenPos;
			int m_animationDelta;

			bool m_hiding;

			Timer m_fpsTimer;
			Timer m_animationTimer;

			void doShow();
			void doHide();
	};
}
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
