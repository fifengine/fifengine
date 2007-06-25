
-- read lines of a textfile and return content as table
-- if table_mode == 0 the table is used as an array (key
-- is line number, value is the line) otherwise the line
-- is used as the key (and the value is just a dummy).
function lines_into_table(filename, table_mode)
  foo = {}
  for line in io.lines(filename) do
    if table_mode == 0 then
      table.insert(foo, line)
    else
      foo[line] = true
    end
  end
  return foo
end

-- used to determine lua specific API
lua_globals = lines_into_table("lua_globals", 1)
lua_tables  = lines_into_table("lua_table_funcs", 1)


-- return urls of commands
function lua_doc_url(name)
  return "http://www.lua.org/manual/5.1/manual.html#pdf-" .. name
end

function fife_doc_url(name)
  return "http://wiki.fifengine.de/API_Docs#" .. name
end

-- print stuff to wrap content
function print_header()
  print('<html><head>')
  print('<style type="text/css">\n' .. 
  '.lua { background-color:#ffee00; }\n' ..
  '.fife { background-color:#00eeff; }\n' ..
  '</style></head>\n<body>\n')
end
function print_footer()
  print('</body></html>')
end
function print_begin_section()
  print('<p>')
end
function print_end_section()
  print('</p><br>')
end

function print_global(t)
  local doc_link_f
  assert(type(t) == 'table' and rawget(t, 'name') ~= nil and rawget(t, 'source') ~= nil)
  if t.source == "lua" then doc_link_f = lua_doc_url else doc_link_f = fife_doc_url end
  print('<div class="' .. t.source .. '">' .. '<a href="' ..
    doc_link_f(t.name) .. '">' .. t.name .. '</a>' .. '</div>')
end

function print_obj(t)
  local doc_link_f
  assert(type(t) == 'table' and rawget(t, 'name') ~= nil and rawget(t, 'source') ~= nil)
  if t.source == "lua" then doc_link_f = lua_doc_url else doc_link_f = fife_doc_url end
  print('<div class="' .. t.source .. '">' .. '<a href="'.. doc_link_f(t.name) ..
    '">' .. t.name .. '</a></div>')
end

fife_globals = lines_into_table(arg[1], 0)
fife_tables  = lines_into_table(arg[2], 0)

table.sort(fife_globals)
table.sort(fife_tables)

print_header()

print_begin_section()
for _, i in pairs(fife_globals) do
  if rawget(lua_globals, i) == nil then
    print_global { source = "fife", name = i }
  else
    print_global { source = "lua", name = i }
  end
end
print_end_section()

print_begin_section()
for _, i in pairs(fife_tables) do
  if rawget(lua_tables, i) == nil then
    print_obj { source = "fife", name = string.gsub(i, " ", ".") }
  else
    print_obj { source = "lua", name = string.gsub(i, " ", ".") }
  end
end
print_end_section()

print_footer()
