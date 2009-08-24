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

#include "twobutton.h"

namespace gcn {
	TwoButton::TwoButton(Image *up_file , Image *down_file, Image *hover_file, const std::string& caption): 
		Button(),
		m_upImage(up_file),
		m_downImage(down_file),
		m_hoverImage(hover_file),
		x_downoffset(0),
		y_downoffset(0) {
		m_hoverImage = hover_file;
		setFrameSize(0);
		adjustSize();
		mCaption = caption;
	}
	
	TwoButton::~TwoButton() {
	}
	
	void TwoButton::setDownOffset(int x, int y) {
		x_downoffset = x;
		y_downoffset = y;
	}
	
	void TwoButton::draw(Graphics *graphics) {
		Image* img = m_upImage;
		int xoffset = 0;
		int yoffset = 0;
		
		if (isPressed()) {
			if( m_downImage ) {
				img = m_downImage;
				xoffset = x_downoffset;
				yoffset = y_downoffset;
			}
		} else if(mHasMouse) {
			if( m_hoverImage ) {
				img = m_hoverImage;
			}
		}
		
		if (img) {
			graphics->drawImage(img, xoffset, yoffset);
		}

		graphics->setColor(getForegroundColor());
		int textX;
		int textY = getHeight() / 2 - getFont()->getHeight() / 2;
		switch (getAlignment())
		{
			case Graphics::LEFT:
				textX = 4;
				break;
			case Graphics::CENTER:
				textX = getWidth() / 2;
				break;
			case Graphics::RIGHT:
				textX = getWidth() - 4;
				break;
			default:
				throw GCN_EXCEPTION("Unknown alignment.");
		}

		graphics->setFont(getFont());
		if (mCaption.size() > 0) {
			if (isPressed())
				graphics->drawText(getCaption(), textX + 1, 
						textY + 1, getAlignment());
			else
				graphics->drawText(getCaption(), textX, textY, getAlignment());
		}
	}
	void TwoButton::adjustSize() {
		int w = 0;
		int h = w;
		if( m_upImage ) {
			w = m_upImage->getWidth();
			h = m_upImage->getHeight();
		}
		if( m_downImage ) {
			w = std::max(m_downImage->getWidth(), w);
			h = std::max(m_downImage->getHeight(), h);
		}
		if( m_hoverImage ) {
			w = std::max(m_hoverImage->getWidth(), w);
			h = std::max(m_hoverImage->getHeight(), h);
		}
		setWidth(w);
		setHeight(h);
	}
	void TwoButton::setUpImage(Image* image) {
		m_upImage = image;
		adjustSize();
	}
	void TwoButton::setDownImage(Image* image) {
		m_downImage = image;
		adjustSize();
	}
	void TwoButton::setHoverImage(Image* image) {
		m_hoverImage = image;
		adjustSize();
	}
		
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */

