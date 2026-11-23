// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cmath>
#include <cstdint>

// Platform specific includes
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

// FIFE includes
#include "util/structures/point.h"

using Catch::Approx;
using FIFE::DoublePoint;
using FIFE::DoublePoint3D;
using FIFE::Point;
using FIFE::Point3D;

TEST_CASE("PointType2D<double> - happy path", "[core][point]")
{
    DoublePoint p(3.0, 4.0);
    REQUIRE(p.x == 3.0);
    REQUIRE(p.y == 4.0);
    REQUIRE(p.length() == Approx(5.0));

    DoublePoint q;
    REQUIRE(q.x == 0.0);
    REQUIRE(q.y == 0.0);

    DoublePoint r(-3.0, -4.0);
    REQUIRE(r.length() == Approx(5.0));
}

TEST_CASE("PointType2D<double>::normalize - happy path", "[core][point]")
{
    DoublePoint p(3.0, 4.0);
    p.normalize();
    REQUIRE(p.x == Approx(0.6));
    REQUIRE(p.y == Approx(0.8));

    DoublePoint unit(1.0, 0.0);
    unit.normalize();
    REQUIRE(unit.x == Approx(1.0));
    REQUIRE(unit.y == Approx(0.0));
}

TEST_CASE("PointType2D<double>::normalize - zero length is NaN (current behavior)", "[core][point]")
{
    DoublePoint p(0.0, 0.0);
    p.normalize();
    REQUIRE(std::isnan(p.x));
    REQUIRE(std::isnan(p.y));
}

TEST_CASE("PointType2D<double>::rotate - happy path", "[core][point]")
{
    DoublePoint p(1.0, 0.0);
    p.rotate(90.0);
    REQUIRE(p.x == Approx(0.0).margin(1e-12));
    REQUIRE(p.y == Approx(1.0));

    DoublePoint unrotated(1.0, 0.0);
    unrotated.rotate(0.0);
    REQUIRE(unrotated.x == Approx(1.0));
    REQUIRE(unrotated.y == Approx(0.0));
}

TEST_CASE("PointType2D<double>::rotate(origin, angle) - happy path", "[core][point]")
{
    // Current behavior: the point is rotated around the origin as
    // new_p = R * (p - origin) (the origin is not added back).
    DoublePoint p(1.0, 1.0);
    DoublePoint origin(0.5, -0.25);
    p.rotate(origin, 90.0);
    REQUIRE(p.x == Approx(-1.25));
    REQUIRE(p.y == Approx(0.5).margin(1e-12));
}

TEST_CASE("PointType2D<double> - operators", "[core][point]")
{
    DoublePoint a(1.0, 2.0);
    DoublePoint b(3.0, 4.0);
    REQUIRE(a + b == DoublePoint(4.0, 6.0));
    REQUIRE(b - a == DoublePoint(2.0, 2.0));
    REQUIRE(a != b);
    REQUIRE(DoublePoint(1.0, 2.0) == DoublePoint(1.0, 2.0));
}

TEST_CASE("PointType2D<int32_t> - happy path", "[core][point]")
{
    Point p(3, 4);
    REQUIRE(p.x == 3);
    REQUIRE(p.y == 4);
    REQUIRE(p.length() == 5);

    Point q(3, 4);
    q.normalize();
    REQUIRE(q == Point(0, 0));

    Point r(1, 0);
    r.rotate(90);
    REQUIRE(r == Point(0, 0));

    // Degenerate integer cases (documented current behavior).
    Point s(1, 0);
    s.rotate(59);
    REQUIRE(s == Point(1, 0));

    Point t(0, 0);
    REQUIRE(t.length() == 0);
    t.normalize();
    REQUIRE(t == Point(0, 0));
}

TEST_CASE("PointType2D<int32_t> - operators", "[core][point]")
{
    Point a(1, 2);
    Point b(3, 4);
    REQUIRE(a + b == Point(4, 6));
    REQUIRE(b - a == Point(2, 2));
    REQUIRE(a != b);
    REQUIRE(Point(1, 2) == Point(1, 2));
}

TEST_CASE("PointType3D<double> - happy path", "[core][point]")
{
    DoublePoint3D p(3.0, 4.0, 0.0);
    REQUIRE(p.length() == Approx(5.0));

    DoublePoint3D q(3.0, 4.0, 0.0);
    q.normalize();
    REQUIRE(q.x == Approx(0.6));
    REQUIRE(q.y == Approx(0.8));
    REQUIRE(q.z == 0.0);
}

TEST_CASE("PointType3D<double>::normalize - zero length is NaN (current behavior)", "[core][point]")
{
    DoublePoint3D p(0.0, 0.0, 0.0);
    p.normalize();
    REQUIRE(std::isnan(p.x));
    REQUIRE(std::isnan(p.y));
    REQUIRE(std::isnan(p.z));
}

TEST_CASE("PointType3D<int32_t> - happy path", "[core][point]")
{
    Point3D p(3, 4, 0);
    REQUIRE(p.length() == 5);

    Point3D q(3, 4, 0);
    q.normalize();
    REQUIRE(q == Point3D(0, 0, 0));

    Point3D z(0, 0, 0);
    z.normalize();
    REQUIRE(z == Point3D(0, 0, 0));
}
