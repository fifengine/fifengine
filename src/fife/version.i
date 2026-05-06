// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "version.hpp"
%}

%inline %{
namespace FIFE {
    inline const char* get_name() { return app_version::get_name(); }
    inline const char* get_version() { return app_version::get_version(); }
    inline const char* get_license() { return app_version::get_license(); }
    inline const char* get_description() { return app_version::get_description(); }
    inline const char* get_homepage() { return app_version::get_homepage(); }
    inline const char* get_copyright() { return app_version::get_copyright(); }
    inline int get_major() { return app_version::get_major(); }
    inline int get_minor() { return app_version::get_minor(); }
    inline int get_patch() { return app_version::get_patch(); }
    inline int get_version_id() { return app_version::get_version_id(); }
} //FIFE
%}
