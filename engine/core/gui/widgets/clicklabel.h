/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_GUI_WIDGETS_CLICKLABEL_H
#define FIFE_GUI_WIDGETS_CLICKLABEL_H

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <guichan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {
	class GuiFont;
}

namespace gcn {
	
	class ClickLabel : public Widget, public MouseListener, public KeyListener, public FocusListener {
	public:
		ClickLabel();
		ClickLabel(const std::string& caption);
		virtual ~ClickLabel();

		virtual void setWidth(int width);
		virtual void setCaption(const std::string& caption);
		virtual const std::string& getCaption() const;

		virtual void draw(Graphics* graphics);

		virtual void adjustSize();

		void setTextWrapping(bool);
		bool isTextWrapping() const;

		// Inherited from FocusListener
	
		virtual void focusLost(const Event& event);
	
	
		// Inherited from MouseListener
	
		virtual void mousePressed(MouseEvent& mouseEvent);
	
		virtual void mouseReleased(MouseEvent& mouseEvent);
	
		virtual void mouseEntered(MouseEvent& mouseEvent);
	
		virtual void mouseExited(MouseEvent& mouseEvent);
	
		virtual void mouseDragged(MouseEvent& mouseEvent);
	
	
		// Inherited from KeyListener
	
		virtual void keyPressed(KeyEvent& keyEvent);
	
		virtual void keyReleased(KeyEvent& keyEvent);

	protected:
		void wrapText();

		FIFE::GuiFont* mGuiFont;
		bool mTextWrapping;
		std::string mCaption;
		std::string mWrappedText;

		bool mHasMouse;
		bool mKeyPressed;
		bool mMousePressed;
	};
}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
