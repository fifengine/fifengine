/***************************************************************************
 *   Copyright (C) 2005 by the FIFE Team                                   *
 *   fife@developer1.cvsdude.com                                           *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "guisetup.h"
#include "gui.h"
#include "engine.h"
#include "exception.h"
#include "console.h"
#include <guichan.hpp>
#include "log.h"

GUISetup::GUISetup() : m_fpslabel(0)
{
	m_is_attached = false;

	try
	{
		m_fpslabel = new gcn::FIFELabel("FPS: 0.000");
		m_fpslabel->setPosition(10,10);
	}
	catch (const FIFE::Exception&)
	{
	}
	m_console = new FIFE::Console();
	//console->attach();

	/*m_textbox = new gcn::FIFETextBox("Editable TextBox\nwhich supports\nMultiple lines");
	m_textbox->setEditable(true);
	m_textbox->setPosition(10,40);
	m_textbox->setSize(300,100);
	m_textbox->setBaseColor(gcn::Color(255,0,0,255));
	m_textbox->setForegroundColor(gcn::Color(0,255,0,255));
	m_textbox->setBackgroundColor(gcn::Color(0,0,255,255));
	m_textfield = new gcn::FIFETextField("Editable TextField");
	m_textfield->setPosition(10,170);
	m_textfield->setSize(300,20);
	m_textfield->setBaseColor(gcn::Color(255,0,0,255));
	m_textfield->setForegroundColor(gcn::Color(0,255,0,255));
	m_textfield->setBackgroundColor(gcn::Color(0,0,255,255));*/
 }

GUISetup::~GUISetup()
{
  if (m_fpslabel) delete m_fpslabel;
}

void GUISetup::attach()
{
	if (m_fpslabel && !m_is_attached) {
		//FIFE::GUI* gui = FIFE::Engine::instance()->getGUI();
		//gui->add(m_fpslabel);
		m_console->attach();
		FIFE::Log::setConsole(m_console);
		//gui->add(m_textbox);
		//gui->add(m_textfield);
		m_is_attached = true;
		FIFE::Log::setConsole(m_console);
	}
}

void GUISetup::detach()
{
	if (m_is_attached) {
		//FIFE::GUI* gui = FIFE::Engine::instance()->getGUI();
		//gui->remove(m_fpslabel);
		m_console->detach();
		//gui->remove(m_textbox);
		//gui->remove(m_textfield);
		m_is_attached = false;
		FIFE::Log::setConsole(0);
	}
}

bool GUISetup::isAttached()
{
	return m_is_attached;
}

void GUISetup::setFPSLabelCaption(const std::string& caption)
{
	if (m_fpslabel)
		m_fpslabel->setCaption(caption);
}
