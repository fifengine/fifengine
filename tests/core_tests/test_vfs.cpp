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

// Platform specific includes
#include "util/fife_unit_test.h"

// 3rd party library includes
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/convenience.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/vfs.h"
#include "util/rect.h"
#include "util/time/timemanager.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"
#include "vfs/raw/rawdata.h"
#include "util/exception.h"
#include "vfs/directoryprovider.h"

static const std::string FIFE_TEST_DIR = "fifetestdir";

using boost::unit_test::test_suite;
using namespace FIFE;

// Environment
struct environment {
	boost::shared_ptr<TimeManager> timemanager;
	boost::shared_ptr<VFS> vfs;

	environment()
		: timemanager(new TimeManager()),
		  vfs(new VFS()) {
		VFS::instance()->addSource(new VFSDirectory());
		}
};

#ifdef FIFE_BOOST_VERSION_103300
void test_is_directory() {
#else
BOOST_AUTO_TEST_CASE( OGL_gui_test ) {
#endif
	environment env;

	if(boost::filesystem::exists(FIFE_TEST_DIR+"/"+FIFE_TEST_DIR)) {
		boost::filesystem::remove(FIFE_TEST_DIR+"/"+FIFE_TEST_DIR);
	}

	if(boost::filesystem::exists(FIFE_TEST_DIR)) {
		boost::filesystem::remove(FIFE_TEST_DIR);
	}
	
	// Check root dir
	BOOST_CHECK(VFS::instance()->isDirectory(""));
	BOOST_CHECK(VFS::instance()->isDirectory("/"));

	BOOST_CHECK(!VFS::instance()->isDirectory(FIFE_TEST_DIR));
	boost::filesystem::create_directory(FIFE_TEST_DIR);
	BOOST_CHECK(VFS::instance()->isDirectory(FIFE_TEST_DIR));
	BOOST_CHECK(!VFS::instance()->isDirectory(FIFE_TEST_DIR+"/"+FIFE_TEST_DIR));
	boost::filesystem::create_directories(FIFE_TEST_DIR+"/"+FIFE_TEST_DIR);
	BOOST_CHECK(VFS::instance()->isDirectory(FIFE_TEST_DIR+"/"+FIFE_TEST_DIR));

	boost::filesystem::remove(FIFE_TEST_DIR+"/"+FIFE_TEST_DIR);
	boost::filesystem::remove(FIFE_TEST_DIR);
}

#ifdef FIFE_BOOST_VERSION_103300
test_suite* init_unit_test_suite(int argc, char** const argv) {
	test_suite* test = BOOST_TEST_SUITE("VFS Tests");

	test->add( BOOST_TEST_CASE( &test_is_directory ),0 );

	return test;
}
#endif
