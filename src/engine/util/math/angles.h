// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_ANGLES_H
#define FIFE_ANGLES_H

// Standard C++ library includes
#include <map>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/location.h"

namespace FIFE
{
    typedef std::map<uint32_t, int32_t> type_angle2id;

    /** Returns id for given angle from angle2id map
     * in case there are no elements in the map, negative value is returned
     */
    int32_t getIndexByAngle(int32_t angle, const type_angle2id& angle2id, int32_t& closestMatchingAngle);

    /** Gets angle of vector defined by given locations
     *  @return angle in polar coordinates between the line
     *  defined by the two locations and the horizontal axis (East <-> West)
     */
    int32_t getAngleBetween(const Location& loc1, const Location& loc2);

    /** Gets facing location defined by given angle and location
     *  @return facing location
     */
    Location getFacing(const Location& loc, const int32_t angle);

    /** Gets angle of vector defined by given map coordinates
     *  @return angle in polar coordinates between the line
     *  defined by the two map coordinates and the horizontal axis (East <-> West)
     */
    int32_t getAngleBetween(const ExactModelCoordinate& emc1, const ExactModelCoordinate& emc2);

    /** Gets facing map coordinate defined by given angle and map coordinate
     *  @return facing map coordinate
     */
    ExactModelCoordinate getFacing(const ExactModelCoordinate& emc, const int32_t angle);
} // namespace FIFE

#endif