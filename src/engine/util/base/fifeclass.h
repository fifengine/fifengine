// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_CLASS_H
#define FIFE_CLASS_H

// Standard C++ library includes
#include <cassert>
#include <cstddef>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
namespace FIFE
{

    using fifeid_t = std::size_t;
    /** Base class for all fife classes
     * Used e.g. to track instances over swig conversion
     */
    class FifeClass
    {
    public:
        FifeClass() : m_fifeid(m_curid++) { }

        virtual ~FifeClass() = default;

        /** Gets unique id of this instance inside the engine
         */
        fifeid_t getFifeId()
        {
            return m_fifeid;
        }

    private:
        fifeid_t m_fifeid;
        static fifeid_t m_curid;
    };
} // namespace FIFE

#endif
