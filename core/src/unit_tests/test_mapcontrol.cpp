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
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/map.h"
#include "map/control.h"
#include "map/elevation.h"
#include "map/layer.h"
#include "util/time/timemanager.h"
#include "util/settingsmanager.h"
#include "engine.h"

using boost::unit_test::test_suite;
using namespace FIFE::map;
using namespace FIFE;

MapPtr create_map() {
	MapPtr map = Map::create();
	ElevationPtr elev = Elevation::create();
	elev->addLayer( Layer::create( Point(100,100), 0) );
	elev->addLayer( Layer::create( Point(200,200), 1) );
	elev->addLayer( Layer::create( Point(100,100), 0) );
	map->addElevation( elev );
	return map;
}

void run_some_turns(Control& control,int n=10) {
	for(int i=0; i!=n; ++i) {
		TimeManager::instance()->update();
		control.turn();
		SDL_Delay(1);
	}
}

void mapcontrol_test() {
	try {
		// This just checks robustness of
		// the control against random state-change requests.
		Engine engine(0,0);
		Log::setLogLevel(Log::LEVEL_MAX);
		{
			Control map_control;
			run_some_turns(map_control);
			map_control.stop();
			run_some_turns(map_control);
			map_control.start();
			run_some_turns(map_control);
			map_control.stop();
			run_some_turns(map_control);
			map_control.setMap( create_map() );
			run_some_turns(map_control);
			map_control.stop();
			run_some_turns(map_control);
			map_control.start();
			run_some_turns(map_control,100);
			map_control.stop();
			run_some_turns(map_control);
			map_control.start();
			run_some_turns(map_control);
			map_control.setMap( create_map() );
			run_some_turns(map_control);
			map_control.stop();
			run_some_turns(map_control);
			map_control.start();
			run_some_turns(map_control,100);
			map_control.stop();
			run_some_turns(map_control);
			map_control.clearMap();
			run_some_turns(map_control);
			map_control.start();
			run_some_turns(map_control);
			map_control.stop();
		}
	} catch( Exception& e ) {
		Log("test_mapcontrol") << e.getMessage();
		throw;
	}
}

test_suite* init_unit_test_suite(int argc, char** argv) {
	test_suite* test = BOOST_TEST_SUITE("MapControl tests");
	test->add(BOOST_TEST_CASE(&mapcontrol_test), 0);
	return test;
}
