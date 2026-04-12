// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_INPUT_SAVERS_CONTROLLER_MAPPING_H
#define FIFE_INPUT_SAVERS_CONTROLLER_MAPPING_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes

namespace FIFE
{
    /** Gamepad mapping saver.
     */
    class /*FIFE_API*/ ControllerMappingSaver
    {
    public:
        ControllerMappingSaver() = default;

        /** Saves mapping to file.
         */
        void save(const std::string& data, const std::string& filename);
    };
} // namespace FIFE
#endif
