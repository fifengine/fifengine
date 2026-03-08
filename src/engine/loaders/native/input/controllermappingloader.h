// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_INPUT_LOADERS_CONTROLLER_MAPPING_H
#define FIFE_INPUT_LOADERS_CONTROLLER_MAPPING_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE
{
    /** Gamepad mapping loader.
     */
    class ControllerMappingLoader
    {
    public:
        ControllerMappingLoader() { };

        /** Loads mapping from file.
         */
        void load(const std::string& filename);
    };
} // namespace FIFE
#endif