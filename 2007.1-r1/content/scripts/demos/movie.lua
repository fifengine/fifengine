function on_engine_start ()
  assert(state_manager.exists('MoviePlayer'))
  --set_next_videofile("test2.avi")
  -- defaults to: ./test.avi
  state_manager.activate('MoviePlayer')
end
