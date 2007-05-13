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

// Standard C++ library includes
#include <cassert>

// 3rd party library includes
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "input/inputmanager.h"
#include "script/scriptbackendmanager.h"
#include "video/gui/guimanager.h"
#include "video/screen.h"
#include "video/rendermanager.h"
#include "video/renderbackend.h"
#include "util/log.h"
#include "util/time/timemanager.h"

#include "commandline.h"
#include "console.h"

namespace FIFE {
	const unsigned  Console::m_maxOutputRows = 50;

	class ScriptExecuter : public gcn::ActionListener {
		public:
			ScriptExecuter(const std::string& cmd) 
				: ActionListener() , m_cmd(cmd) {}
			virtual ~ScriptExecuter(){};

			void action(const gcn::ActionEvent & event) {
				try {
					CScriptEngine()->callFunction(m_cmd.c_str());
				} catch(FIFE::Exception & e) {
					Log("Console") << "Caught exception: " << e.getMessage();
				}
			};

			void setCommand(const std::string& cmd) {
				m_cmd = cmd;
			}

		private:
			std::string m_cmd;
	};

	Console::Console() 
		: gcn::Container(),
		m_textfield(new CommandLine()),
		m_output(new gcn::TextBox("")),
		m_scrollarea(new gcn::ScrollArea(m_output)),
		m_label(new gcn::Label()),
		m_button(new gcn::Button("Tools"))
		{
		
		Screen *screen = CRenderBackend()->getMainScreen();
		assert(screen);

		int w, h, b, input_h, bbar_h, button_w;
		w = screen->getWidth()*4/5;
		h = screen->getHeight()*4/5;
		b = 0;
		input_h = getFont()->getHeight();
		bbar_h = input_h;
		button_w = 80;
		
		gcn::Color black(0x00,0,0,0xff);
		gcn::Color white(0xff,0xff,0xff,0xff);
		gcn::Color dark(50,60,50,0xff);

		setSize(w, h);
		setPosition((screen->getWidth() - w) / 2,-h);//(screen->getHeight() - h) / 2);
		setBorderSize(0);

		setForegroundColor(white);
		setBackgroundColor(black);
		setBaseColor(dark);


		setSize(w, h);

		m_scrollarea->setSize(w - 2*b, h - input_h - 3*b - bbar_h);
		m_scrollarea->setPosition(b,0);

		m_textfield->setPosition(b, h - input_h - b - bbar_h);
		m_textfield->setSize(w - 2*b, input_h);

		m_label->setPosition(b, h - b - bbar_h);
		m_label->setSize(w - 2*b, bbar_h);

		m_button->setPosition(w - button_w, h - b - bbar_h);
		m_button->setSize(button_w, bbar_h);

		m_output->setBackgroundColor(black);
		m_output->setFocusable(false);

		m_scrollarea->setBackgroundColor(black);
		m_scrollarea->setBaseColor(dark);

		m_textfield->setForegroundColor(white);
		m_textfield->setBackgroundColor(black);

		m_label->setForegroundColor(white);
		m_label->setBackgroundColor(black);

		m_button->setForegroundColor(white);
		m_button->setBackgroundColor(black);
		m_button->setBaseColor(dark);


		add(m_scrollarea);
		add(m_textfield);
		add(m_label);
		add(m_button);

		setOpaque(true);

		m_textfield->setCallback( std::bind1st( std::mem_fun(&Console::execute), this) );
		m_prompt = "-- ";

		m_isAttached = false;

		m_fpsTimer.setInterval(500);
		m_fpsTimer.setCallback( boost::bind(&Console::updateCaption, this) );

		m_hiddenPos = -h;
		m_hiding = true;
		m_animationDelta = h/6;

		m_animationTimer.setInterval(20);
		m_animationTimer.setCallback( boost::bind(&Console::updateAnimation, this) );

		//NOTE Needs a script binding to it..
		m_scriptexecuter = new ScriptExecuter("on_tools_button_clicked");
		m_button->addActionListener( m_scriptexecuter );

		// Match leading _ or alpha. Then . and 0-9 may occur
		m_single_word_rx = 
			"^\\s*(_(\\d|\\w)*|(\\d|\\w)+)(\\.(_(\\d|\\w)*|(\\d|\\w)+))*\\s*$";
	}

	Console::~Console() {
		doHide();
		
		remove(m_textfield);
		remove(m_scrollarea);
		remove(m_label);

		delete m_output;
		delete m_textfield;
		delete m_scrollarea;
		delete m_label;
		delete m_button;

		delete m_scriptexecuter;
	}

	void Console::updateCaption() {
		std::string caption = "FIFE Console - FPS: ";
		float fps = 1e3/double(TimeManager::instance()->getAverageFrameTime());
		caption += boost::lexical_cast<std::string>(fps);
		m_label->setCaption( caption );
	}

	void Console::updateAnimation() {
		if( m_hiding ) {
			setPosition( getX(), getY() - m_animationDelta);
		} else {
			setPosition( getX(), getY() + m_animationDelta);
		}

		if( !m_hiding && getY() >= 0 ) {
			setPosition( getX(), 0 );
			m_animationTimer.stop();
		}
		if( m_hiding && getY() <= m_hiddenPos ) {
			doHide();
			m_animationTimer.stop();
		}
	}

	void Console::clear() {
		m_output->setText("");
	}

	void Console::doShow() {
		if (m_isAttached)
			return;
		m_isAttached = true;
		GUIManager::instance()->add(this);
		GUIManager::instance()->getTopContainer()->moveToTop(this);
		// Push a context on the context stack of the input manager,
		// so that keys are not propagated to the MapView
		input::Manager::instance()->pushContext("console");
		// Assure the input field is focused when shown.
		m_textfield->requestFocus();

		m_fpsTimer.start();
	}

	void Console::doHide() {
		if (!m_isAttached)
			return;
		m_isAttached = false;
		// Pop the context again, so that keys are propagated again.
		input::Manager::instance()->popContext("console");
		GUIManager::instance()->remove(this);
		m_fpsTimer.stop();
	}

	void Console::show() {
		if(m_hiding) {
			m_hiding = false;
			doShow();
			m_animationTimer.start();
		}
	}

	void Console::hide() {
		if(!m_hiding) {
			m_hiding = true;
			m_animationTimer.start();
		}
	}

	void Console::toggleShowHide() {
		m_hiding = !m_hiding;
		if(!m_hiding)
			doShow();
		m_animationTimer.start();
	}


	void Console::execute(std::string cmd) {
// 		Log("Console") << "EXECUTE";
		if (cmd.empty())
			return;

		boost::regex  regex(m_single_word_rx, boost::regex::perl|boost::regex::icase);
		boost::cmatch match;
		bool single_word = boost::regex_match(cmd.c_str(), match, regex);

		if( single_word ) {
			cmd = std::string("console.print(") + cmd + ")";
		}

		// copy input to output
		println(m_prompt + cmd);

		// run the command
		try {
			CScriptEngine()->runString(cmd.c_str());
		}
		catch (FIFE::Exception & e) {
			Debug("Console") << "Caught exception: " << e.getMessage();
			println(e.getMessage());
		}
	}

	void Console::println(const std::string & s) {
		assert(m_output);

		// Add the text in rows
		boost::char_separator<char> separator("\n");
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		tokenizer tokens(s,separator);
		for(tokenizer::iterator i = tokens.begin(); i != tokens.end(); ++i) {
			m_output->addRow(*i);
		}

		// Assure the maximum number of rows
		if( m_output->getNumberOfRows() > m_maxOutputRows ) {
			unsigned rows = m_output->getNumberOfRows();
			int delta_rows = rows - m_maxOutputRows;
			std::vector<std::string> rows_text;
			for(size_t i=delta_rows; i != rows; ++i) {
				rows_text.push_back(m_output->getTextRow(i));
			}
			m_output->setText("");
			for(size_t i=0; i != rows_text.size(); ++i) {
				m_output->addRow(rows_text[i]);
			}
		}

		// Assure the new text is visible
		gcn::Rectangle rect(0,m_output->getHeight(),0,0);
		m_scrollarea->showWidgetPart(m_output,rect);
	}
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
