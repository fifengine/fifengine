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

// Standard C++ library includes
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "gui/base/gui_font.h" 
#include "util/base/exception.h"
#include "video/image.h" 

#include "clicklabel.h"

namespace gcn {
	ClickLabel::ClickLabel()  {
		mGuiFont = static_cast<FIFE::GuiFont*> (getFont());
// 		setAlignment(Graphics::LEFT);
		setTextWrapping(false);
		setFrameSize(0);
		addMouseListener(this);
		addKeyListener(this);
		addFocusListener(this);

	}
	
	ClickLabel::ClickLabel(const std::string& caption) {
		mGuiFont = static_cast<FIFE::GuiFont*> (getFont());
// 		setAlignment(Graphics::LEFT);
		setTextWrapping(false);
		setCaption(caption);
		setFrameSize(0);
		addMouseListener(this);
		addKeyListener(this);
		addFocusListener(this);

		wrapText();
	}
	
	ClickLabel::~ClickLabel() {
	}
	
	void ClickLabel::setCaption(const std::string& caption) {
		mCaption = caption;
		mGuiFont = static_cast<FIFE::GuiFont*> (getFont());
		wrapText();
	}

	const std::string& ClickLabel::getCaption() const {
		return mCaption;
	}

	void ClickLabel::setWidth(int width) {
		Widget::setWidth(width);
		wrapText();
	}
	
	void ClickLabel::draw(Graphics* graphics) {

		if (mGuiFont != static_cast<FIFE::GuiFont*> (getFont())) {
			mGuiFont = static_cast<FIFE::GuiFont*> (getFont());
			wrapText();
			adjustSize();
		}

		int textX = 0;
		int textY = 0;

		graphics->setColor(getBackgroundColor());
		graphics->fillRectangle(Rectangle(1, 1, getDimension().width-1, getHeight() - 1));
		if (mGuiFont) {
			if( isTextWrapping() ) {
				mGuiFont->drawMultiLineString(graphics, mWrappedText, textX, textY);
			} else {
				mGuiFont->drawMultiLineString(graphics, mCaption, textX, textY);
			}
		}
	}

	void ClickLabel::setTextWrapping(bool textWrapping) {
		bool wrappingEnabled = !mTextWrapping && textWrapping;
		mTextWrapping = textWrapping;
		if (wrappingEnabled) {
			wrapText();
		}
	}

	bool ClickLabel::isTextWrapping() const {
		return mTextWrapping;
	}

	void  ClickLabel::adjustSize() {
		if (mGuiFont) {
			FIFE::Image* image;
			if( isTextWrapping() ) {
				image = mGuiFont->getAsImageMultiline(mWrappedText);
			} else {
				image = mGuiFont->getAsImageMultiline(mCaption);
			}
			setWidth( image->getWidth() );
			setHeight( image->getHeight() );
		}
	}

	void ClickLabel::wrapText() {
		if( isTextWrapping() && mGuiFont ) {
			mWrappedText = mGuiFont->splitTextToWidth(mCaption,getWidth());
		}
	}


	void ClickLabel::mousePressed(MouseEvent& mouseEvent)
	{
		if (mouseEvent.getButton() == MouseEvent::LEFT) {
			mMousePressed = true;
			mouseEvent.consume();
		}
	}
	
	void ClickLabel::mouseExited(MouseEvent& mouseEvent)
	{
		mHasMouse = false;
	}
	
	void ClickLabel::mouseEntered(MouseEvent& mouseEvent)
	{
		mHasMouse = true;
	}
	
	void ClickLabel::mouseReleased(MouseEvent& mouseEvent)
	{
		if (mouseEvent.getButton() == MouseEvent::LEFT && mMousePressed && mHasMouse) {
			mMousePressed = false;
			distributeActionEvent();
			mouseEvent.consume();
		} else if (mouseEvent.getButton() == MouseEvent::LEFT) {
			mMousePressed = false;
			mouseEvent.consume();
		}
	}
	
	void ClickLabel::mouseDragged(MouseEvent& mouseEvent)
	{
		mouseEvent.consume();
	}
	
	void ClickLabel::keyPressed(KeyEvent& keyEvent)
	{
		Key key = keyEvent.getKey();
	
		if (key.getValue() == Key::ENTER || key.getValue() == Key::SPACE) {
			mKeyPressed = true;
			keyEvent.consume();
		}
	}
	
	void ClickLabel::keyReleased(KeyEvent& keyEvent)
	{
		Key key = keyEvent.getKey();
	
		if ((key.getValue() == Key::ENTER || key.getValue() == Key::SPACE) && mKeyPressed) {
			mKeyPressed = false;
			distributeActionEvent();
			keyEvent.consume();
		}
	}
	
	void ClickLabel::focusLost(const Event& event)
	{
		mMousePressed = false;
		mKeyPressed = false;
	}
}
