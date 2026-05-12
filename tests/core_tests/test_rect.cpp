// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <ctime>
#include <vector>

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes

// FIFE includes
#include "util/structures/rect.h"

using FIFE::Rect;

template <typename T>
void add_rect(std::vector<Rect>& v, T x, T y, T w, T h)
{
    v.emplace_back(x, y, w, h);
}

TEST_CASE("Rect::intersects with overlapping and non-overlapping rects", "[core][rect]")
{
    Rect a(0, 0, 10, 10);

    std::vector<Rect> do_intersect;
    add_rect(do_intersect, 5, 5, 10, 10);
    add_rect(do_intersect, -5, 5, 10, 10);
    add_rect(do_intersect, -5, -5, 10, 10);
    add_rect(do_intersect, 5, -5, 10, 10);

    add_rect(do_intersect, -5, 5, 20, 1);
    add_rect(do_intersect, -5, 5, 10, 1);

    add_rect(do_intersect, 5, -5, 1, 20);
    add_rect(do_intersect, 5, -5, 1, 10);

    add_rect(do_intersect, 5, 5, 3, 3);

    add_rect(do_intersect, -5, -5, 30, 30);

    for (auto const & r : do_intersect) {
        CHECK(a.intersects(r));
        CHECK(r.intersects(a));
    }

    std::vector<Rect> dont_intersect;

    add_rect(dont_intersect, -5, -5, 4, 4);
    add_rect(dont_intersect, -5, -5, 40, 4);
    add_rect(dont_intersect, -5, -5, 4, 40);
    add_rect(dont_intersect, -5, -5, 4, 4);

    add_rect(dont_intersect, 15, 15, 4, 4);
    add_rect(dont_intersect, 15, 15, 40, 4);
    add_rect(dont_intersect, 15, 15, 4, 40);

    for (auto const & r : dont_intersect) {
        CHECK(!a.intersects(r));
        CHECK(!r.intersects(a));
    }
}
