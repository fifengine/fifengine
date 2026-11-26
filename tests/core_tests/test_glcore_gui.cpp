// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <memory>

#include "glcore_test_env.h"
#include "gui/fifechan/fifechanmanager.h"
#include "video/dummy/renderbackendsurface.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

TEST_CASE("FifechanManager dispatches and resizes both new backend names", "[glcore][gui]")
{
    for (auto const * name : {"OpenGL_CORE", "GLES2"}) {
        FIFE::RenderBackendSurface backend({0, 0, 0, 255}, 160, 90);
        backend.init("");
        backend.createMainScreen("headless", "");

        auto manager = std::make_unique<FIFE::FifechanManager>();
        manager->setConsoleEnabled(false);
        CHECK_NOTHROW(manager->init(name, 160, 90));
        CHECK_NOTHROW(manager->resizeTopContainer(0, 0, 160, 90));
        CHECK(manager->getFifechanGUI() != nullptr);
    }
}
