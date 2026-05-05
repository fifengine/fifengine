// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "exception.h"

namespace FIFE
{
    static Logger _log(LM_EXCEPTION);

    Exception::Exception(std::string const & msg) : std::runtime_error(msg), m_what(msg)
    {
    }

    Exception::~Exception() noexcept = default;

    char const * Exception::what() const noexcept
    {
        return m_what.c_str();
    }

    void Exception::update()
    {
        m_what = "_[" + getTypeStr() + "]_ , " + getDescription() + " :: " + m_what;
    }
} // namespace FIFE
