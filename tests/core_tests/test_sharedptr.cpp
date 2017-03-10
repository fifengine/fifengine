/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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
#include "util/base/fife_stdint.h"

using namespace FIFE;

class Data {
public:
	int32_t x,y;

	Data() : x(0), y(0) {}
	Data(int32_t _x, int32_t _y) : x(_x), y(_y) {}

	virtual ~Data() {}

	virtual int32_t total() { return x+y; }
};

class SubData : public Data {
public:
	int32_t z;

	SubData() : Data(0,0), z(0) {}
	SubData(int32_t _x, int32_t _y, int32_t _z) : Data(_x, _y), z(_z) {}

	virtual ~SubData() {}

	virtual int32_t total() { return x+y+z; }
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

/**
* 1. Create a shared pointer to new data
* 2. Check to make sure unique returns true
* 3. Create another shared pointer to same shared data using "=" or copy constructor
* 4. Make sure unique on both shared pointers return false
*/
TEST(case4) {
	SharedPtr<Data> shptr(new Data());
	CHECK(shptr.unique());

	SharedPtr<Data> copy(shptr);

	CHECK(!shptr.unique());
	CHECK(!copy.unique());
}

/**
* 1. Create empty shared pointer using default constructor
* 2. use shared pointer in condition such as an if statement to make sure it evaluates correctly
*/
TEST(case5) {
	SharedPtr<Data> shptr;

	CHECK(shptr == 0);
}

/**
* 1. Create a shared pointer of type base class to a dynamic object of the child class
* 2. Call a virtual function using the "->" of the shared pointer and make sure the proper function overload is called.
*/
TEST(case6) {
	SharedPtr<Data> shptr(new SubData(2,4,6));

	CHECK(shptr->total() == 12);
}

/**
* 1. Create a shared pointer to new data
* 2. Create another shared pointer by using the copy constructor passing the first shared pointer as the parameter
* 3. Create a third shared pointer to new data
* 4. Check for equality of the first 2 shared pointers.
* 5. Check for inequality of the first and 3rd shared pointers.
*/
TEST(case7) {
	SharedPtr<Data> shptr(new Data(2,4));
	SharedPtr<Data> copy(shptr);
	SharedPtr<Data> shptr2(new Data(4,8));

	CHECK(shptr == copy);
	CHECK(shptr != shptr2);
}

/**
* 1. Create a shared pointer to new data
* 2. Create a copy of the shared pointer
* 3. Reset the first shared pointer with new data
* 4. Check the data values of the original shared pointer
* 5. Check the data values of the copied shared pointer
*/
TEST(case8) {
	SharedPtr<Data> shptr(new Data(2,4));
	SharedPtr<Data> copy(shptr);

	CHECK(copy.useCount() == 2);
	shptr.reset(new Data(6,8));
	CHECK(copy.useCount() == 1);

	//shptr holding values we expect?
	CHECK(shptr->x == 6);
	CHECK(shptr->y == 8);

	CHECK(copy->x == 2);
	CHECK(copy->y == 4);

	shptr.reset();
	CHECK(!shptr);
}

int32_t main() {
	return UnitTest::RunAllTests();
}
