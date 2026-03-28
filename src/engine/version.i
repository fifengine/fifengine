// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "version.hpp"
%}

namespace FIFE {
    inline const char* get_name();
    inline const char* get_version();
    inline const char* get_license();
    inline const char* get_description();
    inline const char* get_homepage();
    inline const char* get_copyright();
    inline int get_major();
    inline int get_minor();
    inline int get_patch();
    inline int get_version_id();
} //FIFE
