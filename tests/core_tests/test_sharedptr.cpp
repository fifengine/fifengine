// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <iomanip>
#include <iostream>

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes

// FIFE includes
#include "util/base/fife_stdint.h"
#include "util/base/sharedptr.h"

using FIFE::SharedPtr;

class Data
{
    public:
        int32_t x, y;

        Data() : x(0), y(0)
        {
        }
        Data(int32_t _x, int32_t _y) : x(_x), y(_y)
        {
        }

        virtual ~Data()               = default;
        Data(Data const &)            = default;
        Data& operator=(Data const &) = default;
        Data(Data&&)                  = default;
        Data& operator=(Data&&)       = default;

        virtual int32_t total()
        {
            return x + y;
        }
};

class SubData : public Data
{
    public:
        int32_t z;

        SubData() : Data(0, 0), z(0)
        {
        }
        SubData(int32_t _x, int32_t _y, int32_t _z) : Data(_x, _y), z(_z)
        {
        }

        ~SubData() override                 = default;
        SubData(SubData const &)            = default;
        SubData& operator=(SubData const &) = default;
        SubData(SubData&&)                  = default;
        SubData& operator=(SubData&&)       = default;

        int32_t total() override
        {
            return x + y + z;
        }
};

/**
 * 1. Create a shared pointer to new data
 * 2. Create another shared pointer by using the "=" operator to the first shared pointer
 * 3. Call reset on the first shared pointer and make sure the data is still valid
 * 4. Call reset on the second shared pointer and make sure the dynamic memory is deleted
 **/

TEST_CASE("SharedPtr::operator= preserves data after source reset", "[core][sharedptr]")
{
    SharedPtr<Data> shptr(new Data(5, 10));

    SharedPtr<Data> copy = shptr;
    CHECK_EQ(shptr.useCount(), 2);

    shptr.reset();
    CHECK(!shptr);

    CHECK_EQ(copy.useCount(), 1);
    CHECK(copy.unique());

    CHECK_EQ(copy->x, 5);
    CHECK_EQ(copy->y, 10);

    copy.reset();
    CHECK(!copy);
}

/**
 * 1. Create a shared pointer to new data
 * 2. Create another shared pointer by using the copy constructor passing the first shared pointer as the parameter
 * 3. Call reset on the first shared pointer and make sure the data is still valid
 * 4. Call reset on the second shared pointer and make sure the dynamic memory is deleted
 **/

TEST_CASE("SharedPtr::copy constructor preserves data after source reset", "[core][sharedptr]")
{
    SharedPtr<Data> shptr(new Data(5, 10));

    SharedPtr<Data> copy(shptr);
    CHECK_EQ(shptr.useCount(), 2);

    shptr.reset();
    CHECK(!shptr);

    CHECK_EQ(copy.useCount(), 1);
    CHECK(copy.unique());

    CHECK_EQ(copy->x, 5);
    CHECK_EQ(copy->y, 10);

    copy.reset();
    CHECK(!copy);
}

/**
 * 1. Create a shared pointer to new data
 * 2. Set data using the "->" operator of the shared pointer
 * 3. Create another shared pointer either using "=" or copy constructor
 * 4. Access underlying dynamic data using the "*" operator and make sure the value is the same as what was set in
 * step 2.
 */
TEST_CASE("SharedPtr::operator-> and operator* access underlying data", "[core][sharedptr]")
{
    SharedPtr<Data> shptr(new Data());

    shptr->x = 5;
    shptr->y = 10;

    SharedPtr<Data> copy(shptr);
    CHECK_EQ(shptr.useCount(), 2);

    shptr.reset();
    CHECK(!shptr);

    Data const d = *copy;

    CHECK_EQ(copy.useCount(), 1);
    CHECK(copy.unique());

    CHECK_EQ(d.x, 5);
    CHECK_EQ(d.y, 10);

    copy.reset();
    CHECK(!copy);
}

/**
 * 1. Create a shared pointer to new data
 * 2. Check to make sure unique returns true
 * 3. Create another shared pointer to same shared data using "=" or copy constructor
 * 4. Make sure unique on both shared pointers return false
 */
TEST_CASE("SharedPtr::unique reports single vs shared ownership", "[core][sharedptr]")
{
    SharedPtr<Data> const shptr(new Data());
    CHECK(shptr.unique());

    SharedPtr<Data> const copy(shptr); // NOLINT(performance-unnecessary-copy-initialization)

    CHECK(!shptr.unique());
    CHECK(!copy.unique());
}

/**
 * 1. Create empty shared pointer using default constructor
 * 2. use shared pointer in condition such as an if statement to make sure it evaluates correctly
 */
TEST_CASE("SharedPtr::default constructor creates empty (falsy) pointer", "[core][sharedptr]")
{
    SharedPtr<Data> const shptr;

    CHECK(!shptr);
}

/**
 * 1. Create a shared pointer of type base class to a dynamic object of the child class
 * 2. Call a virtual function using the "->" of the shared pointer and make sure the proper function overload is called.
 */
TEST_CASE("SharedPtr<Base> to Derived calls virtual function correctly", "[core][sharedptr]")
{
    SharedPtr<Data> const shptr(new SubData(2, 4, 6));

    CHECK_EQ(shptr->total(), 12);
}

/**
 * 1. Create a shared pointer to new data
 * 2. Create another shared pointer by using the copy constructor passing the first shared pointer as the parameter
 * 3. Create a third shared pointer to new data
 * 4. Check for equality of the first 2 shared pointers.
 * 5. Check for inequality of the first and 3rd shared pointers.
 */
TEST_CASE("SharedPtr::operator== and operator!= compare pointers correctly", "[core][sharedptr]")
{
    SharedPtr<Data> shptr(new Data(2, 4));
    SharedPtr<Data> copy(shptr);
    SharedPtr<Data> shptr2(new Data(4, 8));

    CHECK_EQ(shptr, copy);
    CHECK_NE(shptr, shptr2);
}

/**
 * 1. Create a shared pointer to new data
 * 2. Create a copy of the shared pointer
 * 3. Reset the first shared pointer with new data
 * 4. Check the data values of the original shared pointer
 * 5. Check the data values of the copied shared pointer
 */
TEST_CASE("SharedPtr::reset with new data isolates copies", "[core][sharedptr]")
{
    SharedPtr<Data> shptr(new Data(2, 4));
    SharedPtr<Data> const copy(shptr);

    CHECK_EQ(copy.useCount(), 2);
    shptr.reset(new Data(6, 8)); // NOLINT(cppcoreguidelines-owning-memory)
    CHECK_EQ(copy.useCount(), 1);

    // shptr holding values we expect?
    CHECK_EQ(shptr->x, 6);
    CHECK_EQ(shptr->y, 8);

    CHECK_EQ(copy->x, 2);
    CHECK_EQ(copy->y, 4);

    shptr.reset();
    CHECK(!shptr);
}
