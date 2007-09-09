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

#ifndef FIFE_CONSOLE_H
#define FIFE_CONSOLE_H

#include <string>
#include <guichan.hpp>
#include "gui_widget.h"

namespace FIFE {

	class Console : public Widget {

		public:
			Console();
			virtual ~Console();
			void print(const std::string& str);
			virtual void runString(const std::string& cmd);

			virtual void attach();
			virtual gcn::Widget* topWidget();
		private:
			GUI* m_gui;
			gcn::FIFETextBox* m_textbox;
			gcn::FIFETextField* m_textfield;
			gcn::Container* m_top;
			gcn::ScrollArea* m_scrollarea;	
	};
}//FIFE

#endif
