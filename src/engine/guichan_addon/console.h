#ifndef FIFE_GUICHAN_CONSOLE_H
#define FIFE_GUICHAN_CONSOLE_H
#include <string>
#include "guichan.hpp"

namespace FIFE {

	using namespace gcn;
	/** Ingame Console
	 *
	 * Activated with F10.
	 */
	class Console : private gcn::TextField {
		public:
			/** Constructor
			 */
			Console();

			/** Destructor
			 */
			~Console();
			void keyPress(const gcn::Key& key);
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

			void clear();

			/** Toggle the console
			 * Toggles whether the Console is shown or not.
			 * Calls show() or hide() respectively.
			 */
			void toggleShowHide();

			void adjustSize();
		private:
			void consoleAction();
			bool isAttached;
			
			gcn::Container*  m_container;
			gcn::TextBox*    m_output;
			gcn::ScrollArea* m_scrollarea;

	};
}
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
