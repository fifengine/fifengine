#include <stdlib.h>
#include <libintl.h>

#include "lualib.h"
#include "lauxlib.h"

#include "lua_i18n.h"

int l_setenv(lua_State *L) {
  const char* _name      = luaL_checkstring(L, 1);
  const char* _value     = luaL_checkstring(L, 2);
  int         _overwrite = (lua_gettop(L) == 3) ? 
    luaL_checknumber(L, 3) : 1;
  
  lua_pushnumber(L, setenv(_name, _value, _overwrite));
  return 1;
}

int l_gettext(lua_State *L) {
  const char* msgid = luaL_checkstring(L, 1);
  
  lua_pushstring(L, gettext(msgid));
  return 1;
}

int l_dgettext(lua_State *L) {
  const char* domain = luaL_checkstring(L, 1);
  const char* msgid  = luaL_checkstring(L, 2);
  
  lua_pushstring(L, dgettext(domain, msgid));
  return 1;
}

int l_ngettext(lua_State *L) {
  const char*       msgid        = luaL_checkstring(L, 1);
  const char*       msgid_plural = luaL_checkstring(L, 2);
  unsigned long int n            = luaL_checknumber(L, 3);

  lua_pushstring(L, ngettext(msgid, msgid_plural, n));
  return 1;
}

int l_dngettext(lua_State *L) {
  const char*       domain       = luaL_checkstring(L, 1);
  const char*       msgid        = luaL_checkstring(L, 1);
  const char*       msgid_plural = luaL_checkstring(L, 2);
  unsigned long int n            = luaL_checknumber(L, 3);
  
  lua_pushstring(L, dngettext(domain, msgid, msgid_plural, n));
  return 1;
}

int l_textdomain(lua_State *L) {
  const char* domain = luaL_checkstring(L, 1);
  
  lua_pushstring(L, textdomain(domain));
  return 1;
}

int l_bindtextdomain(lua_State *L) {
  const char* domain    = luaL_checkstring(L, 1);
  const char* directory = luaL_checkstring(L, 2);

  lua_pushstring(L, bindtextdomain(domain, directory));
  return 1;
}

#define export_function(name) {#name, l_ ## name}
static const struct luaL_reg lua_i18n_functions [] = {
  export_function(setenv),
  export_function(gettext),
  export_function(dgettext),
  export_function(ngettext),
  export_function(dngettext),
  export_function(textdomain),
  export_function(bindtextdomain),
  {NULL, NULL}
};

int luaopen_i18n(lua_State *L) {
  luaL_openlib(L, "i18n", lua_i18n_functions, 0);
  return 1;
};  
