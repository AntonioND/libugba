-- Press some keys, release keys, repeat... and take screenshot

run_frames_and_pause(3)
keys_hold("B", "START")
continue()

run_frames_and_pause(3)
keys_hold("L", "DOWN", "RIGHT")
keys_release("B", "START")
continue()

run_frames_and_pause(3)
keys_hold("R", "UP", "LEFT")
keys_release("L", "DOWN", "RIGHT")
continue()

run_frames_and_pause(3)
keys_hold("A", "SELECT")
keys_release("R", "UP", "LEFT")
continue()

run_frames_and_pause(3)
keys_release("A", "SELECT")
continue()

run_frames_and_pause(3)
screenshot()
continue()

exit()

return 0
