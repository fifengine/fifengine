#ifndef LUA_GETTEXT_H
#define LUA_GETTEXT_H

#include "lua.h"

static int l_setenv(lua_State *L);

static int l_gettext(lua_State *L);
static int l_dgettext(lua_State *L);
static int l_ngettext(lua_State *L);
static int l_dngettext(lua_State *L);

static int l_textdomain(lua_State *L);
static int l_bindtextdomain(lua_State *L);

#endif
