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

#ifndef FIFE_GCN_TWOBUTTON_H
#define FIFE_GCN_TWOBUTTON_H
#include <string>
#include <guichan.hpp>
namespace gcn {

	class TwoButton : public Button {
		public:
			TwoButton(Image *up_image, Image *down_image);
			~TwoButton();
			void draw(Graphics *graphics);
			void adjustSize();
			/*
			bool isPressed() const;
			void mousePress(int,int,int);
			void mouseRelease(int,int,int);
			*/
		private:
			Image *up;
			Image *down;
	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
