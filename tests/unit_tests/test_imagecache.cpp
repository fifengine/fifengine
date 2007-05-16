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
#include <string>

// 3rd party library includes
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/vfssourcefactory.h"
#include "vfs/vfs.h"
#include "video/imagecache.h"
#include "video/renderable.h"
#include "util/settingsmanager.h"


using boost::unit_test::test_suite;
using namespace FIFE;

// Environment
struct environment {
	boost::shared_ptr<SettingsManager> settings;
	boost::shared_ptr<VFSSourceFactory> vfssources;
	boost::shared_ptr<VFS> vfs;
	boost::shared_ptr<ImageCache> imagecache;

	environment()
		: settings(new SettingsManager()),
		  vfssources(new VFSSourceFactory()),
		  vfs(new VFS()),
		  imagecache(new ImageCache()) {}
};

void invariants_test() {
	environment env;

	std::string filename;
	std::set<size_t> ids;

	// Very basic test for duplicate ids...
	for(int i=0; i!= 100; ++i) {
		filename = boost::lexical_cast<std::string>(i);
		size_t id = env.imagecache->addImageFromFile(filename);
		BOOST_CHECK( ids.count( id ) == 0 );
		ids.insert( id );
	}

	for(int i=0; i!= 100; ++i) {
		filename = boost::lexical_cast<std::string>(i);
		size_t id = env.imagecache->addImageFromFile(filename);
		BOOST_CHECK( ids.count( id ) == 1 );
	}

}

test_suite* init_unit_test_suite(int argc, char** argv) {
	test_suite* test = BOOST_TEST_SUITE("ImageCache tests");
	test->add(BOOST_TEST_CASE(&invariants_test), 0);
	return test;
}
