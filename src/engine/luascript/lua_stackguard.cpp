#include <cassert>
#include "lua_stackguard.h"
#include "log.h"

LuaStackguard::LuaStackguard(lua_State *L, bool take_action) {
  assert(L);
  m_state = L;
  m_act = take_action;
  m_top = lua_gettop(m_state); 
}

LuaStackguard::~LuaStackguard() {
  int now_top = lua_gettop(m_state);
  if (now_top > m_top) {
    FIFE::Log("Lua_Stackguard", FIFE::Log::LEVEL_WARN) <<
      "Stack-integrity warning: " << now_top << " > " << m_top;
    if (m_act)
      lua_settop(m_state, m_top);
  }
}
