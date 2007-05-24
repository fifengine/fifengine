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
#include "eventchannel/manager/eventmanager.h"
#include "eventchannel/command/ec_command.h"
#include "eventchannel/command/ec_icommandlistener.h"

using boost::unit_test::test_suite;
using namespace FIFE;

class EventTester: public ICommandListener {
public:
	EventTester(): m_commandreceived(false) {}

	void onCommand(const ICommand& command) {
		m_commandreceived = true;
	}

	bool m_commandreceived;
};

void test_send_commands() {
	Log::setLogLevel(Log::LEVEL_MAX);
	EventManager manager;
	EventTester tester;
	manager.addCommandListener(&tester);
	Command cmd;
	manager.dispatchCommand(cmd);
	BOOST_CHECK(tester.m_commandreceived);
}

test_suite* init_unit_test_suite(int argc, char* argv[]) {
	test_suite* test = BOOST_TEST_SUITE("Event channel tests");
	test->add(BOOST_TEST_CASE(&test_send_commands), 0);
	return test;
}
