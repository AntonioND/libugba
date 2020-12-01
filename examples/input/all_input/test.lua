-- Press some keys, release keys, repeat... and take screenshots

run_frames_and_pause(5)

keys_hold("B", "START", "LEFT", "DOWN", "L")

run_frames_and_pause(10)

keys_release("B", "START", "LEFT", "DOWN", "L")
keys_hold("A", "SELECT", "RIGHT", "UP", "R")

run_frames_and_pause(10)

screenshot("screenshot-1.png")

keys_hold("B", "START", "LEFT", "DOWN", "L")
keys_release("A", "SELECT", "RIGHT", "UP", "R")

run_frames_and_pause(10)

screenshot("screenshot-2.png")

exit()

return 0
