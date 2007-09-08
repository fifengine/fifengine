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
#include "video/renderbackend.h"
#include "util/time/timemanager.h"
#include "util/logger.h"
#include "util/exception.h"
#include "gui/guimanager.h"

#include "commandline.h"
#include "console.h"

namespace FIFE {
	const unsigned  Console::m_maxOutputRows = 50;
	static Logger _log(LM_CONSOLE);

	Console::Console() 
		: gcn::Container(),
		m_consoleexec(0),
		m_textfield(new CommandLine()),
		m_output(new gcn::TextBox("")),
		m_scrollarea(new gcn::ScrollArea(m_output)),
		m_label(new gcn::Label()),
		m_button(new gcn::Button("Tools"))
		{
		
		SDL_Surface *screen = RenderBackend::instance()->getScreenSurface();
		assert(screen);

		int w, h, b, input_h, bbar_h, button_w;
		w = screen->w*4/5;
		h = screen->h*4/5;
		b = 0;
		input_h = getFont()->getHeight();
		bbar_h = input_h;
		button_w = 80;
		
		gcn::Color black(0x00,0,0,0xff);
		gcn::Color white(0xff,0xff,0xff,0xff);
		gcn::Color dark(50,60,50,0xff);

		setSize(w, h);
		setPosition((screen->w - w) / 2,-h);
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

		m_button->addActionListener(this);
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
		// Assure the input field is focused when shown.
		m_textfield->requestFocus();

		m_fpsTimer.start();
	}

	void Console::doHide() {
		if (!m_isAttached)
			return;
		m_isAttached = false;
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
 		FL_DBG(_log, LMsg("in execute with command ") << cmd);
		if (cmd.empty())
			return;

		// copy input to output
		println(m_prompt + cmd);

		// run the command
		try {
			if (m_consoleexec) {
				std::string resp = m_consoleexec->onConsoleCommand(cmd);
				println(resp);
			} else {
				FL_WARN(_log, LMsg("ConsoleExecuter not bind, but command received: ") << cmd.c_str());
			}
		}
		catch (FIFE::Exception & e) {
			FL_WARN(_log, LMsg("Console caught exception: ") << e.getMessage());
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

	void Console::action(const gcn::ActionEvent & event) {
		if (m_consoleexec) {
			m_consoleexec->onToolsClick();
		} else {
			FL_WARN(_log, "ConsoleExecuter not bind, but tools button clicked");
		}
	}

	void Console::setConsoleExecuter(ConsoleExecuter* const consoleexec) {
		m_consoleexec = consoleexec;
	}

	void Console::removeConsoleExecuter() {
		m_consoleexec = NULL;
	}
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
