-- Press some keys, take screenshot, press more keys, take screenshot

run_frames_and_pause(10)
keys_hold("B", "START", "L", "DOWN", "RIGHT")
continue()

run_frames_and_pause(3)
screenshot("screenshot-1.png")
keys_release("B", "START", "L", "DOWN", "RIGHT")
keys_hold("A", "SELECT", "R", "UP", "LEFT")
continue()

run_frames_and_pause(3)
screenshot("screenshot-2.png")
keys_release("A", "SELECT", "R", "UP", "LEFT")
continue()

exit()

return 0
