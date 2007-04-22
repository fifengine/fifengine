/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
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
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#ifndef GUIELEMENT_H
#define GUIELEMENT_H

#include <guichan.hpp>
#include "fifelabel.hpp"
#include "fifetextbox.hpp"
#include "fifetextfield.hpp"

#include "inputlistener.h"

namespace FIFE
{
  class Engine;
  class GUI;
};

// This class is a basic "visible" user interface element.
// -
namespace FIFE{

class Widget
{
	public:
		Widget();
		virtual ~Widget();
		virtual void attach();
		virtual void detach();
		virtual bool isAttached();

		virtual gcn::Widget* topWidget() =0;
	private:
		bool m_attached;
	
};

};
#endif // GUIELEMENT_H
