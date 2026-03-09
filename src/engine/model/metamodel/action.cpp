// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <iostream>
#include <string>
#include <utility>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "audio/actionaudio.h"
#include "util/base/exception.h"

#include "action.h"

namespace FIFE
{
    Action::Action(std::string  identifier) : m_id(std::move(identifier)), m_duration(0), m_visual(nullptr), m_audio(nullptr)
    {
    }

    Action::~Action()
    {
        delete m_visual;
        delete m_audio;
    }
} // namespace FIFE
