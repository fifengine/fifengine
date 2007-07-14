
-- returns true if 'table' contains 'key'
function is_defined(table, key)
  assert(type(table) == 'table')
  return rawget(table, key) ~= nil
end

-- case-insensitive string.find; lower-case pattern! 
function string.icase_find(str, pattern)
  local lower_s = string.lower(str)
  return string.find(lower_s, pattern)
end


-- takes a table, returns a font userdata
function make_font(cfg)

  local new_font = nil
  function handle_color(col)
    assert(new_font ~= nil) -- has to exist by now
   
    -- assume userdata means this is a 'Color', setColor
    -- will barf if not
    if type(cfg[col]) == 'userdata' then
      new_font:setColor(cfg[col])
    elseif type(cfg[col]) == 'table' then
      if is_defined(cfg[col], 'r') and is_defined(cfg[col], 'b') and
        is_defined(cfg[col], 'b') then
        new_font:setColor(cfg[col].r, cfg[col].g, cfg[col].b)
      end
    else
      error('Invalid font-color - I have no idea what you want me to do :-(')
    end
  end

  if type(cfg) ~= 'table' then error('table expected!') end
  if not is_defined(cfg, 'file') then error('No "file" defined!') end

  if string.icase_find(cfg.file, '%.aaf$') ~= nil then
    -- AAF can be coloured, but no 'size' attribute
    new_font = AAFont(cfg.file)
    if is_defined(cfg, 'color') then handle_color('color')
    elseif is_defined(cfg, 'colour') then handle_color('colour') end

  elseif string.icase_find(cfg.file, '%.ttf$') ~= nil then
    -- TTF must have size, can have colour
    -- default size if undefined
    local size = 20
    if is_defined(cfg, 'size') then size = cfg.size end
    new_font = TTFont(cfg.file, size)
    if is_defined(cfg, 'color') then handle_color('color')
    elseif is_defined(cfg, 'colour') then handle_color('colour') end

  else
    -- assume ImageFont; must have glyphs, nothing else
    if not is_defined(cfg, 'glyphs') then 
      error('ImageFont needs a "glyphs" definition!')
    end
    new_font = ImageFont(cfg.file, cfg.glyphs)

  end

  assert(new_font ~= nil)
  return new_font
end

-- should really be at the top of the file; placing it at the bottom
-- seems an easy way to make the functions global
module(..., package.seeall)
