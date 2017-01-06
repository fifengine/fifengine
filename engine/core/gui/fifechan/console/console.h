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

#ifndef FIFE_FIFECHAN_ADDON_CONSOLE_H
#define FIFE_FIFECHAN_ADDON_CONSOLE_H

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <fifechan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/time/timer.h"

namespace FIFE {

	class CommandLine;
	class GuiFont;

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
	class Console : public fcn::Container, public fcn::ActionListener, public fcn::FocusListener {
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
			 * Adds the Console to the fifechan toplevel container
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

			/** Callback from fifechan to respond to button press
			 */
			void action(const fcn::ActionEvent & event);

			/** Sets executer for the console
			 */
			void setConsoleExecuter(ConsoleExecuter* const consoleexec);

			/** Removes executer for the console
			 */
			void removeConsoleExecuter();

			/** Layouts the console to match e.g. font changes
			 */
			void reLayout();

			/** Sets the font used for the input and output areas
			 */
			void setIOFont(GuiFont* font);

			/** Hide if we loose focus
			*/
			void focusLost(const fcn::Event& event);
		private:

			bool m_isAttached;
			ConsoleExecuter* m_consoleexec;
			
			CommandLine*      m_input;
			fcn::TextBox*     m_output;
			fcn::ScrollArea*  m_outputscrollarea;
			fcn::Label*       m_status;
			fcn::Button*      m_toolsbutton;
			static const unsigned m_maxOutputRows;

			std::string m_prompt;

			int32_t m_hiddenPos;
			int32_t m_animationDelta;

			bool m_hiding;

			Timer m_fpsTimer;
			Timer m_animationTimer;

			void doShow();
			void doHide();
	};
}
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
