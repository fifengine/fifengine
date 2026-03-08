// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "version.h"
%}

namespace FIFE {
    inline const char* getVersion();
    inline const char* getVersionShort();
    inline int getMajor();
    inline int getMinor();
    inline int getPatch();
    inline const char* getHash();
    inline const int getVersionId();
} //FIFE