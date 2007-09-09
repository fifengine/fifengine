#ifndef LUA_STACK_GUARD_H
#define LUA_STACK_GUARD_H

#include "lua.hpp"

class LuaStackguard {
  public:
    LuaStackguard(lua_State *L, bool take_action = true);
    ~LuaStackguard();
  private:
    int m_top;
    bool m_act;
    lua_State* m_state;
};

#endif
