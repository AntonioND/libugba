-- Press some keys, release keys, repeat... and take screenshot

-- This isn't enough to trigger the interrupt of A+B
run_frames_and_pause(2)
keys_hold("A")

run_frames_and_pause(2)
keys_release("A")

-- This isn't the condition the program is waiting for
run_frames_and_pause(2)
keys_hold("RIGHT")

run_frames_and_pause(2)
keys_release("RIGHT")

-- Trigger A+B interrupt
run_frames_and_pause(2)
keys_hold("A", "B")

run_frames_and_pause(2)
keys_release("A", "B")

-- Trigger direction pad interrupt
run_frames_and_pause(2)
keys_hold("UP")

run_frames_and_pause(2)
keys_release("UP")

-- Trigger A+B interrupt
run_frames_and_pause(2)
keys_hold("A", "B")

run_frames_and_pause(2)
keys_release("A", "B")

-- Trigger direction pad interrupt with another button
run_frames_and_pause(2)
keys_hold("RIGHT")

run_frames_and_pause(2)
keys_release("RIGHT")

-- Trigger A+B interrupt
run_frames_and_pause(2)
keys_hold("A", "B")

run_frames_and_pause(2)
keys_release("A", "B")

-- This button isn't part of the direction pad
run_frames_and_pause(2)
keys_hold("START")

run_frames_and_pause(2)
keys_release("START")

run_frames_and_pause(2)
screenshot()

exit()

return 0
