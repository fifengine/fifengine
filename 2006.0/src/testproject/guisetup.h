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
#ifndef GUISETUP_H
#define GUISETUP_H

#include "guisetup.h"
#include <guichan.hpp>
#include "fifelabel.hpp"
#include "fifetextbox.hpp"
#include "fifetextfield.hpp"
#include "console.h"

namespace FIFE
{
  class Engine;
  class GUI;
};

/*typdef struct widget_list_struct Widget;
struct widget_list_struct {
	Widget_Type t;
	gcn::Widget* widget = 0;
	Widget* next = 0;
}*/

class GUISetup
{
  public:
    GUISetup();
    virtual ~GUISetup();

    void attach();
    void detach();
		
		bool isAttached();
		
		void setFPSLabelCaption(const std::string& caption);
	FIFE::Console* getConsole() { return m_console; };

  private:
		gcn::FIFELabel* m_fpslabel;
		FIFE::Console* m_console;
		//gcn::FIFETextBox* m_textbox;
		//gcn::FIFETextField* m_textfield;
		bool m_is_attached;

};

#endif
