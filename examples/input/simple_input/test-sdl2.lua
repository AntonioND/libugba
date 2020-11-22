-- Press some keys, take screenshot, press more keys, take screenshot

run_frames_and_pause(10)
set_input("B", "START", "L", "DOWN", "RIGHT")
continue()

run_frames_and_pause(3)
screenshot("screenshot-1.png")
set_input("A", "SELECT", "R", "UP", "LEFT")
continue()

run_frames_and_pause(3)
screenshot("screenshot-2.png")
continue()

exit()

return 0
