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

// 3rd party library includes
#include <boost/lexical_cast.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "renderable_location.h"

namespace FIFE {
	RenderableLocation::RenderableLocation(const std::string& filename): 
		ResourceLocation(filename),
		m_xshift(0),
		m_yshift(0) {
	}
	
	void RenderableLocation::setXShift(int xshift) {
		m_xshift = xshift;
	}
	
	void RenderableLocation::setYShift(int yshift) {
		m_yshift = yshift;
	}
	
	int RenderableLocation::getXShift() const {
		return m_xshift;
	}
	
	int RenderableLocation::getYShift() const {
		return m_yshift;
	}
	
};//FIFE
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
