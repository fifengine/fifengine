// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_CLASS_H
#define FIFE_CLASS_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cassert>
#include <cstddef>

// 3rd party library includes

// FIFE includes
namespace FIFE
{

    using fifeid_t = std::size_t;
    /** Base class for all fife classes
     * Used e.g. to track instances over swig conversion
     */
    class /*FIFE_API*/ FifeClass
    {
        public:
            FifeClass() : m_fifeid(m_curid++)
            {
            }

            virtual ~FifeClass() = default;

            /** Gets unique id of this instance inside the engine
             */
            fifeid_t getFifeId() const
            {
                return m_fifeid;
            }

        private:
            fifeid_t m_fifeid;
            static inline fifeid_t m_curid = 0;
    };
} // namespace FIFE

#endif
