-- Test that runs a program for 10 frames, takes a screenshot, presses some
-- buttons to change the console output on the screen, and takes another
-- screenshot.

run_frames_and_pause(10)
screenshot("screenshot-1.png")
set_input("B")
continue()

run_frames_and_pause(3)
set_input("SELECT")
continue()

run_frames_and_pause(3)
screenshot("screenshot-2.png")
continue()

exit()

return 0
