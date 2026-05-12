// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_FIFE_UNITTEST_H
#define FIFE_FIFE_UNITTEST_H

#include <catch2/catch_test_macros.hpp>

#ifndef CHECK_EQ
    #define CHECK_EQ(lhs, rhs) CHECK((lhs) == (rhs))
#endif

#ifndef CHECK_NE
    #define CHECK_NE(lhs, rhs) CHECK((lhs) != (rhs))
#endif

#endif
