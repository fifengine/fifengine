#include <cassert>
#include "console.h"
#include "log.h"
#include "scriptbase.h"
#include "video/gui/guimanager.h"
#include "video/screen.h"
#include "video/rendermanager.h"
#include "video/renderbackend.h"

#include "inputmanager.h"

namespace FIFE {

	Console::Console() : gcn::TextField(""), 
											 m_container(new gcn::Container()),
											 m_output(new gcn::TextBox("-- welcome to the console")),
											 m_scrollarea(new gcn::ScrollArea(m_output)) {
		
		Screen *screen = RenderManager::instance()->getCurrentBackend()->getMainScreen();
		assert(screen);

		int w, h;
		w = screen->getWidth()*4/5;
		h = screen->getHeight()*4/5;
		

		m_container->setSize(w, h);

		m_container->setPosition((screen->getWidth() - w) / 2,
				(screen->getHeight() - h) / 2);

		m_scrollarea->setSize(w - 30, h - 100);
		m_scrollarea->setPosition(15, 50);

		this->setPosition(15, 5);
		this->setSize(w - 30, 40);

		gcn::Color dark(0x11,0x11,0x33,0xff);

		m_output->setBackgroundColor(dark);
		m_scrollarea->setBackgroundColor(dark);
		this->setBackgroundColor(dark);

		m_container->add(m_scrollarea);
		m_container->add(this);
		m_container->setOpaque(true);

		isAttached = false;
	}

	Console::~Console() {
		assert(m_container && m_output);

		hide();
		
		m_container->remove(this);
		m_container->remove(m_scrollarea);

		delete m_output;
		delete m_container;
		delete m_scrollarea;
		
	}

	void Console::adjustSize() {
	}

	void Console::clear() {
		m_output->setText("");
	}

	void Console::show() {
		if (isAttached)
			return;
		isAttached = true;
		GUIManager::instance()->add(m_container);
		GUIManager::instance()->getTopContainer()->moveToTop(m_container);
		// Push a context on the context stack of the input manager,
		// so that keys are not propagated to the MapView
		InputManager::instance()->pushContext("console");
		// Assure the input field is focused when shown.
		requestFocus();
	}

	void Console::hide() {
		if (!isAttached)
			return;
		isAttached = false;
		// Pop the context again, so that keys are propagated again.
		InputManager::instance()->popContext("console");
		GUIManager::instance()->remove(m_container);
	}

	void Console::toggleShowHide() {
		if (isAttached)
			hide();
		else
			show();
	}

	// directly copied from gcn::textfield with one change
	void Console::keyPress(const gcn::Key & key) {
		//Log("DEBUG-key") << key.getValue();
		if (key.getValue() == Key::LEFT && mCaretPosition > 0)
		{
			--mCaretPosition;
		}
		else if (key.getValue() == Key::RIGHT && mCaretPosition < mText.size())
		{
			++mCaretPosition;
		}
		else if (key.getValue() == Key::DELETE && mCaretPosition < mText.size())
		{
			mText.erase(mCaretPosition, 1);
		}
		else if (key.getValue() == Key::BACKSPACE && mCaretPosition > 0)
		{
			mText.erase(mCaretPosition - 1, 1);
			--mCaretPosition;
		}
		else if (key.getValue() == Key::ENTER)
		{
			// generateAction();
			consoleAction();
		}
		else if (key.getValue() == Key::HOME)
		{
			mCaretPosition = 0;
		}    
		else if (key.getValue() == Key::END)
		{
			mCaretPosition = mText.size();
		}    
		else if (key.isCharacter())
		{
			mText.insert(mCaretPosition, std::string(1,char(key.getValue())));
			++mCaretPosition;
		}
		fixScroll();    
	}

	void Console::consoleAction() {
		Log("Console") << "EXECUTE";
		if (!mText.size())
			return;

		// copy input to output
		println(mText);

		// run the command
		try {
			ScriptEngine::instance()->runString(mText.c_str());
		}
		catch (FIFE::Exception & e) {
			Log("Console") << "Caught exception: " << e.getMessage();
			println(e.getMessage());
		}
		// clear the input field
		setText("");
		
	}

	void Console::println(const std::string & s) {
		assert(m_output);
		m_output->addRow(s);
	}
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
