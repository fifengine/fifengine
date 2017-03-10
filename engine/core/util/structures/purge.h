/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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


// Delete pointers in an STL sequence container
#ifndef PURGE_H
#define PURGE_H
#include <algorithm>

template<class Seq> void purge(Seq& c) {
	typename Seq::iterator i;
	for(i = c.begin(); i != c.end(); i++) {
		delete *i;
		*i = 0;
	}
}

// Iterator version:
template<class InpIt>
void purge(InpIt begin, InpIt end) {
	while(begin != end) {
		delete *begin;
		*begin = 0;
		begin++;
	}
}
template<class Seq> void purge_map(Seq& c) {
	typename Seq::iterator i;
	for(i = c.begin(); i != c.end(); i++) {
		delete i->second;
		i->second = 0;
	}
}

// Iterator version:
template<class InpIt>
void purge_map(InpIt begin, InpIt end) {
	while(begin != end) {
		delete begin->second;
		begin->second = 0;
		begin++;
	}
}
#endif // PURGE_H 
