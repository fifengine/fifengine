// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "exception.h"

namespace FIFE
{
    static Logger _log(LM_EXCEPTION);

    Exception::Exception(const std::string& msg) : std::runtime_error(msg), m_what(msg) { }

    Exception::~Exception() throw() { }

    const char* Exception::what() const throw()
    {
        return m_what.c_str();
    }

    void Exception::update()
    {
        m_what = "_[" + getTypeStr() + "]_ , " + getDescription() + " :: " + m_what;
    }
} // namespace FIFE
