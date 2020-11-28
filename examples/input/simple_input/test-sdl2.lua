-- Press some keys, release keys, repeat... and take screenshot

run_frames_and_pause(3)
keys_hold("B", "START")

run_frames_and_pause(3)
keys_hold("L", "DOWN", "RIGHT")
keys_release("B", "START")

run_frames_and_pause(3)
keys_hold("R", "UP", "LEFT")
keys_release("L", "DOWN", "RIGHT")

run_frames_and_pause(3)
keys_hold("A", "SELECT")
keys_release("R", "UP", "LEFT")

run_frames_and_pause(3)
keys_release("A", "SELECT")

run_frames_and_pause(3)
screenshot()

exit()

return 0
