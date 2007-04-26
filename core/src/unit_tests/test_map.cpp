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
#include <stdio.h>

// 3rd party library includes
#include <boost/test/unit_test.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/map.h"
#include "map/elevation.h"
#include "map/layer.h"
#include "map/objectinfo.h"
#include "map/factory.h"
#include "vfs/vfssourcefactory.h"
#include "vfs/vfs.h"
#include "util/settingsmanager.h"


using boost::unit_test::test_suite;
using namespace FIFE::map;
using namespace FIFE;

// Map Environment
struct environment {
	boost::shared_ptr<SettingsManager> settings;
	boost::shared_ptr<VFSSourceFactory> vfssources;
	boost::shared_ptr<VFS> vfs;
	boost::shared_ptr<Factory> factory;

	environment()
		: settings(new SettingsManager()),
		  vfssources(new VFSSourceFactory()),
		  vfs(new VFS()),
		  factory(new Factory()) {}
};

void map_test() {
	environment env;

	{
		MapPtr map = Map::create();
	
		ElevationPtr e1 = Elevation::create();
		ElevationPtr e2 = Elevation::create();
		ElevationPtr e3 = Elevation::create();

		BOOST_CHECK_THROW( map->addElevation( ElevationPtr() ), Exception );

		map->addElevation( e1 );
		map->addElevation( e3 );
		map->insertElevation( 1, e2 );
	
		BOOST_CHECK( map->getNumElevations() == 3 );
		BOOST_CHECK( map->getElevation(0) == e1 );
		BOOST_CHECK( map->getElevation(1) == e2 );
		BOOST_CHECK( map->getElevation(2) == e3 );
		BOOST_CHECK_THROW( map->getElevation(3), IndexOverflow );
	
		map->removeElevation( 1 );
		BOOST_CHECK( map->getNumElevations() == 2 );
		BOOST_CHECK( map->getElevation(0) == e1 );
		BOOST_CHECK( map->getElevation(1) == e3 );

		BOOST_CHECK( e1->getMap() == map );
		BOOST_CHECK( !e2->getMap() );
		BOOST_CHECK( e3->getMap() == map );

		BOOST_CHECK_THROW( map->addElevation(e1), Duplicate );
		BOOST_CHECK_THROW( map->insertElevation(0,e1), Duplicate );

		map.reset();

		BOOST_CHECK( !e1->getMap() );
		BOOST_CHECK( !e3->getMap() );
	}
	BOOST_CHECK( Elevation::globalCount() == 0 );
}

void elevation_test() {
	environment env;

	{
		ElevationPtr elevation = Elevation::create();
	
		LayerPtr l1 = Layer::create(Point(100,100),0);
		LayerPtr l2 = Layer::create(Point(100,100),0);
		LayerPtr l3 = Layer::create(Point(100,100),0);

		BOOST_CHECK_THROW( elevation->addLayer( LayerPtr() ), Exception );
	
		elevation->addLayer( l1 );
		elevation->addLayer( l3 );
		elevation->insertLayer( 1, l2 );
	
		BOOST_CHECK( elevation->getNumLayers() == 3 );
		BOOST_CHECK( elevation->getLayer(0) == l1 );
		BOOST_CHECK( elevation->getLayer(1) == l2 );
		BOOST_CHECK( elevation->getLayer(2) == l3 );
		BOOST_CHECK_THROW( elevation->getLayer(3), IndexOverflow );
	
		elevation->removeLayer( 1 );
		BOOST_CHECK( elevation->getNumLayers() == 2 );
		BOOST_CHECK( elevation->getLayer(0) == l1 );
		BOOST_CHECK( elevation->getLayer(1) == l3 );

		BOOST_CHECK( l1->getElevation() == elevation );
		BOOST_CHECK( !l2->getElevation() );
		BOOST_CHECK( l3->getElevation() == elevation );

		BOOST_CHECK_THROW( elevation->addLayer(l1), Duplicate );
		BOOST_CHECK_THROW( elevation->insertLayer(0,l1), Duplicate );

		elevation.reset();

		BOOST_CHECK( !l1->getElevation() );
		BOOST_CHECK( !l3->getElevation() );
	}
	BOOST_CHECK( Layer::globalCount() == 0 );
}

void layer_numbering_test() {
	environment env;

	ElevationPtr elevation = Elevation::create();

	LayerPtr l[5];
	l[0] = Layer::create(Point(100,100),0);
	l[1] = Layer::create(Point(100,100),0);
	l[2] = Layer::create(Point(100,100),0);
	l[3] = Layer::create(Point(100,100),0);
	l[4] = Layer::create(Point(100,100),0);

	elevation->addLayer( l[0] );
	elevation->addLayer( l[1] );
	elevation->addLayer( l[3] );
	elevation->addLayer( l[4] );
	elevation->insertLayer( 2, l[2] );

	for(size_t i=0; i != elevation->getNumLayers(); ++i) {
		BOOST_CHECK( elevation->getLayer(i)->getLayerNumber() == i );
	}

	elevation->removeLayer(0);
	elevation->removeLayer(2);

	for(size_t i=0; i != elevation->getNumLayers(); ++i) {
		BOOST_CHECK( elevation->getLayer(i)->getLayerNumber() == i );
	}
}

test_suite* init_unit_test_suite(int argc, char** argv) {
	test_suite* test = BOOST_TEST_SUITE("Map/Elevation/Layer tests");
	test->add(BOOST_TEST_CASE(&map_test), 0);
	test->add(BOOST_TEST_CASE(&elevation_test), 0);
	test->add(BOOST_TEST_CASE(&layer_numbering_test), 0);
	return test;
}
