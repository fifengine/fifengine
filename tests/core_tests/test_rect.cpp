// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <ctime>
#include <vector>

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/rect.h"

using FIFE::Rect;

TEST_CASE("rectangle_interesection")
{
    Rect a(0, 0, 10, 10);

    std::vector<Rect> do_intersect;
#define ADD_RECT(x, y, w, h) do_intersect.push_back(Rect(x, y, w, h))

    ADD_RECT(5, 5, 10, 10);
    ADD_RECT(-5, 5, 10, 10);
    ADD_RECT(-5, -5, 10, 10);
    ADD_RECT(5, -5, 10, 10);

    ADD_RECT(-5, 5, 20, 1);
    ADD_RECT(-5, 5, 10, 1);

    ADD_RECT(5, -5, 1, 20);
    ADD_RECT(5, -5, 1, 10);

    ADD_RECT(5, 5, 3, 3);

    ADD_RECT(-5, -5, 30, 30);

    for (size_t i = 0; i < do_intersect.size(); ++i) {
        CHECK(a.intersects(do_intersect[i]));
        CHECK(do_intersect[i].intersects(a));
    }

    std::vector<Rect> dont_intersect;

#undef ADD_RECT
#define ADD_RECT(x, y, w, h) dont_intersect.push_back(Rect(x, y, w, h))

    ADD_RECT(-5, -5, 4, 4);
    ADD_RECT(-5, -5, 40, 4);
    ADD_RECT(-5, -5, 4, 40);
    ADD_RECT(-5, -5, 4, 4);

    ADD_RECT(15, 15, 4, 4);
    ADD_RECT(15, 15, 40, 4);
    ADD_RECT(15, 15, 4, 40);

    for (size_t i = 0; i < dont_intersect.size(); ++i) {
        CHECK(!a.intersects(dont_intersect[i]));
        CHECK(!dont_intersect[i].intersects(a));
    }
}