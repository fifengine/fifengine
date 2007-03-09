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
#include <algorithm>
#include <ctime>
#include <iostream>
#include <vector> 

// 3rd party library includes
#include <boost/test/unit_test.hpp>
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/event.h"

using boost::unit_test::test_suite;
using namespace FIFE::map;

struct integer_type {
	uint8_t d[sizeof(event_t)];
};

struct integer_type2 {
	uint8_t d[sizeof(event_t)*2];
};

typedef void (*void_func)(void);

volatile int nop_i = 0xdeadbeef;
double nop_speed = 0.0;
void nop() { nop_i += nop_i - 1; }

void create_destroy_event() {
	event_t e(0,1);
}

void create_copy_destroy_event() { 
	event_t e1(0,1);
	event_t e2(e1);
}

void make_event_integer_type() {
	makeEvent(1,integer_type());
}

template<int N>
void fill_nop() {
	std::vector<integer_type> v;
	for(size_t i = 0; i != N; ++i)
		v.push_back(integer_type());
}

template<int N>
void fill_nop2() {
	std::vector<integer_type2> v;
	for(size_t i = 0; i != N; ++i)
		v.push_back(integer_type2());
}

template<int N>
void event_fill() {
	std::vector<event_t> v;
	for(size_t i = 0; i != N; ++i)
		v.push_back(event_t(0,1));
}

template<int N>
void make_event_fill_integer_type() {
	std::vector<event_t> v;
	for(size_t i = 0; i != N; ++i)
		v.push_back(makeEvent(1,integer_type()));
}

template<int N>
void eventptr_fill() {
	std::vector<event_t*> v;
	for(size_t i = 0; i != N; ++i)
		v.push_back(new event_t(0,1));
	for(size_t i = 0; i != N; ++i)
		delete v[i];
}

struct counted {
	static long n;
	integer_type d;

	counted() { ++n; }
	counted(const counted& c) : d(c.d) { ++n; }
	~counted() { --n; }
};

long counted::n = 0;

double __profile(const std::string& name, int runs, int repeat, void_func func) {
	std::vector<clock_t> t(runs);
	std::cout 
		<< "Executing " << name 
		<< " " << runs << "x" << repeat <<" times " << std::flush;
	for(int i = 0; i < runs; ++i) {
		t[i] = clock();
		for(int j = 0; j < repeat; ++j)
			func();
		t[i] = clock() - t[i];
		std::cout << i << " " << std::flush;
	}

	double mintime = double(*std::min_element(t.begin(),t.end()));
	double speed = repeat/(mintime/(CLOCKS_PER_SEC));

	if (nop_speed == 0.0)
		nop_speed = speed;

	std::cout 
		<< "["
		<< speed << " call/s, " 
		<< speed/nop_speed << " call/nop ]" << std::endl;
	return speed;
}
#define profile(runs,repeat,func) __profile( #func , runs, repeat, func)

void event_props() {
	SDL_Init(SDL_INIT_TIMER);
	new FIFE::TimeManager();

	std::cout << "sizeof(map::event_t) = " << sizeof(event_t) << std::endl;
	nop_speed = profile( 5, 10000000, nop);
	profile( 5, 1000000, create_destroy_event);
	profile( 5, 1000000, create_copy_destroy_event);
	profile( 5, 100000, make_event_integer_type);

	nop_speed = 0.0;
	nop_speed = profile( 5, 1000, fill_nop<1000>);
	profile( 5, 1000, fill_nop2<1000>);
	profile( 5, 1000, event_fill<1000>);
	profile( 5, 1000, eventptr_fill<1000>);
	profile( 5, 1000, make_event_fill_integer_type<1000>);


	delete FIFE::TimeManager::instance();
	SDL_Quit();
}

void event_refct() {
	SDL_Init(SDL_INIT_TIMER);
	new FIFE::TimeManager();

	std::vector<event_t> v;
	

	counted::n = 0;
	for(size_t i=0; i != 100000; ++i) {
		v.push_back( makeEvent(1,counted()) );
	}
	std::cout << "Before clear() n=" << counted::n;
	v.clear();
	std::cout << "... after clear() n=" << counted::n << std::endl;

	BOOST_CHECK(counted::n == 0);

	delete FIFE::TimeManager::instance();
	SDL_Quit();
}

test_suite* init_unit_test_suite(int argc, char** argv) {
	test_suite* test = BOOST_TEST_SUITE("Event Test");

	test->add( BOOST_TEST_CASE( &event_props ),0 );
	test->add( BOOST_TEST_CASE( &event_refct ),0 );

	return test;
}
