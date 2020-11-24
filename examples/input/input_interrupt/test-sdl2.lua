-- Press some keys, release keys, repeat... and take screenshot

-- This isn't enough to trigger the interrupt of A+B
run_frames_and_pause(2)
keys_hold("A")
continue()
run_frames_and_pause(2)
keys_release("A")
continue()

-- This isn't the condition the program is waiting for
run_frames_and_pause(2)
keys_hold("RIGHT")
continue()
run_frames_and_pause(2)
keys_release("RIGHT")
continue()

-- Trigger A+B interrupt
run_frames_and_pause(2)
keys_hold("A", "B")
continue()
run_frames_and_pause(2)
keys_release("A", "B")
continue()

-- Trigger direction pad interrupt
run_frames_and_pause(2)
keys_hold("UP")
continue()
run_frames_and_pause(2)
keys_release("UP")
continue()

-- Trigger A+B interrupt
run_frames_and_pause(2)
keys_hold("A", "B")
continue()
run_frames_and_pause(2)
keys_release("A", "B")
continue()

-- Trigger direction pad interrupt with another button
run_frames_and_pause(2)
keys_hold("RIGHT")
continue()
run_frames_and_pause(2)
keys_release("RIGHT")
continue()

-- Trigger A+B interrupt
run_frames_and_pause(2)
keys_hold("A", "B")
continue()
run_frames_and_pause(2)
keys_release("A", "B")
continue()

-- This button isn't part of the direction pad
run_frames_and_pause(2)
keys_hold("START")
continue()
run_frames_and_pause(2)
keys_release("START")
continue()

run_frames_and_pause(2)
screenshot()
continue()

exit()

return 0
