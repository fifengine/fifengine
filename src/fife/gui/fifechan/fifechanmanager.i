// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

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