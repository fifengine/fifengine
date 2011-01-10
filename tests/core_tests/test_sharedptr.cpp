/***************************************************************************
 *   Copyright (C) 2005-2011 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

// Standard C++ library includes
#include <iostream>
#include <iomanip>

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/sharedptr.h"

using namespace FIFE;

class Data {
public:
	int x,y;

	Data() : x(0), y(0) {}
	Data(int _x, int _y) : x(_x), y(_y) {}
	~Data() {}
};

/**
* 1. Create a shared pointer to new data
* 2. Create another shared pointer by using the "=" operator to the first shared pointer
* 3. Call reset on the first shared pointer and make sure the data is still valid
* 4. Call reset on the second shared pointer and make sure the dynamic memory is deleted
**/

TEST(case1) {
	SharedPtr<Data> shptr(new Data(5,10));

	SharedPtr<Data> copy = shptr;
	CHECK(shptr.useCount() == 2);

	shptr.reset();
	CHECK(!shptr);

	CHECK(copy.useCount() == 1);
	CHECK(copy.unique());

	CHECK(copy->x == 5);
	CHECK(copy->y == 10);

	copy.reset();
	CHECK(!copy);
}

/**
* 1. Create a shared pointer to new data
* 2. Create another shared pointer by using the copy constructor passing the first shared pointer as the parameter
* 3. Call reset on the first shared pointer and make sure the data is still valid
* 4. Call reset on the second shared pointer and make sure the dynamic memory is deleted
**/

TEST(case2) {
	SharedPtr<Data> shptr(new Data(5,10));

	SharedPtr<Data> copy(shptr);
	CHECK(shptr.useCount() == 2);

	shptr.reset();
	CHECK(!shptr);

	CHECK(copy.useCount() == 1);
	CHECK(copy.unique());

	CHECK(copy->x == 5);
	CHECK(copy->y == 10);

	copy.reset();
	CHECK(!copy);
}

/**
* 1. Create a shared pointer to new data
* 2. Set data using the "->" operator of the shared pointer
* 3. Create another shared pointer either using "=" or copy constructor
* 4. Access underlying dynamic data using the "*" operator and make sure the value is the same as what was set in step 2.
*/
TEST(case3) {
	SharedPtr<Data> shptr(new Data());

	shptr->x = 5;
	shptr->y = 10;

	SharedPtr<Data> copy(shptr);
	CHECK(shptr.useCount() == 2);

	shptr.reset();
	CHECK(!shptr);

	Data d = *copy;

	CHECK(copy.useCount() == 1);
	CHECK(copy.unique());

	CHECK(d.x == 5);
	CHECK(d.y == 10);

	copy.reset();
	CHECK(!copy);
}

int main() {
	return UnitTest::RunAllTests();
}
