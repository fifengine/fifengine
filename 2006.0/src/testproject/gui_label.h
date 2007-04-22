/***************************************************************************
 *   Copyright (C) 2005 by the FIFE Team                                   *
 *   fife@developer1.cvsdude.com                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef GUIBASICELEMENTS_H
#define GUIBASICELEMENTS_H

#include "gui_widget.h"
#include "fifelabel.hpp"


namespace gcn {
	class FIFELabel;
};

namespace FIFE {
class Label : public Widget 
{
	public:
		Label();
		virtual ~Label();


		void setLabel(const std::string& s);
		void setPosition(int x, int y);
		gcn::Widget* topWidget();
	private:
		gcn::FIFELabel *m_label;
		bool m_attached;
};


};//FIFE
#endif //GUIBASICELEMENTS_H
