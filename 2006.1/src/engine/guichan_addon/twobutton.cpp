/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#include <cassert>
#include "twobutton.h"
#include "log.h"

namespace gcn {
	TwoButton::TwoButton(Image * up_file , Image *down_file)  : Button() {
		up = up_file;
		assert(up!=NULL);
		down = down_file;
		assert(down!=NULL);
		setBorderSize(0);
		adjustSize();
		mMouseDown = false;
		mKeyDown = false;
	}
	TwoButton::~TwoButton() {
	}
	void TwoButton::draw(Graphics *graphics) {
		if (isPressed())
			graphics->drawImage(down, 0, 0);
		else
			graphics->drawImage(up, 0, 0);
	}
	void TwoButton::adjustSize() {
		//FIXME: should check if 'down' is same size
		setWidth(up->getWidth());
		setHeight(up->getHeight());
	}
	/*
	bool TwoButton::isPressed() const
	{
		//FIFE::Log("twobutton") << "has mouse: " << int(hasMouse()) << " mMouseDown " << mMouseDown << " mKeyDown " << mKeyDown;
		return (hasMouse() && mMouseDown) || mKeyDown;
	}
	void TwoButton::mousePress(int x, int y, int button) {
		FIFE::Log("twobutton") << "mouse press " << x<<"," << y << " " << button;
	}
	void TwoButton::mouseRelease(int x, int y, int button) {
		FIFE::Log("twobutton") << "mouse release " << x<<"," << y << " " << button;
	}*/
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
