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

#ifndef FIFE_GUICHAN_ADDON_CONSOLE_H
#define FIFE_GUICHAN_ADDON_CONSOLE_H

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <guichan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/time/timer.h"

namespace FIFE {

	class CommandLine;

	/**
	* Console executer is listener interface for console activity
	*/
	class ConsoleExecuter {
		public:
			/** Destructor
			 */
			virtual ~ConsoleExecuter() {}

			/** Called when console tools button is clicked
			 */
			virtual void onToolsClick() = 0;

			/** Called when user has typed command to console and pressed enter
			 * @return response from executer
			 */
			virtual std::string onConsoleCommand(const std::string& command) = 0;
	};


	/** Ingame Console
	 */
	class Console : public gcn::Container, public gcn::ActionListener {
		public:
			/** Constructor
			 */
			Console();

			/** Destructor
			 */
			virtual ~Console();

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

			/** Callback from guichan to respond to button press
			 */
			void action(const gcn::ActionEvent & event);

			/** Sets executer for the console
			 */
			void setConsoleExecuter(ConsoleExecuter* const consoleexec);

			/** Removes executer for the console
			 */
			void removeConsoleExecuter();

		private:

			bool m_isAttached;
			ConsoleExecuter* m_consoleexec;
			
			CommandLine*      m_textfield;
			gcn::TextBox*     m_output;
			gcn::ScrollArea*  m_scrollarea;
			gcn::Label*       m_label;
			gcn::Button*      m_button;
			static const unsigned m_maxOutputRows;

			std::string m_prompt;


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
