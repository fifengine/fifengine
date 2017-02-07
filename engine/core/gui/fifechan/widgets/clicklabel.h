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

#ifndef FIFE_GUI_WIDGETS_CLICKLABEL_H
#define FIFE_GUI_WIDGETS_CLICKLABEL_H

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <fifechan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {
	class GuiFont;
}

namespace fcn {
	
	/**
	 * Implementation of a label capable of displaying a single or multiline caption.
	 *
	 * In case text wrapping is enabled, the labels width will be the same as the width from the parent.
	 * Or as fallback the maximal size parameter is used.
	 * If a label is clicked an action event will be sent to all action listener's of the label.
	 */
	class ClickLabel : public Widget, public MouseListener, public KeyListener, public FocusListener, public WidgetListener {
	public:
		/**
		 * Constructor.
		 */
		ClickLabel();

		/**
		 * Constructor. The label will be automatically resized
		 * to fit the caption.
		 *
		 * @param caption The caption of the label.
		 */
		ClickLabel(const std::string& caption);

		virtual ~ClickLabel();
		
		/**
		 * Gets the caption of the label.
		 *
		 * @return The caption of the label.
		 * @see setCaption
		 */
		virtual const std::string& getCaption() const;

		/**
		 * Sets the caption of the label.
		 *
		 * @param caption The caption of the label.
		 * @see getCaption, adjustSize
		 */
		virtual void setCaption(const std::string& caption);

		/**
		 * Sets the alignment of the caption. The alignment is relative
		 * to the center of the label.
		 *
		 * @param alignemnt The alignment of the caption of the label.
		 * @see getAlignment, Graphics
		 */
		virtual void setAlignment(Graphics::Alignment alignment);

		/**
		 * Gets the alignment of the caption. The alignment is relative to
		 * the center of the label.
		 *
		 * @return The alignment of caption of the label.
		 * @see setAlignmentm Graphics
		 */
		virtual Graphics::Alignment getAlignment() const;

		/**
		 * Sets the opacity of the background.
		 * 
		 * @param opaque True if opaque, false otherwise.
		 */
		virtual void setOpaque(bool opaque);

		/**
		 * @return Whether this background is opaque or not.
		 */
		virtual bool isOpaque() const;

		/**
		 * Sets the text wrapping of the caption. The text will be splitted
		 * to the width of the label if enabled.
		 *
		 * @param textWrapping True if text wrapping is enabled, false otherwise.
		 */
		virtual void setTextWrapping(bool textWrapping);

		/**
		 * Gets the text wrapping of the caption. The text will be splitted
		 * to the width of the label if it returns true.
		 *
		 * @return True if text wrapping is enabled, false otherwise.
		 */
		virtual bool isTextWrapping() const;


		// Inherited from Widget

		virtual void setDimension(const Rectangle& dimension);
		virtual void resizeToContent(bool recursiv=true);
		virtual void adjustSize();
		virtual void draw(Graphics* graphics);
		virtual void fontChanged();


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


		// Inherited from WidgetListener

		virtual void ancestorHidden(const Event& event);

	protected:
		virtual void wrapText();

		/**
		 * Holds the gui font.
		 */
		FIFE::GuiFont* mGuiFont;

		/**
		 * Holds the caption of the label.
		 */
		std::string mCaption;

		/**
		 * Holds the wrapped text of the label.
		 */
		std::string mWrappedText;

		/**
		 * Holds the alignment of the caption.
		 */
		Graphics::Alignment mAlignment;
		
		/**
		 * True if opaque, otherwise false.
		 */
		bool mOpaque;

		/**
		 * True if text wrapping is enabled, otherwise false.
		 */
		bool mTextWrapping;

		/**
		 * True if the mouse is ontop of the button, false otherwise.
		 */
		bool mHasMouse;

		/**
		 * True if a key has been pressed, false otherwise.
		 */
		bool mKeyPressed;

		/**
		 * True if a mouse has been pressed, false otherwise.
		 */
		bool mMousePressed;
	};
}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
