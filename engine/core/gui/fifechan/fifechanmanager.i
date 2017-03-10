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

%module fife
%{
#include <fifechan.hpp>
#include "gui/fifechan/fifechanmanager.h"
#include "gui/guimanager.h"
%}

%include "gui/guimanager.i"

namespace fcn {
	class Widget;
}
namespace FIFE {
	class Console;
	class Cursor;
	
	class FifechanManager : public IGUIManager {
	public:
		FifechanManager();
		virtual ~FifechanManager();
		
		void init(const std::string& backend, int32_t screenWidth, int32_t	screenHeight);
		
		Console* getConsole() const;
		void setConsoleEnabled(bool console);
		bool isConsoleEnabled() const;

		void setCursor(Cursor* cursor);
		Cursor* getCursor() const;

		void add(fcn::Widget* widget);
		void remove(fcn::Widget* widget);
		
		GuiFont* createFont(const std::string& path, uint32_t size, const std::string& glyphs);
		void releaseFont(GuiFont* font);
		GuiFont* setDefaultFont(const std::string& path, uint32_t size, const std::string& glyphs);
		GuiFont* getDefaultFont();
		
		KeyEvent translateKeyEvent(const fcn::KeyEvent& evt);
		MouseEvent translateMouseEvent(const fcn::MouseEvent& evt);

		void setTabbingEnabled(bool tabbing);
		bool isTabbingEnabled() const;
	private:
		virtual void turn();
		virtual void resizeTopContainer(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		virtual bool onSdlEvent(SDL_Event& evt);
	};
}
