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

// Standard C++ library includes
#include <vector> 

// 3rd party library includes
#include <boost/test/unit_test.hpp>


// FIFE includes
#include "map/geometry.h"

using boost::unit_test::test_suite;
using namespace FIFE::map;
using namespace FIFE;

int raw_points[] = { 0,0, -1,-1, -1,0, 12,-12, -12,-12,  -12,12, 529,395, 4000,501, 700,3888  };

void geometries_exist() {
	BOOST_CHECK(Geometry::getGeometryFromId(0) != 0);
	BOOST_CHECK(Geometry::getGeometryFromId(1) != 0);
	// Only 2 Geometries ... 2 should return 0;
	BOOST_CHECK(Geometry::getGeometryFromId(2) == 0);
}

template<int id>
void geo_identities() {
#define ABS(n) ((n) < 0 ? -(n) : (n)) 
	std::vector<Point> points;
	for(size_t i=0; i+1 < sizeof(raw_points)/sizeof(int); i+=2) {
		points.push_back(Point(raw_points[i],raw_points[i+1]));
	}

	Geometry *geometry = Geometry::getGeometryFromId(id);
	for(size_t i=0; i != points.size(); ++i) {
		Point new_point = geometry->fromScreen(geometry->toScreen(points[i]));
		BOOST_CHECK_MESSAGE( new_point == points[i],
			"fromScreen(toScreen(" << points[i] << ") =(?) " << new_point);
	}


	for(size_t i=0; i != points.size(); ++i) {
		Point new_point = geometry->toScreen(geometry->fromScreen(points[i]));
		Point diff = (new_point - points[i]);
		Point bs = geometry->baseSize();
		BOOST_CHECK_MESSAGE(  ABS(diff.x)  < bs.x && ABS(diff.y) < bs.y ,
			"toScreen(fromScreen(" << points[i] << ") =(?) " << new_point);
	}
}

template<int id>
void geo_directionToGrid_props() {
	Geometry *geometry = Geometry::getGeometryFromId(id);
	size_t n_dirs = geometry->getNumDirections();

	Point start(0,0);
	Point p(start);
	for(size_t i=0; i!= n_dirs; ++i) {
		p = p + geometry->directionToGrid(i,p);
	}
	BOOST_CHECK_MESSAGE( p == start,
		"One round-trip should end at start(" << start << ")"
		<< " - ended at " << p << " instead.");

	start=Point(1,0);
	p=start; 
	for(size_t i=0; i!= n_dirs; ++i) {
		p = p + geometry->directionToGrid(i,p);
	}
	BOOST_CHECK_MESSAGE( p == start,
		"One round-trip should end at start(" << start << ")"
		<< " - ended at " << p << " instead.");

	start=Point(0,0);
	for(size_t i=0; i!= n_dirs; ++i) {
		p = start;
		p = p + geometry->directionToGrid(i,p);
		p = p + geometry->directionToGrid((i + n_dirs/2) % n_dirs,p);
		BOOST_CHECK_MESSAGE( p == start,
			"Going one dir and back should end at start(" << start << ")"
			<< " - ended at " << p << " instead. Dir=" << i);
	}

	start=Point(1,0);
	for(size_t i=0; i!= n_dirs; ++i) {
		p = start;
		p = p + geometry->directionToGrid(i,p);
		p = p + geometry->directionToGrid((i + n_dirs/2) % n_dirs,p);
		BOOST_CHECK_MESSAGE( p == start,
			"Going one dir and back should end at start(" << start << ")"
			<< " - ended at " << p << " instead. Dir=" << i);
	}
}


template<int id>
void geo_directionToScreen_props() {
	Geometry *geometry = Geometry::getGeometryFromId(id);
	size_t n_dirs = geometry->getNumDirections();

	std::vector<Point> points;
	for(size_t i=0; i+1 < sizeof(raw_points)/sizeof(int); i+=2) {
		points.push_back(Point(raw_points[i],raw_points[i+1]));
	}

	for(size_t pi = 0; pi != points.size(); ++pi) {
		for(size_t di = 0; di != n_dirs; ++di) {
			Point a = geometry->directionToScreen(di);
			Point b = geometry->toScreen(points[pi] + geometry->directionToGrid(di,points[pi]))
				- geometry->toScreen(points[pi]);
	
			BOOST_CHECK_MESSAGE( a == b,
				"directionToScreen should be grid point independent: "
				<< "point " << points[pi] << " dir " << di
				<< " grid-depend. pos " << b
				<< " grid-INdepend. pos " << a);
		}
	}
}

test_suite* init_unit_test_suite(int argc, char** argv) {
	test_suite* test = BOOST_TEST_SUITE("Geometry Tests");

	test->add( BOOST_TEST_CASE( &geometries_exist ),0 );
	test->add( BOOST_TEST_CASE( &geo_identities<0> ),0 );
	test->add( BOOST_TEST_CASE( &geo_identities<1> ),0 );
	test->add( BOOST_TEST_CASE( &geo_directionToGrid_props<0> ),0 );
	test->add( BOOST_TEST_CASE( &geo_directionToGrid_props<1> ),0 );
	test->add( BOOST_TEST_CASE( &geo_directionToScreen_props<0> ),0 );
	test->add( BOOST_TEST_CASE( &geo_directionToScreen_props<1> ),0 );
	return test;
}
