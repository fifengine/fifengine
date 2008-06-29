/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

%module fife
%{
#include <guichan.hpp>
#include "gui/guimanager.h"
%}

namespace gcn {
	class Widget;
	class ActionEvent;
	class ActionListener {
	public:
		virtual void action(const ActionEvent& actionEvent) = 0;
	};
}
namespace FIFE {
	class Console;
	
	%feature("notabstract") GUIManager;
	class GUIManager: public gcn::ActionListener {
	public:
		Console* getConsole();
		void add(gcn::Widget* widget);
		void remove(gcn::Widget* widget);
		GuiFont* createFont(const std::string& path, unsigned int size, const std::string& glyphs);
		void releaseFont(GuiFont* font);
		
	private:
		GUIManager(IWidgetListener* widgetListener);
	};
}
