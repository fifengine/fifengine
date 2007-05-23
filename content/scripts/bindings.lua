dofile('content/scripts/keyboard_consts.lua')

-- this little baby could import the 'keyboard_consts' variables 
-- as globals; but a simpler solution exists (below).

--setmetatable(_G, { __index = function(_, n)
--  if rawget(keyboard_consts, n) ~= nil then
--    return rawget(keyboard_consts, n)
--  end
--  end
--})

-- Event IDS -- Keep in sync with events.h
GO_NORTH = 1
GO_SOUTH = 2
GO_WEST  = 3
GO_EAST  = 4

--- Event IDs for Testing MapView
TEST_TOGGLE_OBJECTS   = 201
TEST_TOGGLE_FLOOR     = 202
TEST_TOGGLE_ROOF      = 203
TEST_TOGGLE_TILE_GRID = 204
TEST_TOGGLE_OBJ_GRID  = 205
TEST_CHANGE_ELEVATION = 206
TEST_INCREASE_ALPHA   = 207
TEST_DECREASE_ALPHA   = 208
TEST_TOGGLE_COORDS    = 209
TEST_TOGGLE_FADEOUT   = 210

GUI_TOGGLE_CONSOLE  = 602

-- Event IDs for Engine
MAKE_SCREENSHOT = 401
QUIT_GAME = 402

-- wrapper around the 'mapKeyToEventId' function
-- uses keyboard consts defined in another table (to keep the
-- number of global vars down)
local function key_setup(context, T)
  for key, action in pairs(T) do
    inputmanager.mapKeyToEventId(context, rawget(keyboard_consts, key), action)
  end
end

-- SDLK_PRINT doesn't work with UNICODE activated for some reason.
-- so we use 'p' for now.
key_setup('engine', {
    SDLK_ESCAPE  = QUIT_GAME,
    SDLK_p       = MAKE_SCREENSHOT,
    SDLK_F10     = GUI_TOGGLE_CONSOLE
})

key_setup('map_view', {
    SDLK_UP      = GO_NORTH,
    SDLK_DOWN    = GO_SOUTH,
    SDLK_LEFT    = GO_EAST,
    SDLK_RIGHT   = GO_WEST,
    SDLK_e       = TEST_CHANGE_ELEVATION,
    SDLK_o       = TEST_TOGGLE_OBJECTS,
    SDLK_f       = TEST_TOGGLE_FLOOR,
    SDLK_r       = TEST_TOGGLE_ROOF,
    SDLK_s       = TEST_INCREASE_ALPHA,
    SDLK_x       = TEST_DECREASE_ALPHA,
    SDLK_t       = TEST_TOGGLE_TILE_GRID,
    SDLK_g       = TEST_TOGGLE_OBJ_GRID,
    SDLK_c       = TEST_TOGGLE_COORDS,
    SDLK_d       = TEST_TOGGLE_FADEOUT,
})

-- equivalent to above:

--inputmanager.mapKeyToEventId('engine',SDLK_ESCAPE,QUIT_GAME)
--inputmanager.mapKeyToEventId('engine',SDLK_PRINT,MAKE_SCREENSHOT)

--inputmanager.mapKeyToEventId('map_view',SDLK_UP,  GO_NORTH)
--inputmanager.mapKeyToEventId('map_view',SDLK_DOWN,GO_SOUTH)
--inputmanager.mapKeyToEventId('map_view',SDLK_LEFT,GO_EAST)
--inputmanager.mapKeyToEventId('map_view',SDLK_RIGHT,GO_WEST)
