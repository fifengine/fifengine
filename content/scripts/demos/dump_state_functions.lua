--[[

prints available Lua functions
run: ./guimap_test content/scripts/demos/dump_state_functions.lua

To get a cleaned up listing I use something like this:
./guimap_test content/scripts/demos/dump_state_functions.lua | \
 grep -e "function " -e "table " >out


You should of course ignore the functions "empty_string" & "explain";
Heisenberg strikes again :-)
--]]

table_nest_level = 0


function explain(name, Type, visited_tables)
  if type(Type) == "function" then
    if name == "on_engine_start" then return end
    print(string.rep("  ",table_nest_level) .. "function " .. name)
  elseif type(Type) == "table" then
    if visited_tables[Type] then return end
    visited_tables[Type] = 1

    if name == "_G" then return end  -- skip _G ; otherwise this loops forever
    if name == "loadlib" or name == "loaded" or name == "loaders" then return end
    print(string.rep("  ",table_nest_level) .. "** table " .. name .. " **")
    table_nest_level = table_nest_level + 1
    for i, j in pairs(Type) do
      explain(i, j,visited_tables)
    end
    table_nest_level = table_nest_level - 1
  end
end

function on_engine_start()
  print("DUMP_START_HERE")
  for i, j in pairs(_G) do
    explain(i, j,{})
  end
  print("DUMP_END_HERE")
  Timer.singleshot( 1000, engine.stop )
end
