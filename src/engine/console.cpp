/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#include "console.h"
#include "log.h"
#include "engine.h"
#include "gui.h"
#include "scriptbase.h"

#include <list>
#include <string>

namespace gcn {

	class ConsoleTextField : public FIFETextField {
		public:
			ConsoleTextField(FIFE::Console* _console) : FIFETextField(""), m_console(_console) {};
			virtual void keyPress(const Key& key);
		private:
			FIFE::Console* m_console;
			std::list<std::string> m_history;
		
	};


	void gcn::ConsoleTextField::keyPress(const Key& key)
	{
		std::string cmd;
		switch( key.getValue() )
		{
			case Key::ENTER:
				cmd = getText();
				if( cmd != "" && ( m_history.empty() || m_history.back() != cmd )){
					m_history.push_back( cmd );
				}
				m_console->runString( cmd );
				setText("");
			break;
			case Key::UP:
				cmd = getText();
				m_history.push_front( cmd );
				setText( m_history.back() );
				m_history.pop_back();
			break;
			case Key::DOWN:
				cmd = getText();
				m_history.push_back( cmd );
				setText( m_history.front() );
				m_history.pop_front();
			break;
	
			default:
				FIFETextField::keyPress(key);
		};
	};

};

namespace FIFE {

	Console::Console() {

		int w = Engine::instance()->screenWidth();
		int h = Engine::instance()->screenHeight();

		// Simple Geometry: 80% width, 75% height, centered on screen
		int con_width = int(w * 0.80);
		int con_height = int(h * 0.75);
		int con_entry_height =  25;

		m_textbox = new gcn::FIFETextBox("");
		m_textfield = new gcn::ConsoleTextField(this);
		m_textfield->setPosition(0,con_height - con_entry_height);
		m_textfield->setSize(con_width,con_entry_height);
		m_textbox->setEditable(false);

		m_scrollarea = new gcn::ScrollArea(m_textbox);
		m_scrollarea->setPosition(0,0);
		m_scrollarea->setSize(con_width,con_height - con_entry_height);

		m_top = new gcn::Container();
		// Center on screen
		m_top ->setPosition( int((w - con_width)/2) , int((h - con_height)/2) );
		m_top->setSize(con_width,con_height);
		m_top ->add( m_scrollarea );
		m_top ->add( m_textfield );
		

		m_textbox->setBaseColor(gcn::Color(255,0,0,255));
		m_textbox->setForegroundColor(gcn::Color(0,255,0,225));
		m_textbox->setBackgroundColor(gcn::Color(0,0,0,225));

		m_scrollarea->setBaseColor(gcn::Color(255,255,255,255));
		m_scrollarea->setForegroundColor(gcn::Color(0,0,0,225));
		m_scrollarea->setBackgroundColor(gcn::Color(0,0,0,225));

		m_textfield->setBaseColor(gcn::Color(255,0,0,255));
		m_textfield->setForegroundColor(gcn::Color(255,255,255,255));
		m_textfield->setBackgroundColor(gcn::Color(0,0,0,255));
/*
		const char* repr_def = 
			"function repr ( val ) "
				"if val == nil then "
					"return \"\" "
				"else "
					"return tostring(val) "
				"end "
			"end";

		try {
			ScriptEngine::instance()->runString( repr_def );
			ScriptEngine::instance()->runString( "_=nil" );
		} catch ( ScriptException& e ) {
			Log("console") << e.getMessage();
		};
*/
	};

	Console::~Console() {
		Log::setConsole(0);
		detach();
		delete m_textbox;
		delete m_textfield;
		delete m_scrollarea;
		delete m_top;
	};

	void Console::print(const std::string& str) {
		std::string s = m_textbox->getText() + str;
		m_textbox->setText( s );
	};

	void Console::runString(const std::string& cmd) {
/*
		std::string post_cmd = "_as_str = repr(_)";
		std::string post_cmd_alt = "_as_str = ''";
		std::string tmpcmd;
		unsigned pos = cmd.find("=");
		bool assignment = (pos != std::string::npos);
		if( !assignment ) { // no assignment
			tmpcmd = std::string("_ = ") + cmd; 
		} else { // with assignment
			tmpcmd = cmd;
		};
*/
		try {
			ScriptEngine::instance()->runString(cmd.c_str());
/*
			if( !assignment )
				ScriptEngine::instance()->runString(post_cmd.c_str());
			else 
				ScriptEngine::instance()->runString(post_cmd_alt.c_str());
*/
		} catch( ScriptException& e ) {
			Log("console") << e.getMessage();
		} catch( ... ) {
			Log("console") << "unknown exception in scriptengine ... runstring: " << cmd;
		};
/*
		std::string retval = "";
		if( !assignment )
			retval = ScriptEngine::instance()->getGlobalString("_as_str");
*/
		print(std::string(">") + cmd +  "\n");
/*
		if( retval != "" )
			print( retval + "\n" );
*/		
	};

	void Console::attach() {
		Widget::attach();
		// Make sure the textfield is focused.
		m_textfield->requestFocus();
	}
		
	gcn::Widget* Console::topWidget() {
		return m_top;
	};

}//FIFE

