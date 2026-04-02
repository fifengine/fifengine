// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_INPUT_SAVERS_CONTROLLER_MAPPING_H
#define FIFE_INPUT_SAVERS_CONTROLLER_MAPPING_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE
{
    /** Gamepad mapping saver.
     */
    class ControllerMappingSaver
    {
    public:
        ControllerMappingSaver() = default;

        /** Saves mapping to file.
         */
        void save(const std::string& data, const std::string& filename);
    };
} // namespace FIFE
#endif
