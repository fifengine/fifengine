function f2_new_game ()
  set_next_mapfile('maps/arvillag.map')
  state_manager.activate('MapView')
  dofile('content/scripts/ingame_interface.lua')
  dofile('content/scripts/console.lua')
end

function on_engine_start ()
  dofile('content/scripts/bindings.lua')
  if (_tmp_map_to_load ~= nil) then
    set_next_mapfile(_tmp_map_to_load)
    state_manager.activate('MapView')
  else
    dofile('content/scripts/mainmenu.lua')
  end
end
