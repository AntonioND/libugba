-- Test that runs a program for 10 frames, takes a screenshot, presses some
-- buttons to change the console output on the screen, and takes another
-- screenshot.

run_frames_and_pause(10)
screenshot("screenshot-1.png")
keys_hold("B")

run_frames_and_pause(3)
keys_release("B")
keys_hold("SELECT")

run_frames_and_pause(3)
keys_release("SELECT")

run_frames_and_pause(3)
screenshot("screenshot-2.png")

exit()

return 0
