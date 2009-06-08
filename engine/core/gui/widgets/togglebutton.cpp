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
/***************************************************************************
 * Note! Group and groupmap borrows heavily from ideas of Guichan library  *
 * version 0.8.1                                                           *
 ***************************************************************************/



// Standard C++ library includes
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include <iostream>

#include <guichan/mouseevent.hpp>

#include "togglebutton.h"


namespace gcn {
	ToggleButton::GroupMap ToggleButton::m_groupMap;

	ToggleButton::ToggleButton(Image *up_file , Image *down_file, Image *hover_file, const std::string& caption, const std::string& group): 
		Button(),
		m_upImage(up_file),
		m_downImage(down_file),
		m_hoverImage(hover_file),
		x_downoffset(0),
		y_downoffset(0),
		m_group(group) {

		m_hoverImage = hover_file;
		setFrameSize(0);
		setGroup(m_group);
		adjustSize();
		mCaption = caption;
		m_toggled = false;

		addActionListener(this);
	}
	
	ToggleButton::~ToggleButton() {
		setGroup(""); // Remove button from group
	}
	
	void ToggleButton::setDownOffset(int x, int y) {
		x_downoffset = x;
		y_downoffset = y;
	}
	
	void ToggleButton::draw(Graphics *graphics) {
		Color faceColor = getBaseColor();
        Color highlightColor;
		Color shadowColor;
        int alpha = getBaseColor().a;

		Image* img = NULL;
		int xoffset = 0;
		int yoffset = 0;
		
		if (isPressed() || m_toggled) {
			faceColor = faceColor - 0x303030;
            faceColor.a = alpha;
            highlightColor = faceColor - 0x303030;
            highlightColor.a = alpha;
            shadowColor = faceColor + 0x303030;
            shadowColor.a = alpha;

			if( m_downImage ) {
				img = m_downImage;
				xoffset = x_downoffset;
				yoffset = y_downoffset;
			}
		} else if(mHasMouse) {
			faceColor = faceColor + 0x303030;
            faceColor.a = alpha;
			highlightColor = faceColor + 0x303030;
            highlightColor.a = alpha;
            shadowColor = faceColor - 0x303030;
            shadowColor.a = alpha;

			if ( m_hoverImage ) {
				img = m_hoverImage;
			}
		} else{
			highlightColor = faceColor + 0x303030;
            highlightColor.a = alpha;
            shadowColor = faceColor - 0x303030;
            shadowColor.a = alpha;

			if (m_upImage) {
				img = m_upImage;
			}
		}
		

		graphics->setColor(faceColor);
        graphics->fillRectangle(Rectangle(1, 1, getDimension().width-1, getHeight() - 1));

        graphics->setColor(highlightColor);
        graphics->drawLine(0, 0, getWidth() - 1, 0);
        graphics->drawLine(0, 1, 0, getHeight() - 1);

        graphics->setColor(shadowColor);
        graphics->drawLine(getWidth() - 1, 1, getWidth() - 1, getHeight() - 1);
        graphics->drawLine(1, getHeight() - 1, getWidth() - 1, getHeight() - 1);

        graphics->setColor(getForegroundColor());

		if (img) {
			graphics->drawImage(img, xoffset, yoffset);
		}

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

	void ToggleButton::action(const ActionEvent& actionEvent) {
		setToggled(!m_toggled);
	}

	void ToggleButton::adjustSize() {
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

		if( mCaption.length() > 0 ) {
			w = std::max(static_cast<int>(getFont()->getWidth(mCaption)+2*mSpacing), w);
			h = std::max(static_cast<int>(getFont()->getHeight()+2*mSpacing), h);
		}

		setWidth(w);
		setHeight(h);
	}

	void ToggleButton::setUpImage(Image* image) {
		m_upImage = image;
		adjustSize();
	}

	void ToggleButton::setDownImage(Image* image) {
		m_downImage = image;
		adjustSize();
	}

	void ToggleButton::setHoverImage(Image* image) {
		m_hoverImage = image;
		adjustSize();
	}

	bool ToggleButton::isToggled() const {
		return m_toggled;
	}

	void ToggleButton::setToggled(bool toggled) {
		if (toggled && m_group != "") {
			// untoggle all buttons in group
			GroupIterator iter, iterEnd;
			iterEnd = m_groupMap.upper_bound(m_group);

			for (iter = m_groupMap.lower_bound(m_group); iter != iterEnd; iter++) {
				if (iter->second->isToggled()) {
					iter->second->setToggled(false);
				}
			}
		}

		m_toggled = toggled;
	}

	void ToggleButton::setGroup(const std::string &group) {
		// Remove button from previous group
        if (m_group != "") {
            GroupIterator iter, iterEnd;
            iterEnd = m_groupMap.upper_bound(m_group);

            for (iter = m_groupMap.lower_bound(m_group); iter != iterEnd; iter++) {
                if (iter->second == this) {
                    m_groupMap.erase(iter);
                    break;
                }
            }
        }

		// Add button to new group
        if (group != "") {
            m_groupMap.insert( std::pair<std::string, ToggleButton *>(group, this));
        }

        m_group = group;
    }

    const std::string &ToggleButton::getGroup() const {
        return m_group;
    }

	int ToggleButton::getDownXOffset() const { 
		return x_downoffset; 
	}
	
	int ToggleButton::getDownYOffset() const { 
		return y_downoffset; 
	}

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */

