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
#include "util/settingsmanager.h"

using boost::unit_test::test_suite;
using namespace FIFE;

static const std::string SETTINGS_FILE_NAME = "__test_settingsmanager.data";

void write_test() {
	boost::shared_ptr<SettingsManager> sm(new SettingsManager());
	sm->write("firstValue", 1);
	sm->write("secondValue", 3);
	sm->write("font", "content/fonts/FreeMono.ttf");
	sm->write("boolValue", false);
	BOOST_CHECK_THROW(sm->saveSettings(SETTINGS_FILE_NAME, false), CannotOpenFile);
	sm->saveSettings(SETTINGS_FILE_NAME, true);
}

void read_test() {
	boost::shared_ptr<SettingsManager> sm(new SettingsManager());
	sm->loadSettings(SETTINGS_FILE_NAME);
	unsigned int utmp = sm->read("firstValue", 0);
	BOOST_CHECK(utmp != 0);
	utmp = sm->read("secondValue", 100);
	BOOST_CHECK(utmp == 3);
	std::string strtmp = sm->read<std::string>("font", "-");
	BOOST_CHECK(strtmp == "content/fonts/FreeMono.ttf");
	bool btmp = sm->read("boolValue", true);
	BOOST_CHECK(!btmp);
}

void cleanup_test() {
	BOOST_CHECK(remove(SETTINGS_FILE_NAME.c_str()) == 0);
}

test_suite* init_unit_test_suite(int argc, char** argv) {
	test_suite* test = BOOST_TEST_SUITE("Settings manager tests");
	test->add(BOOST_TEST_CASE(&write_test), 0);
	test->add(BOOST_TEST_CASE(&read_test), 0);
	test->add(BOOST_TEST_CASE(&cleanup_test), 0);
	return test;
}
