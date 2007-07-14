-- class.lua
function class(base,ctor)
  local c = {}     -- a new class instance
  if not ctor and type(base) == 'function' then
      ctor = base
      base = nil
  elseif type(base) == 'table' then
   -- our new class is a shallow copy of the base class!
      for i,v in pairs(base) do
          c[i] = v
      end
      c._base = base
  end
  -- the class will be the metatable for all its objects,
  -- and they will look up their methods in it.
  c.__index = c

  -- expose a ctor which can be called by <classname>(<args>)
  local mt = {}
  mt.__call = function(class_tbl,...)
    local obj = {}
    setmetatable(obj,c)
    if ctor then
       ctor(obj,unpack(arg))
    else 
    -- make sure that any stuff from the base class is initialized!
       if base and base.init then
         base.init(obj,unpack(arg))
       end
    end
    return obj
  end
  c.init = ctor
  c.is_a = function(self,klass)
      local m = getmetatable(self)
      while m do 
         if m == klass then return true end
         m = m._base
      end
      return false
    end
  setmetatable(c,mt)
  return c
end