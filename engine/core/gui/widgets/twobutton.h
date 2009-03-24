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

#ifndef FIFE_GUICHAN_ADDON_TWOBUTTON_H
#define FIFE_GUICHAN_ADDON_TWOBUTTON_H

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <guichan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace gcn {

	class TwoButton;

	class TwoButton : public Button {
		public:
			TwoButton(Image *up_image = 0, Image *down_image = 0, Image *hover_file = 0, const std::string& caption = "");
			~TwoButton();
			void draw(Graphics *graphics);
			void adjustSize();

			void setUpImage(Image* image);
			void setDownImage(Image* image);
			void setHoverImage(Image* image);
			
			void setDownOffset(int x, int y);
			int getDownXOffset() { return x_downoffset; }
			int getDownYOffset() { return y_downoffset; }
			
		private:
			Image *m_upImage;
			Image *m_downImage;
			Image *m_hoverImage;
			int x_downoffset;
			int y_downoffset;
	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
