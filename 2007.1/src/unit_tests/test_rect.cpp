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
#include <ctime>
#include <vector> 

// 3rd party library includes
#include <boost/test/unit_test.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/rect.h"

using boost::unit_test::test_suite;
using namespace FIFE;

void rectangle_intersection() {
	Rect a(0,0,10,10);

	std::vector<Rect> do_intersect;
#define ADD_RECT(x,y,w,h) do_intersect.push_back( Rect(x,y,w,h) )

	ADD_RECT(5,5,10,10);
	ADD_RECT(-5,5,10,10);
	ADD_RECT(-5,-5,10,10);
	ADD_RECT(5,-5,10,10);

	ADD_RECT(-5,5,20,1);
	ADD_RECT(-5,5,10,1);

	ADD_RECT(5,-5,1,20);
	ADD_RECT(5,-5,1,10);

	ADD_RECT(5,5,3,3);
	
	ADD_RECT(-5,-5,30,30);

	for(size_t i=0; i<do_intersect.size(); ++i) {
		BOOST_CHECK(a.intersects(do_intersect[i]));
		BOOST_CHECK(do_intersect[i].intersects(a));
	}	

	std::vector<Rect> dont_intersect;

#undef ADD_RECT
#define ADD_RECT(x,y,w,h) dont_intersect.push_back( Rect(x,y,w,h) )

	ADD_RECT(-5,-5,4,4);
	ADD_RECT(-5,-5,40,4);
	ADD_RECT(-5,-5,4,40);
	ADD_RECT(-5,-5,4,4);

	ADD_RECT(15,15,4,4);
	ADD_RECT(15,15,40,4);
	ADD_RECT(15,15,4,40);

	for(size_t i=0; i<dont_intersect.size(); ++i) {
		BOOST_CHECK(!a.intersects(dont_intersect[i]));
		BOOST_CHECK(!dont_intersect[i].intersects(a));
	}	

}


test_suite* init_unit_test_suite(int argc, char** argv) {
	test_suite* test = BOOST_TEST_SUITE("Rectangle Tests");

	test->add( BOOST_TEST_CASE( &rectangle_intersection ),0 );

	return test;
}
