// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_ANGLES_H
#define FIFE_ANGLES_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <map>

// 3rd party library includes

// FIFE includes
#include "model/structures/location.h"

namespace FIFE
{
    using type_angle2id = std::map<uint32_t, int32_t>;
    /** Returns id for given angle from angle2id map
     * in case there are no elements in the map, negative value is returned
     */
    FIFE_API int32_t getIndexByAngle(int32_t angle, type_angle2id const & angle2id, int32_t& closestMatchingAngle);

    /** Gets angle of vector defined by given locations
     *  @return angle in polar coordinates between the line
     *  defined by the two locations and the horizontal axis (East <-> West)
     */
    FIFE_API int32_t getAngleBetween(Location const & loc1, Location const & loc2);

    /** Gets facing location defined by given angle and location
     *  @return facing location
     */
    FIFE_API Location getFacing(Location const & loc, int32_t angle);

    /** Gets angle of vector defined by given map coordinates
     *  @return angle in polar coordinates between the line
     *  defined by the two map coordinates and the horizontal axis (East <-> West)
     */
    FIFE_API int32_t getAngleBetween(ExactModelCoordinate const & emc1, ExactModelCoordinate const & emc2);

    /** Gets facing map coordinate defined by given angle and map coordinate
     *  @return facing map coordinate
     */
    FIFE_API ExactModelCoordinate getFacing(ExactModelCoordinate const & emc, int32_t angle);
} // namespace FIFE

#endif
