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
#include "util/log/logger.h"
#include "util/base/exception.h"
#include "gui/fifechan/fifechanmanager.h"
#include "gui/fifechan/base/gui_font.h"

#include "commandline.h"
#include "console.h"

namespace FIFE {
	const unsigned  Console::m_maxOutputRows = 50;
	static Logger _log(LM_CONSOLE);

	Console::Console()
		: fcn::Container(),
		m_consoleexec(0),
		m_input(new CommandLine()),
		m_output(new fcn::TextBox()),
		m_outputscrollarea(new fcn::ScrollArea(m_output)),
		m_status(new fcn::Label()),
		m_toolsbutton(new fcn::Button("Tools"))
		{
		reLayout();

		add(m_outputscrollarea);
		add(m_input);
		add(m_status);
		add(m_toolsbutton);

		setOpaque(true);

		m_input->setCallback( std::bind1st( std::mem_fun(&Console::execute), this) );
		m_prompt = "-- ";

		m_isAttached = false;

		m_fpsTimer.setInterval(500);
		m_fpsTimer.setCallback( boost::bind(&Console::updateCaption, this) );

		m_hiding = true;

		m_animationTimer.setInterval(20);
		m_animationTimer.setCallback( boost::bind(&Console::updateAnimation, this) );

		m_toolsbutton->addActionListener(this);
		m_toolsbutton->setFocusable(false);
		m_input->addFocusListener(this);

		GuiFont* font = FifechanManager::instance()->createFont();
		font->setColor(255,255,255);
		setIOFont(font);
	}

	void Console::reLayout() {
		int32_t w, h, b, input_h, bbar_h, button_w;
		w = RenderBackend::instance()->getScreenWidth() * 4/5;
		h = RenderBackend::instance()->getScreenHeight() * 4/5;
		b = 0;
		input_h = getFont()->getHeight();
		bbar_h = input_h;
		button_w = 80;

		fcn::Color black(0x00,0,0,0xff);
		fcn::Color white(0xff,0xff,0xff,0xff);
		fcn::Color dark(50,60,50,0xff);

		setSize(w, h);
		setPosition((RenderBackend::instance()->getScreenWidth() - w) / 2,-h);
		setBorderSize(0);

		setForegroundColor(white);
		setBackgroundColor(black);
		setBaseColor(dark);

		setSize(w, h);

		m_outputscrollarea->setSize(w - 2*b, h - input_h - 3*b - bbar_h);
		m_outputscrollarea->setPosition(b,0);

		m_input->setPosition(b, h - input_h - b - bbar_h);
		m_input->setSize(w - 2*b, input_h);

		m_status->setPosition(b, h - b - bbar_h);
		m_status->setSize(w - 2*b, bbar_h);

		m_toolsbutton->setPosition(w - button_w, h - b - bbar_h);
		m_toolsbutton->setSize(button_w, bbar_h);

		m_output->setBackgroundColor(black);
		m_output->setFocusable(false);

		m_outputscrollarea->setBackgroundColor(black);
		m_outputscrollarea->setBaseColor(dark);

		m_input->setForegroundColor(white);
		m_input->setBackgroundColor(black);

		m_status->setForegroundColor(white);
		m_status->setBackgroundColor(black);

		m_toolsbutton->setForegroundColor(white);
		m_toolsbutton->setBackgroundColor(black);
		m_toolsbutton->setBaseColor(dark);

		m_hiddenPos = -h;
		m_animationDelta = h/6;
	}

	Console::~Console() {
		doHide();

		remove(m_input);
		remove(m_outputscrollarea);
		remove(m_status);

		delete m_output;
		delete m_input;
		delete m_outputscrollarea;
		delete m_status;
		delete m_toolsbutton;
	}

	void Console::updateCaption() {
		std::string caption = "FIFE Console - FPS: ";
		double fps = 1e3/TimeManager::instance()->getAverageFrameTime();
		caption += boost::lexical_cast<std::string>(fps);
		m_status->setCaption( caption );
	}

	void Console::updateAnimation() {
	    if (m_hiding){
		setPosition(getX(), getY() - m_animationDelta);
		if (getY() <= m_hiddenPos){
		    doHide();
			m_animationTimer.stop();
		}
	    }else{
		setPosition(getX(), getY() + m_animationDelta);
		if (getY() >= 0){
		    setPosition(getX(), 0);
			m_animationTimer.stop();
		}
	}
	}

	void Console::clear() {
		m_output->setText("");
	}

	void Console::doShow() {
		if (m_isAttached)
			return;
		m_isAttached = true;
		FifechanManager::instance()->add(this);
		FifechanManager::instance()->getTopContainer()->moveToTop(this);
		// Assure the input field is focused when shown.
		m_input->requestFocus();

		m_fpsTimer.start();
	}

	void Console::doHide() {
		if (!m_isAttached)
			return;
		m_isAttached = false;
		FifechanManager::instance()->remove(this);
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
		catch (const FIFE::Exception & e) {
			FL_WARN(_log, LMsg("Console caught exception: ") << e.what());
			println(e.what());
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
			int32_t delta_rows = rows - m_maxOutputRows;
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
		fcn::Rectangle rect(0,m_output->getHeight(),0,0);
		m_outputscrollarea->showWidgetPart(m_output,rect);
	}

	void Console::action(const fcn::ActionEvent & event) {
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

	void Console::setIOFont(GuiFont* font) {
		m_input->setFont(font);
		m_output->setFont(font);
	}

	void Console::focusLost(const fcn::Event& ) {
		hide();
	}
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
