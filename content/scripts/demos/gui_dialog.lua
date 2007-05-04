-- this assumes sqlite3 and the Lua binding are available
require('sqlite3')

-- furthermore this database has to be there
database = 'demo.db'
db = sqlite3.open(database)

-- backend

Dialog = {} 

function Dialog:new(o)
  o = o or {}
  setmetatable(o, self)
  self.__index = self
  o.owner = o.owner or 1
  o.stage = o.stage or 1
  o.num_options = 0
  o.count = 0
  o.options = {}
  o.db_handle = db 
  o:loadFromDB()
  return o
end

function Dialog:getNumOptions()
  return self.count
end

function Dialog:saveRowAs(row, k)
  if rawget(row, 'post_cond') then
    row.post_cond = loadstring('return ' .. row.post_cond)
  end
  if rawget(row, 'action') then
    row.action = loadstring(row.action)
  end
  self.options[k] = row
end

function Dialog:loadFromDB()
  local counter = 0
  local row
  local stmt = assert(self.db_handle:prepare('SELECT * FROM dialog_stage_view WHERE owner_id = '
  .. self.owner .. ' AND stage_id = ' .. self.stage))

  for row in stmt:rows() do
    if rawget(row, 'pre_cond') ~= nil then
      local cond_loaded = loadstring('return ' .. row.pre_cond)
      --print('cnd: ' .. row.pre_cond .. ' result: ', cond_loaded())
      if cond_loaded() then
        self:saveRowAs(row, counter)
        counter = counter + 1
      end
    else
      if row.piece_id > 0 then
        self:saveRowAs(row, counter)
        counter = counter + 1
      else
        self.text = row.as_string
      end
    end
  end
  self.count = counter
  print('loaded ' .. counter .. ' options')
end

function Dialog:chooseOption(k)
  local opt = self.options[k]
  if type(opt) == 'table' then
    --print('* option *')
    --for i, j in pairs(opt) do
    --  print(i, ':', j)
    --end
    local next_stage = opt.next_dialog_stage
    if type(rawget(opt, 'post_cond')) == 'function' then
      if not opt.post_cond() then
        --print('POST-Condition failed')
        next_stage = opt.next_on_fail
      end
    end
    if type(rawget(opt, 'action')) ~= nil then
      if type(opt.action) == 'function' then
        --print('calling action')
        opt.action()
      end
    end
    self:setStage(next_stage)
  else
    error('invalid option in Dialog: ' .. k)
  end
end

function Dialog:setStage(new_stage)
  self.options = {}
  self.count = 0
  --print('switching to '.. new_stage)
  if new_stage == 0 then
    print('* dialog exited')
    self.stage = 0
  else
    self.stage = new_stage
    self:loadFromDB()
  end
end

-- visual

gui.actions.dialog_click_cb = function()
  local i = gui.list:getSelected()
  local next_stage = dialog_src.options[i].next_dialog_stage
  print('Selected: ' .. i .. ' db-next:' .. next_stage
    .. ' text: ' .. dialog_src.options[i].as_string)
  gui.dialog_text:addRow('< '..dialog_src.options[i].as_string)
  dialog_src:setStage(next_stage)
  if dialog_src.count == 0 then
    gui.top:setVisible(false)
    if next_stage == 0 then
      l = Label('The End -- press ESC')
    else
      l = Label('ERROR in Dialog stage: ' .. dialog_src.stage .. ' -- press ESC')
    end
    print(l:getCaption())
    l:setPosition(200, 300)
    guimanager.add(l)
  else
    update_dialog_gui()
  end
end

function update_dialog_gui()
  gui.dialog_text:addRow('> ' ..dialog_src.text)
  gui.list_model = ListModel()
  for i = 1, dialog_src.count do
    --print(dialog_src.options[i-1].as_string)
    gui.list_model:addElement(dialog_src.options[i-1].as_string)
  end
  gui.list:setListModel(gui.list_model)
end

function on_engine_start()
  
  gui.font = TTFont('content/fonts/FreeMono.ttf', 16)
  gui.font:setColor(0, 0, 0)
  gui.con = Container()
  gui.con:setSize(600, 300)
  
  gui.top = Window() --gui.con, i18n.gettext('Move me around'))
  gui.top:setCaption('rpg dialog demo')
  
  dark = Color(20, 50, 10)
  gui.top:setBaseColor(dark)
  
  gui.top:setSize(610, 530)
  gui.top:setPosition(5, 5)
  gui.top:setOpaque(false)
  gui.top:setTitleBarHeight(30)
  guimanager.add(gui.top)

  gui.scroll_out = ScrollArea()
  gui.scroll_out:setSize(600, 300)
  gui.top:add(gui.scroll_out)
  
  gui.scroll_in = ScrollArea()
  gui.scroll_in:setSize(600, 300)
  gui.scroll_in:setPosition(0, 305)
  gui.scroll_in:setScrollPolicy(1, 2)
  gui.top:add(gui.scroll_in)

  --gui.list_model = ListModel('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p')

  gui.dialog_text = TextBox('- dialog start -')
  gui.dialog_text:setEditable(false)
  gui.dialog_text:setFont(gui.font)
  gui.dialog_text:setSize(600, 300)
  gui.scroll_out:setContent(gui.dialog_text)

  dialog_src = Dialog:new( { owner = 2 } )

  gui.list = ListBox(ListModel())
  gui.list:setFont(gui.font)
  --gui.list:setSelected(0)
  gui.list:setEventId('dialog_click_cb')
  --gui.scroll:setContent(gui.list)
  gui.scroll_in:setContent(gui.con)
  gui.con:add(gui.list)

  update_dialog_gui()
  local h = gui.list_model:getNumberOfElements() * 23
  gui.list:setSize(600, h)
  gui.con:setSize(600, h)

end
