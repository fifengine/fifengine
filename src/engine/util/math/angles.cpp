// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cassert>
#include <iostream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/layer.h"

#include "angles.h"

namespace FIFE
{
    namespace
    {
        [[nodiscard]] uint32_t normalizeAngleKey(const int32_t angle)
        {
            return static_cast<uint32_t>(((angle % 360) + 360) % 360);
        }

        [[nodiscard]] int32_t toSignedAngle(const uint32_t angle)
        {
            assert(angle <= 360U);
            return static_cast<int32_t>(angle);
        }
    } // namespace

    int32_t getIndexByAngle(int32_t angle, const type_angle2id& angle2id, int32_t& closestMatchingAngle)
    {
        if (angle2id.empty()) {
            return -1;
        }
        if (angle2id.size() == 1) {
            closestMatchingAngle = toSignedAngle(angle2id.begin()->first);
            return angle2id.begin()->second;
        }

        const uint32_t wangle = normalizeAngleKey(angle);
        auto u(angle2id.upper_bound(wangle));
        type_angle2id::const_iterator tmp;

        // take care of the forward wrapping case
        if (u == angle2id.end()) {
            const int32_t ud = toSignedAngle(wangle - (--u)->first);
            const int32_t ld = toSignedAngle(360U - wangle + angle2id.begin()->first);
            if (ud > ld) {
                // wrapped value (first)
                closestMatchingAngle = toSignedAngle(angle2id.begin()->first);
                return angle2id.begin()->second;
            }
            // non-wrapped value
            closestMatchingAngle = toSignedAngle(u->first);
            return u->second;
        }

        // take care of the backward wrapping case
        if (u == angle2id.begin()) {
            tmp = angle2id.end();
            --tmp;
            const int32_t ld = toSignedAngle(u->first - wangle);
            const int32_t ud = toSignedAngle(360U - tmp->first + wangle);
            if (ud > ld) {
                // non-wrapped value (first)
                closestMatchingAngle = toSignedAngle(angle2id.begin()->first);
                return angle2id.begin()->second;
            }
            // wrapped value (last)
            closestMatchingAngle = toSignedAngle(tmp->first);
            return tmp->second;
        }

        // value in the middle...
        const int32_t ud  = toSignedAngle(u->first - wangle);
        const int32_t ucm = toSignedAngle(u->first);
        const int32_t ui  = u->second;
        u--;
        const int32_t ld  = toSignedAngle(wangle - u->first);
        const int32_t lcm = toSignedAngle(u->first);
        const int32_t li  = u->second;

        // if ud and ls is equal then lcm is prefered (next angle)
        if (ud < ld) {
            closestMatchingAngle = ucm;
            return ui;
        }
        closestMatchingAngle = lcm;
        return li;
    }

    int32_t getAngleBetween(const Location& loc1, const Location& loc2)
    {
        ExactModelCoordinate c1 = loc1.getMapCoordinates();
        ExactModelCoordinate c2 = loc2.getMapCoordinates();

        double dy = (c2.y - c1.y);
        double dx = (c2.x - c1.x);
        // add grid rotation to angle, to guarantee uniform angles (not grid based)
        int32_t angle = static_cast<int32_t>(
            round((Mathd::ATan2(-dy, dx) * (180.0 / Mathd::pi())) + loc1.getLayer()->getCellGrid()->getRotation()));
        if (angle < 0) {
            angle += 360;
        }
        angle %= 360;
        return angle;
    }

    Location getFacing(const Location& loc, const int32_t angle)
    {
        Location facing(loc);
        ExactModelCoordinate emc = facing.getMapCoordinates();
        // remove grid rotation from angle, to guarantee uniform angles (not grid based)
        double tmpAngle = static_cast<double>(angle) - loc.getLayer()->getCellGrid()->getRotation();
        emc.x += Mathd::Cos(tmpAngle * (Mathd::pi() / 180.0));
        emc.y -= Mathd::Sin(tmpAngle * (Mathd::pi() / 180.0));
        facing.setMapCoordinates(emc);

        return facing;
    }

    int32_t getAngleBetween(const ExactModelCoordinate& emc1, const ExactModelCoordinate& emc2)
    {
        double dy = (emc2.y - emc1.y);
        double dx = (emc2.x - emc1.x);

        int32_t angle = static_cast<int32_t>(round(Mathd::ATan2(-dy, dx) * (180.0 / Mathd::pi())));
        if (angle < 0) {
            angle += 360;
        }
        angle %= 360;
        return angle;
    }

    ExactModelCoordinate getFacing(const ExactModelCoordinate& emc, const int32_t angle)
    {
        ExactModelCoordinate result = emc;
        result.x += Mathd::Cos(static_cast<double>(angle) * (Mathd::pi() / 180.0));
        result.y -= Mathd::Sin(static_cast<double>(angle) * (Mathd::pi() / 180.0));
        return result;
    }
} // namespace FIFE
