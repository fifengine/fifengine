/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
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

// Standard C++ library includes
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "gui/base/gui_font.h" 
#include "util/exception.h"
#include "video/image.h" 

#include "clicklabel.h"

namespace gcn {
	ClickLabel::ClickLabel(): Button() {
		mGuiFont = static_cast<FIFE::GuiFont*> (getFont());
		setAlignment(Graphics::LEFT);
		setTextWrapping(false);
		setBorderSize(0);
	}
	
	ClickLabel::ClickLabel(const std::string& caption): Button(caption)  {
		mGuiFont = static_cast<FIFE::GuiFont*> (getFont());
		setAlignment(Graphics::LEFT);
		setTextWrapping(false);
		setBorderSize(0);
		wrapText();
	}
	
	ClickLabel::~ClickLabel() {
	}
	
	void ClickLabel::setCaption(const std::string& caption) {
		Button::setCaption(caption);
		mGuiFont = static_cast<FIFE::GuiFont*> (getFont());
		wrapText();
	}

	void ClickLabel::setWidth(int width) {
		Button::setWidth(width);
		wrapText();
	}
	
	void ClickLabel::draw(Graphics* graphics) {

		if (mGuiFont != static_cast<FIFE::GuiFont*> (getFont())) {
			mGuiFont = static_cast<FIFE::GuiFont*> (getFont());
			wrapText();
			adjustSize();
		}

		int textX;
		int textY = 0;

		switch (getAlignment())
		{
			case Graphics::LEFT:
				textX = 0;
				break;
			case Graphics::CENTER:
				textX = getWidth() / 2;
				break;
			case Graphics::RIGHT:
				textX = getWidth();
				break;
			default:
				throw FIFE::GuiException("Unknown alignment.");
		}
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
				image = mGuiFont->getAsImage(mCaption);
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

}
