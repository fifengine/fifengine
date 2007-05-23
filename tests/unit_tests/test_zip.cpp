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
#include <boost/test/unit_test.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log.h"
#include "vfs/vfs.h"
#include "vfs/vfshostsystem.h"
#include "vfs/zip/zipprovider.h"

using boost::unit_test::test_suite;
using namespace FIFE;

static const std::string ZIP_FILE = "data/zipvfstest.zip";

static boost::shared_ptr<VFS> initialize_vfs(bool add_zip) {
	boost::shared_ptr<VFS> result(new VFS());

	// Make sure the zip file we are testing with actually exists:
	result->addSource(new VFSHostSystem());
	if ((!result->exists(ZIP_FILE))) {
		BOOST_FAIL("Test zip archive \"" << ZIP_FILE << "\" not found.");
	}

	if (add_zip)
	{
		// Add the zip provider:
		FIFE::zip::ZipProvider provider;
		// Validity of the zip file is checked in one of the tests below.
		result->addSource(provider.createSource(ZIP_FILE));
	}

	return result;
}

void test_isReadable() {
	boost::shared_ptr<VFS> pvfs = initialize_vfs(false);
	FIFE::zip::ZipProvider provider;
	if (!provider.isReadable(ZIP_FILE)) {
		BOOST_ERROR("Test zip archive \"" << ZIP_FILE << "\" not readable.");
	}
}

void test_isNotReadable() {
	boost::shared_ptr<VFS> pvfs = initialize_vfs(false);
	FIFE::zip::ZipProvider provider;
	if (provider.isReadable("data/dat1vfstest.dat")) {
		BOOST_ERROR("False positive on a non-zip file.");
	}
}

void test_createSource() {
	boost::shared_ptr<VFS> pvfs = initialize_vfs(false);
	FIFE::zip::ZipProvider provider;
	if (provider.createSource(ZIP_FILE) == NULL) {
		BOOST_ERROR("Provider does not make a valid source.");
	}
}

void test_fileExists() {
	Log::setLogLevel(Log::LEVEL_MAX);
	initialize_vfs(true);
	BOOST_ERROR("Blank Test. Please provide an implementation");
}

void test_fileNotExists() {
	Log::setLogLevel(Log::LEVEL_MAX);
	initialize_vfs(true);
	BOOST_ERROR("Blank Test. Please provide an implementation");
}

void test_listFiles() {
	Log::setLogLevel(Log::LEVEL_MAX);
	initialize_vfs(true);
	BOOST_ERROR("Blank Test. Please provide an implementation");
}

void test_listDirectories() {
	Log::setLogLevel(Log::LEVEL_MAX);
	initialize_vfs(true);
	BOOST_ERROR("Blank Test. Please provide an implementation");
}

void test_fileRead() {
	Log::setLogLevel(Log::LEVEL_MAX);
	initialize_vfs(true);
	BOOST_ERROR("Blank Test. Please provide an implementation");
}

test_suite* init_unit_test_suite(int argc, char* argv[]) {
	test_suite* test = BOOST_TEST_SUITE("VFS Zip tests");
	test->add(BOOST_TEST_CASE(&test_isReadable), 0);
	test->add(BOOST_TEST_CASE(&test_isNotReadable), 0);
	test->add(BOOST_TEST_CASE(&test_fileExists), 0);
	test->add(BOOST_TEST_CASE(&test_fileNotExists), 0);
	test->add(BOOST_TEST_CASE(&test_listFiles), 0);
	test->add(BOOST_TEST_CASE(&test_listDirectories), 0);
	test->add(BOOST_TEST_CASE(&test_fileRead), 0);
	return test;
}
