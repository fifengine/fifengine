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

void simple_ownership_test() {
	environment env;

	LayerPtr layer = Layer::create(Point(100,100),0);
	LayerPtr layer2 = Layer::create(Point(100,100),1);
	ObjectPtr object(ObjectInfo::create());

	std::cout << "sizeof(map::ObjectInfo) = " << sizeof(ObjectInfo) << std::endl;

	BOOST_CHECK( !object->hasOwner() );
	BOOST_CHECK( !object->isOwner( layer ) );
	BOOST_CHECK( !object->isOwner( layer2 ) );
	BOOST_CHECK( !object->isOwner( LayerPtr() ) );

	object->setOwner(layer);

	BOOST_CHECK( object->hasOwner() );
	BOOST_CHECK( object->isOwner( layer ) );
	BOOST_CHECK( !object->isOwner( layer2 ) );
	BOOST_CHECK( !object->isOwner( LayerPtr() ) );

	object->setOwner(layer2);

	BOOST_CHECK( object->hasOwner() );
	BOOST_CHECK( !object->isOwner( layer ) );
	BOOST_CHECK( object->isOwner( layer2 ) );
	BOOST_CHECK( !object->isOwner( LayerPtr() ) );

	object->setOwner(LayerPtr());

	BOOST_CHECK( !object->hasOwner() );
	BOOST_CHECK( !object->isOwner( layer ) );
	BOOST_CHECK( !object->isOwner( layer2 ) );
	BOOST_CHECK( !object->isOwner( LayerPtr() ) );
}

void indirect_ownership_test() {
	environment env;

	ObjectPtr o1 = ObjectInfo::create();
	ObjectPtr o2 = ObjectInfo::create();
	ObjectPtr o3 = ObjectInfo::create();

	o1->addToInventory( o2 );
	o2->addToInventory( o3 );
	
	BOOST_CHECK( o3->isIndirectOwner( o1 ) );
	BOOST_CHECK( !o1->isIndirectOwner( o3 ) );
	BOOST_CHECK_THROW( o3->addToInventory( o1 ), Duplicate );
}

void inventory_ownership_test() {
	environment env;
	BOOST_CHECK( ObjectInfo::globalCount() == 0 );
	{
		ObjectPtr object(ObjectInfo::create());
		ObjectPtr o1(ObjectInfo::create());
		ObjectPtr o2(ObjectInfo::create());
		BOOST_CHECK( ObjectInfo::globalCount() == 3 );
	
		object->addToInventory( o1 );
		object->addToInventory( o2 );
		BOOST_CHECK( o1->isOwner( object ) );
		BOOST_CHECK( o2->isOwner( object ) );
		BOOST_CHECK( object->getInventory().contains( o1 ) );
		BOOST_CHECK( object->getInventory().contains( o2 ) );
		BOOST_CHECK( object->getInventory().size() == 2 );
	
		object->addToInventory( o1 );
		object->getInventory().erase( o1 );
		BOOST_CHECK( !o1->hasOwner() );
		BOOST_CHECK( object->getInventory().size() == 1 );
	
		BOOST_CHECK_THROW(o2->addToInventory( object ), Exception);
	}	
	BOOST_CHECK( ObjectInfo::globalCount() == 0 );
}

void layer_ownership_test() {
	environment env;
	{
		ObjectPtr o1(ObjectInfo::create());
		ObjectPtr o2(ObjectInfo::create());
	
		LayerPtr layer = Layer::create(Point(100,100),0);
	
		layer->addObject( o1 );
		layer->addObjectAt( o2, 50,50 );
		
		BOOST_CHECK( o1->isOwner( layer ) );
		BOOST_CHECK( o2->isOwner( layer ) );
	
		layer->removeObject( o1 );
		layer->getObjectsAt(50,50).erase( o2 );
	
		BOOST_CHECK( !o1->hasOwner() );
		BOOST_CHECK( !o2->hasOwner() );
	
		layer->addObjectAt( o1, 10,10 );
		layer->addObjectAt( o1, 20,20 );
		layer->getObjectsAt(90,90).append( o2 );
		
		BOOST_CHECK( o1->isOwner( layer ) );
		BOOST_CHECK( o2->isOwner( layer ) );
	
		BOOST_CHECK( layer->getObjectsAt(10,10).empty() );
		BOOST_CHECK( o1->getPosition() == Point(20,20) );
		BOOST_CHECK( o2->getPosition() == Point(90,90) );
	
		o1->resetOwner();
		o2->resetOwner();
	
		BOOST_CHECK( layer->getObjectsAt(20,20).empty() );
		BOOST_CHECK( layer->getObjectsAt(90,90).empty() );
		BOOST_CHECK( layer->getAllObjects().empty() );
	}
	BOOST_CHECK( ObjectInfo::globalCount() == 0 );
}


test_suite* init_unit_test_suite(int argc, char** argv) {
	test_suite* test = BOOST_TEST_SUITE("Object tests");
	test->add(BOOST_TEST_CASE(&simple_ownership_test), 0);
	test->add(BOOST_TEST_CASE(&inventory_ownership_test), 0);
	test->add(BOOST_TEST_CASE(&layer_ownership_test), 0);
	test->add(BOOST_TEST_CASE(&indirect_ownership_test), 0);
	return test;
}
