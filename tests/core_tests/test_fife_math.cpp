// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cmath>

// Platform specific includes
#include <catch2/catch_test_macros.hpp>

// FIFE includes
#include "util/math/fife_math.h"

using FIFE::Math;
using FIFE::Mathd;

TEST_CASE("Math<int32_t> - integer traits mirror legacy double-based behavior", "[core][fife_math]")
{
    // Values that the old `static_cast<int32_t>(...)` expressions around
    // Mathd produced (pi: 3, zero tolerance: 0).
    REQUIRE(Math<int32_t>::pi() == 3);
    REQUIRE(Math<int32_t>::zeroTolerance() == 0);

    REQUIRE(Math<int32_t>::Cos(0) == 1);
    REQUIRE(Math<int32_t>::Sin(0) == 0);
    REQUIRE(Math<int32_t>::Cos(1) == 0); // trunc of 0.5403
    REQUIRE(Math<int32_t>::Sqrt(25) == 5);
}

TEST_CASE("Math<double> - happy path", "[core][fife_math]")
{
    REQUIRE(Math<double>::Cos(0) == 1.0);
    REQUIRE(Mathd::Sqrt(25.0) == 5.0);

    REQUIRE(Mathd::pi() > 3.14159);
    REQUIRE(Mathd::pi() < 3.14160);
    REQUIRE(Math<double>::zeroTolerance() == 1e-08);
}
